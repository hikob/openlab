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
 * Copyright (C) 2012 HiKoB.
 */

/*
 * scsi_mmapfs.c
 *
 *  Created on: Jun 19, 2012
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

/*
 * All FAT information gathered from
 * http://en.wikipedia.org/wiki/File_Allocation_Table
 */


#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "storage.h"
#include "scsi.h"
#include "scsi_mmapfs.h"


#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_WARNING
#include "printf.h"
#include "debug.h"

#if (LOG_LEVEL < LOG_LEVEL_DEBUG)
#define DBG(x...)   printf(x)
#else
#define DBG(x...)   do { } while(0)
#endif // LOG_LEVEL

#define FAT_TEST 0

#if FAT_TEST == 1
#define DUMP_READ(x...)   scsi_dump_blk(x)
#define DUMP_WRITE(x...)  scsi_dump_blk(x)
#else
#define DUMP_READ(x...)   do { } while (0)
#define DUMP_WRITE(x...)  do { } while (0)
#endif

#define BOOTSEC_TEST 0

#if BOOTSEC_TEST == 1
#define BOOT_READ(x...)   do {log_not_implemented("reading bootsector (lba : %d)", x); scsi_dump_blk(x);} while (0)
#else
#define BOOT_READ(x...)   do { } while (0)
#endif


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */

#define LUN_SECTORSIZE          512ul                                   // bytes
#define LUN_SIZE                ((256 * 1024 * 1024) / LUN_SECTORSIZE)  // #sectors


#define MEDIA_TYPE              0xf8             // F8 = Fixed disk
#define SECTORSIZE              LUN_SECTORSIZE   // 
#define TRACKSIZE               0x0039           // #sectors per track
#define HEADS                   0x0009           // # of heads
#define CLUSTERSIZE             8                // #sectors
#define RESERVED                8                // #

#define FAT_NUMBEROFFAT         1
#define FAT_FATSIZE             ((LUN_SIZE / CLUSTERSIZE) / (SECTORSIZE/2)) // #cluster per FAT == (LUN_SIZE / 256)
#define FAT_FAT1_FIRST          (RESERVED)
#define FAT_FAT1_LAST           (FAT_FAT1_FIRST + FAT_FATSIZE - 1)
#define FAT_FILE_DELETED        0xE5
#define FAT_DATE_DEFAULT        0x40d3           // June, 19th 2012

#define DIRECTORY_ENTRIES       0x0200
#define DIRECTORY_ENTRYSIZE     32
#define DIRECTORY_SIZE          (DIRECTORY_ENTRIES * DIRECTORY_ENTRYSIZE / LUN_SECTORSIZE)
#define DIRECTORY_FIRST         (FAT_FAT1_LAST + 1)
#define DIRECTORY_LAST          (DIRECTORY_FIRST + DIRECTORY_SIZE - 1) // sector

// DATA allocation. Cluster x in FAT maps to Cluster DATA_FIRST_CLUSTER + x
#define DATA_FIRST_SECTOR       (DIRECTORY_LAST + 1)                   // first sector containing DATA
#define DATA_FIRST_CLUSTER      (DATA_FIRST_SECTOR / CLUSTERSIZE)      // first cluster containing DATA
#define DATA_FIRST_FAT_CLUSTER  3                                      // clusters 0, 1 unusable, 2 is left unused

//#define DATA_SECTOR_SIZE        LUN_SIZE
#define DATA_SECTOR_SIZE        (LUN_SIZE - DATA_FIRST_SECTOR)
#define DATA_CLUSTER_SIZE       (LUN_SIZE / CLUSTERSIZE - DATA_FIRST_CLUSTER)
 
#if DATA_SECTOR_SIZE > 65535
  #define EBPB_SIZE16           0
  #define EBPB_SIZE32           DATA_SECTOR_SIZE
#else
  #define EBPB_SIZE16           DATA_SECTOR_SIZE
  #define EBPB_SIZE32           0
