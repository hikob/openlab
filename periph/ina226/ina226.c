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
 * ina226.c
 *
 *  Created on: Jul 12, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include <math.h>
#include "ina226_.h"
#include "ina226_registers.h"

#include "debug.h"

static uint16_t read_reg(uint8_t addr);
static void write_reg(uint8_t addr, uint16_t value);

static struct
{
    i2c_t i2c;
    uint8_t i2c_address;
    exti_line_t alert_line;

    float current_lsb;
} ina;

void ina226_init(i2c_t i2c, uint8_t address, exti_line_t alert_line)
{
    // Store the parameters
    ina.i2c = i2c;
    ina.i2c_address = address;
    ina.alert_line = alert_line;

    // Reset the chip
    write_reg(INA226_REG_CONFIGURATION, INA226_CONFIGURATION__RST);
}
void ina226_disable()
{
    // Clear the MODE bits of the configuration register
    uint16_t reg;
    reg = read_reg(INA226_REG_CONFIGURATION);
    reg &= ~INA226_CONFIGURATION__MODE_MASK;

    // Write the configuration value
    write_reg(INA226_REG_CONFIGURATION, reg);
}

void ina226_calibrate(float r_shunt, float max_current)
{
    // Compute the current LSB as max_expected_current/2**15
    float current_lsb = max_current / (1 << 15);

    // Compute calibration register as 0.00512 / (current_lsb * r_shunt)
    float calib = 0.00512 / (current_lsb * r_shunt);

    // Register is a 16bit unsigned integer, thus convert and round above
    uint16_t calib_reg = (uint16_t) floorf(calib);

    // Re-compute and store real current LSB
    ina.current_lsb = 0.00512 / (r_shunt * calib_reg);

    // Write calibration
    write_reg(INA226_REG_CALIBRATION, calib_reg);
}

void ina226_configure(ina226_sampling_period_t period,
        ina226_averaging_factor_t average)
{
    // Prepare register value;
    uint16_t reg;
    reg = (period << INA226_CONFIGURATION__BUS_CONV_TIME_SHIFT)
            & INA226_CONFIGURATION__BUS_CONV_TIME_MASK;
    reg |= (period << INA226_CONFIGURATION__SHUNT_CONV_TIME_SHIFT)
            & INA226_CONFIGURATION__SHUNT_CONV_TIME_MASK;
    reg |= (average << INA226_CONFIGURATION__AVG_SHIFT)
            & INA226_CONFIGURATION__AVG_MASK;
    reg |= INA226_CONFIGURATION__MODE_MASK;

    // Write the configuration value
    write_reg(INA226_REG_CONFIGURATION, reg);
}

void ina226_alert_enable(handler_t handler, handler_arg_t arg)
{
    // Enable DATA READY alert source
    write_reg(INA226_REG_MASK_ENABLE, 1 << 10);

    // Enable the alert line
    exti_set_handler(ina.alert_line, handler, arg);
    exti_enable_interrupt_line(ina.alert_line, EXTI_TRIGGER_FALLING);
}
void ina226_alert_disable()
{
    // Disable all alert sources
    write_reg(INA226_REG_MASK_ENABLE, 0);

    // Disable the alert line
    exti_disable_interrupt_line(ina.alert_line);
    exti_set_handler(ina.alert_line, NULL, NULL);
}
int ina226_conversion_ready()
{
    // Read the MASK ENABLE register and check for the ConversionReady flag
    return (read_reg(INA226_REG_MASK_ENABLE) & INA226_MASK_ENABLE__CVRF) != 0;
}

void ina226_read(float *voltage, float *current, float *power, float* shunt_voltage)
{
    uint16_t voltage_reg;
    // Read BUS voltage register
    voltage_reg = read_reg(INA226_REG_BUS_VOLTAGE);
    int16_t current_reg;
    // Read current register
    current_reg = (int16_t) read_reg(INA226_REG_CURRENT);
    int16_t power_reg;
    // Read POWER register
    power_reg = (int16_t) read_reg(INA226_REG_POWER);
    int16_t shunt_voltage_reg;
    // Read POWER register
    shunt_voltage_reg = (int16_t) read_reg(INA226_REG_SHUNT_VOLTAGE);

    // Read the mask/enable register to clear it
    (void) read_reg(INA226_REG_MASK_ENABLE);

    // Check for the requested measures and compute their values
    if (voltage)
    {
        // Convert to Volts
        *voltage = (float) voltage_reg * 1.25e-3;
    }
    if (current)
    {

        // Convert to Amperes
        *current = (float) current_reg * ina.current_lsb;
    }
    if (power)
    {

        // Convert to Watts
        *power = (float) power_reg * 25 * ina.current_lsb;
    }
    if (shunt_voltage)
    {
        // Convert to Volts
        *shunt_voltage = (float) shunt_voltage_reg * 2.5e-6;
    }
}

static uint16_t read_reg(uint8_t addr)
{
    uint8_t buf[2];

    // Write the register address
    buf[0] = addr;
    i2c_tx(ina.i2c, ina.i2c_address, buf, 1);

    // Read the value
    i2c_rx(ina.i2c, ina.i2c_address, buf, 2);

    return (buf[0] << 8) + buf[1];
}
static void write_reg(uint8_t addr, uint16_t value)
{
    // Prepare the 3 byte buffer
    uint8_t buf[3];
    buf[0] = addr;
    buf[1] = value >> 8;
    buf[2] = value & 0xFF;
    // Write the value
    i2c_tx(ina.i2c, ina.i2c_address, buf, 3);
}
