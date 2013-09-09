/*
 * This file is part of HiKoB Openlab.
 *
 * HiKoB Openlab is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, version 3.
 *
 * HiKoB Openlab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with HiKoB Openlab. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2011,2012 HiKoB.
 */

/**
 * \file fs.c
 * \date Aug 17, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "platform.h"
#include "sdio.h"
#include "buf_util.h"
#include "fs.h"
#include "printf.h"
#include "debug.h"

/**
 * \todo verify that when scanning the pool to find the less used buffer,
 * the result is consistent (i.e. after the loop the result is indeed the
 * less used buffer). Maybe it does not matter
 */

extern sdio_t sdio;

#define FS_POOL_SIZE  30

#define MAX_RETRY          3
#define MAX_FAILED_ATTEMPT 10

#define MAX_USE            254

// Buffer pool itself
static buffer_t pool[FS_POOL_SIZE];

// Semaphore indicating how much dirty buffers there is
static xSemaphoreHandle dirty_sem;

// Mutexes for resource management
static xSemaphoreHandle sd_access_mutex, sd_transfer_mutex;

// Mutex for thread waiting for a clean buffer to be available
static xSemaphoreHandle waiting_for_clean_mutex;
static xSemaphoreHandle waiting_mutex;
static uint16_t num_waiting;

// Prototype for write task function
static void vWriteTask(void *pvParameters);

// Error recovery
static uint16_t failed_attempt;

// SD card error
static sd_error_t transfer_error;

#define min(a,b) ((a)<(b)?(a):(b))

void transfer_handler(handler_arg_t arg)
{
    signed portBASE_TYPE pxHigherPriorityTaskWoken;

    transfer_error = (sd_error_t)arg;

    xSemaphoreGiveFromISR(sd_transfer_mutex, &pxHigherPriorityTaskWoken);

    if (pxHigherPriorityTaskWoken != pdFALSE)
    {
        portYIELD();
    }
}

