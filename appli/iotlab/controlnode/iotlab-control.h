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
/*
 * iotlab-control.h
 *
 *  Created on: Aug 12, 2013
 *      Author: burindes
 */

#ifndef IOTLAB_CONTROL_H_
#define IOTLAB_CONTROL_H_

/** Start the control library */
void iotlab_control_start();

/**
 * Convert a local time to an experiment time.
 *
 * \param timestamp the local time to convert, obtained from soft_timer_time()
 * \return the experiment time corresponding.
 */
uint32_t iotlab_control_convert_time(uint32_t timestamp);

#endif /* IOTLAB_CONTROL_H_ */
