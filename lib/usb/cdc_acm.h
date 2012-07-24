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
 * cdc_acm.c
 *
 *  Created on: Aug 30, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 *              Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */


#ifndef __USB_CDC_ACM_H
#define __USB_CDC_ACM_H

extern const usb_profile_t usb_cdc_acm;

/**
 * USB CDC ACM Application Interface 
 */

typedef void (*cdc_acm_data_callback_t)(uint8_t *buf, uint16_t len);

void cdc_acm_register_rx_callback(cdc_acm_data_callback_t cb_rx);

uint16_t cdc_acm_send(uint8_t *buf, uint16_t len);

#endif