fs_error_t fs_init()
{
    int i;

    // Initialize the minimum counting to the size of the pool,
    // set the counter to the min value at first so that the
    // first call on wait will be blocking (except if another
    // process give the semaphore just before)
    dirty_sem = xSemaphoreCreateCounting(FS_POOL_SIZE, 0);

    // Initialize SD card access mutex
    sd_access_mutex = xSemaphoreCreateMutex();
    sd_transfer_mutex = xSemaphoreCreateCounting(1, 0);

    // Initialize the "waiting for clean buffer" mutex
    waiting_for_clean_mutex = xSemaphoreCreateCounting(1, 0);
    waiting_mutex = xSemaphoreCreateMutex();
    num_waiting = 0;

    failed_attempt = 0;

    // Create all buffers mutexes and initializes the buffer descriptors
    for (i = 0; i < FS_POOL_SIZE; i++)
    {
        pool[i].vMutex = xSemaphoreCreateMutex();
        pool[i].vContentMutex = xSemaphoreCreateMutex();
        pool[i].dirty = false;
        pool[i].page = 0;
        pool[i].use = 0;
    }

    if (sd_init(sdio) != SD_NO_ERROR)
    {
        return FS_MEDIUM_INIT_ERROR;
    }

    sd_set_transfer_handler(sdio, transfer_handler);

    xTaskCreate(vWriteTask, (signed char *)"SDWrite", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

    return FS_OK;
}

buffer_t *reserve_buffer()
{
    int i, j = -1;
    uint8_t use_min = MAX_USE + 1;

    // Find the less used and not dirty buffer
    for (i = 0; i < FS_POOL_SIZE; i++)
    {
        // Lock the buffer while we are examining it
        xSemaphoreTake(pool[i].vMutex, portMAX_DELAY);

        // Only try if the buffer is not dirty, otherwise
        // it needs to be written on the micro SD card
        if (!pool[i].dirty)
        {
            // If the buffer is less used than the previous one...
            if (pool[i].use < use_min)
            {
                use_min = pool[i].use;

                // Release the previous buffer as we found a better one
                if (j != -1)
                {
                    xSemaphoreGive(pool[j].vMutex);
                }

                j = i;
                // Do not release the chosen buffer
                continue;
            }
        }

        // Release the buffer as it is not interesting (either dirty or more used)
        xSemaphoreGive(pool[i].vMutex);
    }

    if (j != -1)
    {
        // Return the chose buffer, it is still locked
        return &(pool[j]);
    }
    else
    {
        // If all buffers were dirty, return NULL
        return NULL;
    }
}

inline static void failed()
{
    failed_attempt++;

    if (failed_attempt > MAX_FAILED_ATTEMPT)
    {
        log_warning("Too many failed attempts, trying to reinit SD card");
        failed_attempt = 0;
        xSemaphoreTake(sd_access_mutex, portMAX_DELAY);

        if (sd_init(sdio) != SD_NO_ERROR)
        {
            log_error("Cannot reinit SD card");

            // Not recoverable error...
            HALT();
        }

        xSemaphoreGive(sd_access_mutex);
        log_info("SD card reinitialized");
    }
}

inline static void passed()
{
    failed_attempt = 0;
}

inline static sd_error_t safe_read(uint32_t page, uint8_t *buf)
{
    sd_error_t ret;

    // Take medium access lock
    xSemaphoreTake(sd_access_mutex, portMAX_DELAY);

    if (sd_get_type(sdio) != SDHC)
    {
        ret = sd_read_single_block(sdio, page * 512ul, buf);
    }
    else
    {
        ret = sd_read_single_block(sdio, page, buf);
    }

    if (ret == SD_NO_ERROR)
    {
        // Wait for DMA transfer to be completed
        xSemaphoreTake(sd_transfer_mutex, portMAX_DELAY);

        // Check if no error occured during transfer
        if (transfer_error != SD_NO_ERROR)
        {
            ret = FS_READ_FAILED;
        }
    }

    // Realease medium access lock
    xSemaphoreGive(sd_access_mutex);

    return ret;
}

inline static sd_error_t safe_write(buffer_t *buffer)
{
    sd_error_t ret = SD_NO_ERROR;

    xSemaphoreTake(sd_access_mutex, portMAX_DELAY);

    if (sd_get_type(sdio) != SDHC)
    {
        ret = sd_write_single_block(sdio, buffer->page * 512ul, buffer->content);
    }
    else
    {
        ret = sd_write_single_block(sdio, buffer->page, buffer->content);
    }

    if (ret == SD_NO_ERROR)
    {
        // As we wrote the page on the micro SD card, the flash page is
        // consistent with the buffer, so the page is not dirty anymore
        buffer->dirty = false;
        buffer->use = 1;

        // Wait for DMA transfer to be completed
        xSemaphoreTake(sd_transfer_mutex, portMAX_DELAY);

        // Check if no error occured during transfer
        if (transfer_error != SD_NO_ERROR)
        {
            ret = transfer_error;
        }
    }

    xSemaphoreGive(sd_access_mutex);

    return ret;
}

uint16_t fs_write(uint32_t page, uint16_t offset, uint8_t *buf, uint16_t size)
{
    uint32_t i;
    uint16_t s = min(size, 512 - offset);
    buffer_t *b;
    sd_error_t ret;

    while (true)
    {
        // Lookup for the page in the pool
        for (i = 0; i < FS_POOL_SIZE; i++)
        {
            // Wait for the header to be readable
            xSemaphoreTake(pool[i].vMutex, portMAX_DELAY);

            if (page == pool[i].page)
            {
                // If the buffer was not dirty, signal to the writing
                // thread that there is a new dirty buffer
                if (!pool[i].dirty)
                {
                    // If the buffer is not dirty, the writing thread is not using it
                    // so there is no need to take the content mutex
                    pool[i].dirty = true;

                    if (pool[i].use < MAX_USE)
                    {
                        pool[i].use++;
                    }

                    cpy(buf, pool[i].content + offset, s);

                    // Signal to the writing thread that there is a new dirty buffer
                    xSemaphoreGive(dirty_sem);

                    // Release the header lock
                    xSemaphoreGive(pool[i].vMutex);
                }
                else
                {
                    // The buffer is dirty, so the writing thread might be being
                    // writing it to the flash card. We need to take the content lock
                    xSemaphoreTake(pool[i].vContentMutex, portMAX_DELAY);

                    // If we got through the lock, we need to check if the buffer is
                    // still dirty, if not, it means that it has been written down to
                    // the flash card by the writing thread
                    if (pool[i].use < MAX_USE)
                    {
                        pool[i].use++;
                    }

                    cpy(buf, pool[i].content + offset, s);

                    // The buffer might not be dirty anymore as the writing thread
                    // might have written the buffer down to the flash while we were
                    // waiting on the vContentMutex
                    if (!pool[i].dirty)
                    {
                        pool[i].dirty = true;
                        // Signal to the writing thread that there is a new dirty buffer
                        xSemaphoreGive(dirty_sem);
                    }

                    // Release both mutexes
                    xSemaphoreGive(pool[i].vContentMutex);
                    xSemaphoreGive(pool[i].vMutex);
                }

                return s;
            }

            xSemaphoreGive(pool[i].vMutex);
        }

        // If we end up here, it means that the page is not loaded
        // First reserve a buffer
        b = reserve_buffer();

        if (b != NULL)
        {
            // If b is not NULL it means that we successfully allocated a buffer

            if (s != 512)
            {
                for (i = 0; i < MAX_RETRY; i++)
                {
                    // In this case, the page has to be loaded first
                    if ((ret = safe_read(page, b->content)) == SD_NO_ERROR)
                    {
                        passed();
                        break;
                    }

                    log_warning("Read attempt #%d failed: %d", i, ret);
                    failed();
                }

                if (ret != SD_NO_ERROR)
                {
                    // If there is an error, report it by telling the
                    // caller that no byte has been read
                    s = 0;
                }
            }

            if (s != 0)
            {
                // The buffer is already locked, just update the information
                b->page = page;
                b->dirty = true;
                b->use = 1;
                cpy(buf, b->content + offset, s);

                // Signal to the writing thread that there is a new dirty buffer
                xSemaphoreGive(dirty_sem);
            }

            // After updating the buffer, release the lock
            xSemaphoreGive(b->vMutex);

            return s;
        }
        else
        {
            // If b is NULL we've got a problem...
            // All buffers are dirty, so we need to wait until one clean buffer
            // is available. The buffer allocation will be hopefully done in the
            // next iteration
            log_warning("No buffer available");
            xSemaphoreTake(waiting_mutex, portMAX_DELAY);
            num_waiting++;
            xSemaphoreGive(waiting_mutex);

            xSemaphoreTake(waiting_for_clean_mutex, portMAX_DELAY);

            xSemaphoreTake(waiting_mutex, portMAX_DELAY);
            num_waiting--;
            xSemaphoreGive(waiting_mutex);
        }
    }

    return 0;
}

uint16_t fs_read(uint32_t page, uint16_t offset, uint8_t *buf, uint16_t size)
{
    uint32_t i;
    uint16_t s = min(size, 512 - offset);
    buffer_t *b;
    sd_error_t ret;

    while (true)
    {
        // Lookup for the page in the pool
        for (i = 0; i < FS_POOL_SIZE; i++)
        {
            xSemaphoreTake(pool[i].vMutex, portMAX_DELAY);

            if ((page == pool[i].page) && (pool[i].use != 0))
            {
                if (pool[i].use < MAX_USE)
                {
                    pool[i].use++;
                }

                cpy(pool[i].content + offset, buf, s);
                xSemaphoreGive(pool[i].vMutex);
                return s;
            }

            xSemaphoreGive(pool[i].vMutex);
        }

        // If the page is not already loaded, load it
        b = reserve_buffer();

        if (b != NULL)
        {
            // The buffer is already locked, just update the information
            b->page = page;
            b->dirty = false;
            b->use = 1;

            // Read the page
            for (i = 0; i < MAX_RETRY; i++)
            {
                if ((ret = safe_read(page, b->content)) == SD_NO_ERROR)
                {
                    // If the page is successfully read, copy the data
                    cpy(b->content + offset, buf, s);

                    passed();
                    break;
                }

                log_warning("Read attempt #%d failed: %d", i, ret);
                failed();
            }

            if (ret != SD_NO_ERROR)
            {
                // Else signal the error
                s = 0;
            }

            // After updating the buffer, release the lock
            xSemaphoreGive(b->vMutex);
        }
        else
        {
            // If b is NULL we've got a problem...
            // All buffers are dirty, so we need to wait until one clean buffer
            // is available. The buffer allocation will be hopefully done in the
            // next iteration
            log_warning("No buffer available");
            xSemaphoreTake(waiting_mutex, portMAX_DELAY);
            num_waiting++;
            xSemaphoreGive(waiting_mutex);

            xSemaphoreTake(waiting_for_clean_mutex, portMAX_DELAY);

            xSemaphoreTake(waiting_mutex, portMAX_DELAY);
            num_waiting--;
            xSemaphoreGive(waiting_mutex);
        }
    }

    return s;
}

void vWriteTask(void *pvParameters)
{
    int i, j;
    uint8_t use_min;
    sd_error_t ret;

    while (true)
    {
        // Wait for a buffer to become dirty
        xSemaphoreTake(dirty_sem, portMAX_DELAY);

        j = -1;
        use_min = MAX_USE + 1;

        // Find the less used and dirty buffer
        for (i = 0; i < FS_POOL_SIZE; i++)
        {
            // Lock the buffer while we are examining it
            xSemaphoreTake(pool[i].vMutex, portMAX_DELAY);

            // Only try if the buffer is dirty
            if (pool[i].dirty)
            {
                // If the buffer is less used than the previous one...
                if (pool[i].use < use_min)
                {
                    use_min = pool[i].use;

                    // Release the previous buffer as we found a better one
                    if (j != -1)
                    {
                        xSemaphoreGive(pool[j].vMutex);
                    }

                    j = i;
                    // Do not release the chosen buffer
                    continue;
                }
            }

            // Release the buffer as it is not interesting (either not dirty or more used than the previous one)
            xSemaphoreGive(pool[i].vMutex);
        }

        // Lock buffer content before unlocking buffer header
        xSemaphoreTake(pool[j].vContentMutex, portMAX_DELAY);
        xSemaphoreGive(pool[j].vMutex);

        // We found the best buffer to write on the micro SD card
        if ((ret = safe_write(&(pool[j]))) != SD_NO_ERROR)
        {
            // If write failed leave the buffer dirty and wait for another attempt
            xSemaphoreGive(dirty_sem);

            log_warning("Write attempt failed on buffer %d (page: %d): %d", j, pool[j].page, ret);
            failed();
        }
        else
        {
            passed();

            // Signal the thread waiting for a clean buffer (if any)
            // to be available that we just released one
            xSemaphoreTake(waiting_mutex, portMAX_DELAY);

            if (num_waiting > 0)
            {
                xSemaphoreGive(waiting_for_clean_mutex);
            }

            xSemaphoreGive(waiting_mutex);
        }

        // Do not forget to unlock the buffer content
        xSemaphoreGive(pool[j].vContentMutex);
    }
}
