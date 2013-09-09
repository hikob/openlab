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
 * Copyright (C) 2013 HiKoB.
 */

/** \example example_fat32.c
 * This file is an example of the use of FAT32 file system on SD cards.
 * \date Jan 4, 2013
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "platform.h"
#include "printf.h"
#include "debug.h"
#include "fat32.h"

static void fat32Task(void *pvParameters);

int main(void)
{
    // Initialize the platform
    platform_init();

    // Create the logging task
    xTaskCreate(fat32Task, (signed char *)"FAT32", configMINIMAL_STACK_SIZE, NULL, 3, NULL);

    // Initialize FAT32 FS library
    if (fat32_init() != FAT32_OK)
    {
        log_error("Error initializing FAT32");
        HALT();
    }

    // Start the scheduler
    platform_run();

    return 0;
}

static void fat32Task(void *pvParameters)
{
    static uint8_t str[] = "Hello world!!!\nFrom HiKoB Openlab\n";
    static uint8_t fb[512], buf[32];
    static file_t f;
    static uint16_t s, i;

    // Mount the FAT32 device
    if(fat32_mount() != FAT32_OK)
    {
        log_error("Unable to mount file system");
        HALT();
    }

    /** Creating and writing a file */

    // Create a file named TEST??.TXT
    if(file_create_next((uint8_t *)"TEST", (uint8_t *)"TXT", fb, &f) != FAT32_OK)
    {
        log_error("Unable to mount create file");
        HALT();
    }

    // Write the "Hello world" string to the file and close it
    file_write(f, str, sizeof(str) - 1);

    file_close(f);

    log_info("File write ended!");

    /** Opening and reading a file */

    // Open file "README.TXT"
    if(file_open((uint8_t*)"README.TXT", fb, &f) == FAT32_OK)
    {
        // Loop reading the file using a 32-byte buffer
        while(1)
        {
            s = sizeof(buf);

            file_read(f, buf, &s);

            // Print the buffer content
            for(i = 0; i < s; i++)
            {
                log_printf("%c", buf[i]);
            }

            // If we read less than 32 bytes it means that we reached the end of the file
            if(s != sizeof(buf))
            {
                log_info("Reading of README.TXT ended");
                file_close(f);
                break;
            }
        }
    }
    else
    {
        log_error("File README.TXT not found");
    }

    vTaskSuspend(NULL); // Suspend this task
    
    while(1);
}