#endif

static inline uint32_t fat_cluster_to_sector(uint16_t fatcluster)
{
    // first two clusters are unused.
    return (fatcluster + DATA_FIRST_CLUSTER - 2) * CLUSTERSIZE;
}

static inline void write16(uint8_t *buf, uint16_t val)
{
    buf[0] = (val     ) & 0xff;
    buf[1] = (val >> 8) & 0xff;
}

static inline void write32(uint8_t *buf, uint32_t val)
{
    buf[0] = (val      ) & 0xff;
    buf[1] = (val >>  8) & 0xff;
    buf[2] = (val >> 16) & 0xff;
    buf[3] = (val >> 24) & 0xff;
}

#define FAT_EMPTYCLUSTER        0x0000
#define FAT_RESERVEDCLUSTER     0x0001
//                              0x0002 - 0xFFEF -> used cluster
//                              0xFFF0 - 0xFFF6 -> reserved values
#define FAT_BADCLUSTER          0xFFF7
//                              0xFFF8 - 0xFFFF -> used cluster, end of file
#define FAT_ENDOFCLUSTER        0xFFFF

#define DISK_LABEL             "HiKoB   "
#define SERIAL_NUMBER          'C','o','N','f'
#define PARTITION_LABEL        'H','i','K','o','B',' ',' ',' ',' ',' ',' '
#define FILESYSTEM_TYPE        'F','A','T','1','6',' ',' ',' '

/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */

void scsi_mmapfs_init(uint8_t lun)
{
}

/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */

static const uint8_t BootSector[] = 
{
// Jump instruction (3 * NOP) - Had to replace the first byte from 0x90 to 0xEB and second from 0x90 to 0x3C
    0xEB, 0x3C, 0x90, 
// Vendor string
    'm', 'k', 'd', 'o', 's', 'f', 's', 0,
// Extended Bios Parameter Block, offset 0x0b
    SECTORSIZE & 0xff, SECTORSIZE >> 8,        // 0x0b: Sector size 
    CLUSTERSIZE,                               // 0x0d: Logical sector per cluster
    RESERVED & 0xff, RESERVED >> 8,            // 0x0e: Reserved logical sector count
    FAT_NUMBEROFFAT,                           // 0x10: Number of File Allocation Tables.
    DIRECTORY_ENTRIES & 0xff, DIRECTORY_ENTRIES >> 8,  // 0x11: Maximum number root directory entries
    EBPB_SIZE16 & 0xff, EBPB_SIZE16 >> 8,      // 0x13: Total logical sectors 
    MEDIA_TYPE,                                // 0x15: Media descriptor
    FAT_FATSIZE & 0xff, FAT_FATSIZE >> 8,      // 0x16: Logical sectors per File Allocation Table for FAT12/FAT16
    TRACKSIZE   & 0xff, TRACKSIZE   >> 8,      // 0x18: Sectors per track
    HEADS       & 0xff, HEADS       >> 8,      // 0x1a: Number of heads
    0x00, 0x00, 0x00, 0x00,                    // 0x1c: hidden sectors
    EBPB_SIZE32       & 0xff, 
    EBPB_SIZE32 >>  8 & 0xff,
    EBPB_SIZE32 >> 16 & 0xff,
    EBPB_SIZE32 >> 24 & 0xff,                  // 0x20: Total logical sector if > 65535 else see [0x13]
// the following part is specific to FAT12/16
    0x00,                                      // 0x24: Physical drive number
    0x00,                                      // 0x25: Reserved
    0x29,                                      // 0x26: Extended Boot Signature
    SERIAL_NUMBER,                             // 0x27: Disk serial number 
    PARTITION_LABEL,                           // 0x2b: Partition volume label
    FILESYSTEM_TYPE,                           // 0x36: Filesystem type
// Boot code @
    // This byte is the boot sector jump target when starting with 
    // 0xEB 0x3C 0x90
};

