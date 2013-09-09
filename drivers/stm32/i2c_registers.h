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

/*
 * i2c_registers.h
 *
 *  Created on: Jul 31, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef I2C_REGISTERS_H_
#define I2C_REGISTERS_H_

static inline volatile uint16_t *i2c_get_CR1(const _i2c_t *_i2c)
{
    return mem_get_reg16(_i2c->base_address + I2Cx_CR1_OFFSET);
}

static inline volatile uint16_t *i2c_get_CR2(const _i2c_t *_i2c)
{
    return mem_get_reg16(_i2c->base_address + I2Cx_CR2_OFFSET);
}

static inline volatile uint16_t *i2c_get_OAR1(const _i2c_t *_i2c)
{
    return mem_get_reg16(_i2c->base_address + I2Cx_OAR1_OFFSET);
}

static inline volatile uint16_t *i2c_get_OAR2(const _i2c_t *_i2c)
{
    return mem_get_reg16(_i2c->base_address + I2Cx_OAR2_OFFSET);
}

static inline volatile uint16_t *i2c_get_DR(const _i2c_t *_i2c)
{
    return mem_get_reg16(_i2c->base_address + I2Cx_DR_OFFSET);
}

static inline volatile uint16_t *i2c_get_SR1(const _i2c_t *_i2c)
{
    return mem_get_reg16(_i2c->base_address + I2Cx_SR1_OFFSET);
}

static inline volatile uint16_t *i2c_get_SR2(const _i2c_t *_i2c)
{
    return mem_get_reg16(_i2c->base_address + I2Cx_SR2_OFFSET);
}

static inline volatile uint16_t *i2c_get_CCR(const _i2c_t *_i2c)
{
    return mem_get_reg16(_i2c->base_address + I2Cx_CCR_OFFSET);
}

static inline volatile uint16_t *i2c_get_TRISE(const _i2c_t *_i2c)
{
    return mem_get_reg16(_i2c->base_address + I2Cx_TRISE_OFFSET);
}

enum
{
    I2C_CR1__PE        = 0x0001,
    I2C_CR1__SMBUS     = 0x0002,
    I2C_CR1__SMBUSTYPE = 0x0008,
    I2C_CR1__ENARP     = 0x0010,
    I2C_CR1__ENPEC     = 0x0020,
    I2C_CR1__ENGC      = 0x0040,
    I2C_CR1__NOSTRETCH = 0x0080,
    I2C_CR1__START     = 0x0100,
    I2C_CR1__STOP      = 0x0200,
    I2C_CR1__ACK       = 0x0400,
    I2C_CR1__POS       = 0x0800,
    I2C_CR1__PEC       = 0x1000,
    I2C_CR1__ALERT     = 0x2000,
    I2C_CR1__SWRST     = 0x8000
};

enum
{
    I2C_CR2__FREQ_MASK = 0x003F,
    I2C_CR2__ITERREN   = 0x0100,
    I2C_CR2__ITEVTEN   = 0x0200,
    I2C_CR2__ITBUFEN   = 0x0400,
    I2C_CR2__DMAEN     = 0x0800,
    I2C_CR2__LAST      = 0x1000
};

enum
{
    I2C_SR1__SB       = 0x0001,
    I2C_SR1__ADDR     = 0x0002,
    I2C_SR1__BTF      = 0x0004,
    I2C_SR1__ADD10    = 0x0008,
    I2C_SR1__STOPF    = 0x0010,
    I2C_SR1__RXNE     = 0x0040,
    I2C_SR1__TXE      = 0x0080,
    I2C_SR1__BERR     = 0x0100,
    I2C_SR1__ARLO     = 0x0200,
    I2C_SR1__AF       = 0x0400,
    I2C_SR1__OVR      = 0x0800,
    I2C_SR1__PECERR   = 0x1000,
    I2C_SR1__TIMEOUT  = 0x4000,
    I2C_SR1__SMBALERT = 0x8000
};

enum
{
    I2C_SR2__MSL        = 0x0001,
    I2C_SR2__BUSY       = 0x0002,
    I2C_SR2__TRA        = 0x0004,
    I2C_SR2__GENCALL    = 0x0010,
    I2C_SR2__SMBDEFAULT = 0x0020,
    I2C_SR2__SMBHOST    = 0x0040,
    I2C_SR2__DUALF      = 0x0080,
    I2C_SR2__PEC_MASK   = 0xFF00
};

enum
{
    I2C_CCR__CCR_MASK = 0x0FFF,
    I2C_CCR__DUTY     = 0x4000,
    I2C_CCR__FS       = 0x8000
};

enum
{
    I2C_TRISE__TRISE_MASK = 0x1F
};

#endif /* I2C_REGISTERS_H_ */
