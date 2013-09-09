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
 * unique_id.c
 *
 *  Created on: Sept 20, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>, Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#include "unique_id.h"
#include "printf.h"

uid_t uuid;
const uid_t const *uid = &uuid;

void uid_print()
{
    printf("UID32:\n\t%x\n\t%x\n\t%x\n", uid->uid32[0], uid->uid32[1],
           uid->uid32[2]);
    printf("UID16:\n\t%x\n\t%x\n\t%x\n\t%x\n\t%x\n\t%x\n",
           uid->uid16[0], uid->uid16[1], uid->uid16[2],
           uid->uid16[3], uid->uid16[4], uid->uid16[5]);
    printf("UID8:\n\t%x\n\t%x\n\t%x\n\t%x\n\t%x\n\t%x\n\t%x\n\t%x\n\t%x\n\t%x\n\t%x\n\t%x\n",
           uid->uid8[0], uid->uid8[1], uid->uid8[2],
           uid->uid8[3], uid->uid8[4], uid->uid8[5],
           uid->uid8[6], uid->uid8[7], uid->uid8[8],
           uid->uid8[9], uid->uid8[10], uid->uid8[11]);
}