static void build_bootsector(const mmapfs_t *mmapfs, uint16_t lba, uint8_t *sector)
{
    memset( sector,          0, SECTORSIZE);
    DBG("b");

    if (lba == 0)
    {
	memcpy( sector, BootSector, sizeof(BootSector));
	// signature
	sector[0x01FE] = 0x55;
	sector[0x01FF] = 0xAA;

	log_info("mmapfs read BootSector %d",lba);
	BOOT_READ(lba, sector, SECTORSIZE);

	DBG("Structure:\n");
	DBG("Sector size %d, Cluster size %d\n",CLUSTERSIZE,SECTORSIZE);
	DBG("BootSector      %03d - %03d / \n",0,FAT_FAT1_FIRST - 1);
	DBG("FAT1            %03d - %03d / size %d\n",FAT_FAT1_FIRST,FAT_FAT1_LAST,FAT_FATSIZE);
	DBG("RootDirectory   %03d - %03d / size %d\n",DIRECTORY_FIRST,DIRECTORY_LAST,DIRECTORY_SIZE);
	DBG("DATA            %03d -     / cluster %03d - \n",DATA_FIRST_SECTOR,DATA_FIRST_CLUSTER);
    }
    else
    {
	BOOT_READ(lba, sector, SECTORSIZE);
    }
}

/*
 * 12 bits File Allocation Table - FAT12
 * -------------------------------------
 *
 *  3bytes for 2 sectors, little endian
 *    
 *    even   odd                FAT
 *   0x0ABC 0x0DEF  ->   0xBC  0xFA  0xDE
 *
 * 
 * static const uint8_t FAT[] =    
 * {
 *
 * //    0x0F8F   0x0FFF
 * //    0x0000   0x0004
 * //    0x0FFF   0x0006
 * //    0x0007   0x0008
 *
 *    MEDIA_TYPE, 0xFF,   0xFF,    // 0 & 1 are unusable, 2 is unused
 *    0x00,       0x40,   0x00,    // clusters 3 & 4 = EEPROM.BIN
 *    0xFF,       0x6F,   0x00,    // CONFIG.TXT starts at cluster 5
 *    0x07,       0x80,   0x00,
 *    [...]  
 * };
 *
 * 16 bits File Allocation Table - FAT16
 * -------------------------------------
 * 
 * 2 bytes per sector, little endian
 *
 *
 * first FAT sector can allocate BLOCKSIZE / 2 - 3 clusters
 *
 */

