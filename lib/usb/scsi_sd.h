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
 * scsi_sd.h
 *
 *  Created on: Jun 17, 2012
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */


#ifndef __USB_SCSI_SD_H
#define __USB_SCSI_SD_H

void          scsi_sd_init          ( uint8_t lun );
void          scsi_sd_read_capacity ( uint8_t lun, uint32_t *bcount, uint16_t *bsize );
scsi_cmdret_t scsi_sd_read10        ( scsi_params_t scsi_params );
scsi_cmdret_t scsi_sd_write10       ( scsi_params_t scsi_params );

#endif