static void build_fat(const mmapfs_t *mmapfs, uint16_t lba, uint8_t *sector)
{
    int i,c;
    int fat_sect    = 0;                          // current FAT sector
    int fat_off     = DATA_FIRST_FAT_CLUSTER * 2; // current FAT offset (byte) > 6
    uint16_t cstart = DATA_FIRST_FAT_CLUSTER;     // current cluster start for file
    uint16_t cend   = cstart - 1;                 // max cluster end for file
    uint16_t csize;                               // max file size in cluster
    uint16_t c2size,c2end;                        // actual cluster size and end.
    uint16_t mk;

    int      present;
    uint32_t size;

    memset(sector, 0, LUN_SECTORSIZE);
    for(i=0; i < SECTORSIZE; i+=2)
    {
	write16( &sector[i], FAT_BADCLUSTER);
    }

    DBG("f");
    /* FAT16 start */
    if (lba == 0)
    {
	sector[0] = MEDIA_TYPE;
	sector[1] = 0xFF;
	sector[2] = FAT_ENDOFCLUSTER & 0xff;
	sector[3] = FAT_ENDOFCLUSTER >> 8;
	//sector[4] = 0; 
	//sector[5] = 0;
    }

    for( i = 0 ; i < mmapfs->size ; i++)
    {
	if (mmapfs->files[i].info)
	{
	    mmapfs->files[i].info( mmapfs->files[i].arg, &present, &size);
	}
	else
	{
	    present = 1;
	    size    = mmapfs->files[i].maxsize;
	}

	// max size and end
	csize   = mmapfs->files[i].maxsize / (CLUSTERSIZE * SECTORSIZE);
	csize  += (csize * (CLUSTERSIZE * SECTORSIZE)) < mmapfs->files[i].maxsize;
	cend   += csize;
	
	// actual size and end
	c2size  =  size / (CLUSTERSIZE * SECTORSIZE);
	c2size += (size * (CLUSTERSIZE * SECTORSIZE)) < size;
	c2end   = cstart + c2size - 1;


	DBG("FAT %8s.%3s size %5d, cluster %3d.%3d.%3d, max %3d.%3d.%3d",
		  mmapfs->files[i].name,mmapfs->files[i].ext,mmapfs->files[i].maxsize,
		  cstart,csize,cend,cstart,c2size,c2end);

	for( c = cstart ; c <= cend ; c++ )
	{
	    if (fat_sect == lba)
	    {
		if (c < c2end)
		{
		    mk = c;
		}
		else if (c == c2end)
		{
		    mk = FAT_ENDOFCLUSTER;
		}
		else
		{
		    mk = FAT_BADCLUSTER;
		}

		sector[fat_off    ] =  mk       & 0xff;
		sector[fat_off + 1] = (mk >> 8) & 0xff;
	    }

	    fat_off += 2;

	    if (fat_off >= (SECTORSIZE/2))
	    {
		fat_sect ++;
		fat_off  = 0;
	    }
	}
        // prepare next round
	cstart = cend + 1;
    }

    if (lba == 0) // we juste finished the requested sector
    {
	log_info("mmapfs read FAT sector %d",c);
	DUMP_READ(lba, sector, SECTORSIZE);
    }
}

/*
 * RootDirectory
 *
 * 1: Volume label
 * 2: EEPROM.BIN,   4kB, clusters 3 & 4 
 * 3: CONFIG.TXT,  50kB, clusters 5 to ..
 *
 *
 * time & date format:
 *    15-11 	Hours (0-23)
 *    10-5 	Minutes (0-59)
 *    4-0 	Seconds/2 (0-29)
 * date:  0xD3, 0x40 == 0x40D3 == [2012,jun,19]
 *    15-9 	Year (0 = 1980)
 *    8-5 	Month (1–12)
 *    4-0 	Day (1–31)
 */

typedef enum {
    MMAPFS_BUILD,
    MMAPFS_CHECK,
} rootdir_mode_t;


static void build_rootdirectory(rootdir_mode_t mode, const mmapfs_t *mmapfs, uint16_t lba, uint8_t *sector)
{
    int i;
    int offset;
    uint16_t date;

    uint16_t cstart = DATA_FIRST_FAT_CLUSTER;     // current cluster start for file
    uint16_t cend   = cstart - 1;                 // current cluster end for file
    uint16_t csize;

    int      present;
    uint32_t size;

    DBG("r");
    if (mode == MMAPFS_BUILD)
    {
	memset(sector, 0, LUN_SECTORSIZE);
	if (lba == 0) // DISKLABEL
	{
	    memcpy(sector    , DISK_LABEL, 8);
	    memcpy(sector + 8, "   ",      3);
	    sector[0x0b] = 0x08;

	    write16( &sector[0x10], FAT_DATE_DEFAULT); // create date
	    write16( &sector[0x12], FAT_DATE_DEFAULT); // last access
	    write16( &sector[0x18], FAT_DATE_DEFAULT); // last modified
	    
	    offset = DIRECTORY_ENTRYSIZE;
	}
    }

    if (lba != 0)
    {
	//log_error("mmapfs unimplemented rootdirectory sector > 0 (%d)",lba);
	return ;
    }

    for(i=0; i< mmapfs->size; i++)
    {
	date = FAT_DATE_DEFAULT;

	if (mmapfs->files[i].info)
	{
	    mmapfs->files[i].info( mmapfs->files[i].arg, &present, &size);
	}
	else
	{
	    present = 1;
	    size    = mmapfs->files[i].maxsize;
	}

	csize  = mmapfs->files[i].maxsize / (CLUSTERSIZE * SECTORSIZE);
	csize += (csize * (CLUSTERSIZE * SECTORSIZE)) < mmapfs->files[i].maxsize;
	cend  += csize;

	DBG("RootDirectory %s.%s size %d start %d end %d (%d clusters)",
		  mmapfs->files[i].name,mmapfs->files[i].ext,mmapfs->files[i].maxsize,
		  cstart,cend,csize);

	switch (mode) 
	{
	case MMAPFS_BUILD: 
	    // name and extension
	    memcpy(& sector[ offset     ], mmapfs->files[i].name, 8);
	    if (present == 0)
	    {
		sector[offset] = FAT_FILE_DELETED;
	    }
	    memcpy(& sector[ offset + 8 ], mmapfs->files[i].ext,  3);
	    // creation, read, write dates
	    write16( &sector[offset + 0x10], date);
	    write16( &sector[offset + 0x12], date);
	    write16( &sector[offset + 0x18], date);
	    // start cluster
	    write16( &sector[offset + 0x1A], cstart);
	    // size
	    write32( &sector[offset + 0x1C], size);
	    break;
	case MMAPFS_CHECK:
	    if ((present) && (sector[offset] == FAT_FILE_DELETED))
	    {
		if (mmapfs->files[i].unlink)
		{
		    mmapfs->files[i].unlink( mmapfs->files[i].arg );
		}
	    }
	    break;
	}

	cstart = cend + 1;
	offset = offset + DIRECTORY_ENTRYSIZE;
    }

    if (lba == 0)
    {
	if (mode == MMAPFS_BUILD)
	{
	    log_info("mmapfs read RootDirectory sector %d",lba);
	}
	DUMP_READ(lba, sector, SECTORSIZE);
    }
}

/*
 *
 *
 *
 *
 */

typedef enum {
    MMAPFS_READ,
    MMAPFS_WRITE,
} data_mode_t;

static void build_data(data_mode_t mode, const mmapfs_t *mmapfs, uint16_t lba, uint8_t *sector)
{
    int i;
    uint16_t cstart = DATA_FIRST_FAT_CLUSTER;     // current cluster start for file
    uint16_t cend   = DATA_FIRST_FAT_CLUSTER - 1; // current cluster end for file
    uint16_t csize;
    uint32_t sstart,send;

    // log_info("mmapfs data sector %d",lba);
    // memset(sector,0,SECTORSIZE);

    for(i=0; i < mmapfs->size; i++)
    {
	csize  = mmapfs->files[i].maxsize / (CLUSTERSIZE * SECTORSIZE);
	csize += (csize * (CLUSTERSIZE * SECTORSIZE)) < mmapfs->files[i].maxsize;
	cend  += csize;

	sstart  = fat_cluster_to_sector( cstart );
	send    = fat_cluster_to_sector( cend   ) + CLUSTERSIZE - 1;

	// the requested sector is in the file
	if ((lba >= sstart) && (lba <= send))
	{
	    switch (mode)
	    {
	    case MMAPFS_READ:
		DBG("mmapfs read %s.%s size %d start %d end %d (%d clusters) :: lba %d cluster %d",
			  mmapfs->files[i].name,mmapfs->files[i].ext,mmapfs->files[i].maxsize,
			  cstart, cend, csize, lba, lba / CLUSTERSIZE);
		mmapfs->files[i].read( mmapfs->files[i].arg, lba - sstart, sector);
		break;
	    case MMAPFS_WRITE:
		DBG("mmapfs write %s.%s size %d start %d end %d (%d clusters) :: lba %d cluster %d",
			  mmapfs->files[i].name,mmapfs->files[i].ext,mmapfs->files[i].maxsize,
			  cstart, cend, csize, lba, lba / CLUSTERSIZE);
		mmapfs->files[i].write( mmapfs->files[i].arg, lba - sstart, sector);
		break;
	    }
	    return;
	}
	cstart = cend + 1;
    }
    
    memset(sector,0,SECTORSIZE);
    //log_error("mmapfs file not found lba %d cluster %d",lba,lba / CLUSTERSIZE);
}


/* ********************************************************************** */
/* READ CAPACITY(10), MMC5 page 435                                       */
/* ********************************************************************** */


void scsi_mmapfs_read_capacity(uint8_t lun, uint32_t *bcount, uint16_t *bsize) 
{
    *bcount =  LUN_SIZE;
    *bsize  =  LUN_SECTORSIZE;
}


/* ********************************************************************** */
/* READ10, MMC5 page 425                                                  */
/* ********************************************************************** */


scsi_cmdret_t scsi_mmapfs_read10(scsi_params_t scsi_params)
{
    const mmapfs_t *mmapfs;
    mmapfs    = (const mmapfs_t*)scsi_lun[scsi_params.lun].info;

    scsi_cdb10_t   *cdb10    = (scsi_cdb10_t *)scsi_params.cmd;
    //uint8_t       DPO      = (cdb10->cdb_info >> 4) & 0x1;
    //uint8_t       FUA      = (cdb10->cdb_info >> 3) & 0x1;
    uint32_t        lba      = msbtohost32(cdb10->lba);
    uint32_t        nblocks  = msbtohost16(cdb10->length);
    
    scsi_cmdret_t   ret;

    if ((lba + nblocks - 1) >= LUN_SIZE)
    {
	log_error("SCSI Read(10), mmapfs read beyond limit (limit 0x%08x, request 0x%08x / +%d)",LUN_SIZE,lba,nblocks);
	*scsi_params.status = SCSI_CHECK_CONDITION;
	return SCSI_CMD_DONE;
    }

    if (scsi_params.datamax < LUN_SECTORSIZE)
    {
	log_error("SCSI Read(10), mmapfs read buffer is smaller than a single sector (%d/%d bytes)",scsi_params.datalen,LUN_SECTORSIZE);
	*scsi_params.status = SCSI_CHECK_CONDITION;
	return SCSI_CMD_ERROR;
    }

    DBG("R%d",lba);

    // Boot and reserved sectors
    if ((lba >= 0) && (lba < FAT_FAT1_FIRST))
    {
	build_bootsector(mmapfs,lba,scsi_params.data);
    } 
    // FAT 1
    else if ((lba >= FAT_FAT1_FIRST) && (lba <= FAT_FAT1_LAST))
    {
	build_fat(mmapfs,lba - FAT_FAT1_FIRST,scsi_params.data);
    } 
    // RootDirectory
    else if ((lba >= DIRECTORY_FIRST) && (lba <= DIRECTORY_LAST))
    {
	build_rootdirectory(MMAPFS_BUILD, mmapfs,lba - DIRECTORY_FIRST,scsi_params.data);
    }
    // Data
    else
    {
	build_data(MMAPFS_READ, mmapfs,lba,scsi_params.data);
    }

    DBG(" ");


    *scsi_params.datalen = scsi_params.datamax;

    switch (nblocks)
    {
    case 0:
	*scsi_params.status     = SCSI_CHECK_CONDITION;
	ret            = SCSI_CMD_ERROR;
	break;
    case 1:
	*scsi_params.datalen    = LUN_SECTORSIZE;
	*scsi_params.status     = SCSI_GOOD;
	ret            = SCSI_CMD_DONE;
	break;
    default:
	cdb10->lba     = msbtohost32( lba     + 1 );
	cdb10->length  = msbtohost16( nblocks - 1 );
	*scsi_params.datalen    = LUN_SECTORSIZE;
	*scsi_params.status     = SCSI_GOOD;
	ret            = SCSI_CMD_PARTIAL;
	break;
    }
    return ret;
}


/* ********************************************************************** */
/* WRITE10, MMC5 page 425                                                  */
/* ********************************************************************** */


scsi_cmdret_t scsi_mmapfs_write10( scsi_params_t scsi_params)
{
    const mmapfs_t *mmapfs;
    mmapfs    = (const mmapfs_t*)scsi_lun[scsi_params.lun].info;

    scsi_cdb10_t   *cdb10    = (scsi_cdb10_t *)scsi_params.cmd;
    //uint8_t       DPO      = (cdb10->cdb_info >> 4) & 0x1;
    //uint8_t       FUA      = (cdb10->cdb_info >> 3) & 0x1;
    uint32_t        lba      = msbtohost32(cdb10->lba);
    uint32_t        nblocks  = msbtohost16(cdb10->length);
    scsi_cmdret_t   ret;


    if ((lba + nblocks - 1) >= LUN_SIZE)
    {
	log_error("SCSI Write(10), mmapfs write beyond limit (limit 0x%08x, request 0x%08x / +%d)",LUN_SIZE,lba,nblocks);
	*scsi_params.status = SCSI_CHECK_CONDITION;
	return SCSI_CMD_DONE;
    }

    if (scsi_params.datamax < LUN_SECTORSIZE)
    {
	log_error("SCSI Write(10), mmapfs write buffer is smaller than a single sector (%d/%d bytes)",scsi_params.datamax,LUN_SECTORSIZE);
	*scsi_params.status = SCSI_CHECK_CONDITION;
	return SCSI_CMD_ERROR;
    }

    DBG("W%d-",lba);

    
    if (scsi_params.cont == 0)
    {
	// log_info("SCSI Start Write(10) mmapfs, lba 0x%08x, length %d",lba,nblocks);
	*scsi_params.status = SCSI_GOOD;
	return SCSI_CMD_PARTIAL;
    }
    else
    {
	// Boot and reserved sectors
	if ((lba >= 0) && (lba < FAT_FAT1_FIRST))
	{
	    log_error("mmapfs write to Boot sector %d",lba );
	    DUMP_WRITE(lba, scsi_params.data, SECTORSIZE);
	} 
	// FAT 1
	else if ((lba >= FAT_FAT1_FIRST) && (lba <= FAT_FAT1_LAST))
	{
	    log_info("mmapfs write to FAT sector %d",lba - FAT_FAT1_FIRST);
	    DUMP_WRITE(lba, scsi_params.data, SECTORSIZE);
	} 
	// RootDirectory
	else if ((lba >= DIRECTORY_FIRST) && (lba <= DIRECTORY_LAST))
	{
	    build_rootdirectory(MMAPFS_CHECK, mmapfs,lba - DIRECTORY_FIRST,scsi_params.data);
	}
	// Data
	else
	{
	    build_data(MMAPFS_WRITE, mmapfs,lba,scsi_params.data);
	}
    }


    switch (nblocks)
    {
    case 0:
	*scsi_params.status   = SCSI_CHECK_CONDITION;
	ret            = SCSI_CMD_ERROR;
	break;
    case 1:
	*scsi_params.datalen  = LUN_SECTORSIZE;
	*scsi_params.status   = SCSI_GOOD;
	ret            = SCSI_CMD_DONE;
	break;
    default:
	cdb10->lba     = msbtohost32( lba     + 1 );
	cdb10->length  = msbtohost16( nblocks - 1 );
	*scsi_params.datalen  = LUN_SECTORSIZE;
	*scsi_params.status   = SCSI_GOOD;
	ret            = SCSI_CMD_PARTIAL;
	break;
    }
    return ret;
}


/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
