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
 * \file fiteco_lib_gwt.c
 * \date Nov 30, 2012
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "fiteco_lib_gwt_.h"
#include "fiteco_lib_gwt.h"
#include "ina226.h"

#include "event.h"

static void current_sample_ready_isr(handler_arg_t arg);
static void process_current_sample(handler_arg_t arg);

static struct
{
    const fiteco_lib_gwt_config_t *config;

    fiteco_lib_gwt_current_monitor_handler_t handler;
} gwt;

void fiteco_lib_gwt_set_config(const fiteco_lib_gwt_config_t* config)
{
    gwt.config = config;

    // Initialize GPIOs
    gpio_set_output(gwt.config->main_power_gpio, gwt.config->main_power_pin);
    gpio_set_output(gwt.config->battery_power_gpio,
            gwt.config->battery_power_pin);
    gpio_set_output(gwt.config->battery_charge_gpio,
            gwt.config->battery_charge_pin);

    // Set default mode
    fiteco_lib_gwt_opennode_power_select(FITECO_GWT_OPENNODE_POWER__MAIN);
    fiteco_lib_gwt_battery_charge_enable();
}

void fiteco_lib_gwt_current_monitor_select(
        fiteco_lib_gwt_current_monitor_selection_t selection,
        fiteco_lib_gwt_current_monitor_handler_t handler, handler_arg_t arg)
{
    // Store handler
    gwt.handler = handler;

    switch (selection)
    {
        case FITECO_GWT_CURRENT_MONITOR__OFF:
            adg759_disable(gwt.config->current_mux);
            ina226_disable();
            return;

        case FITECO_GWT_CURRENT_MONITOR__OPEN_3V:
            adg759_enable(gwt.config->current_mux);
            adg759_select(gwt.config->current_mux, ADG759_INPUT_1);

            // Calibrate (RShut = 1 Ohm, Imax = 50mA)
            ina226_calibrate(1, 0.05f);
            break;

        case FITECO_GWT_CURRENT_MONITOR__OPEN_5V:
            adg759_enable(gwt.config->current_mux);
            adg759_select(gwt.config->current_mux, ADG759_INPUT_2);

            // Calibrate (RShut = 0.082 Ohm, Imax = 500mA)
            ina226_calibrate(0.082f, 0.5f);
            break;

        case FITECO_GWT_CURRENT_MONITOR__BATTERY:
            adg759_enable(gwt.config->current_mux);
            adg759_select(gwt.config->current_mux, ADG759_INPUT_3);

            // Calibrate (RShut = 1 Ohm, Imax = 800mA)
            ina226_calibrate(0.082f, 0.5f);
            break;
    }

    ina226_configure(INA226_PERIOD_1100us, INA226_AVERAGE_256);
    ina226_alert_enable(current_sample_ready_isr, arg );
}

static void current_sample_ready_isr(handler_arg_t arg)
{
    event_post_from_isr(EVENT_QUEUE_APPLI, process_current_sample, arg);
}
static void process_current_sample(handler_arg_t arg)
{
    // Read INA values
    float u, i, p, sv;
    ina226_read(&u, &i, &p, &sv);

    // Call handler
    if (gwt.handler)
    {
        gwt.handler(arg, u, i, p, sv);
    }
}

void fiteco_lib_gwt_opennode_power_select(
        fiteco_lib_gwt_opennode_power_selection_t selection)
{
    gpio_pin_set(gwt.config->main_power_gpio, gwt.config->main_power_pin);
    gpio_pin_set(gwt.config->battery_power_gpio, gwt.config->battery_power_pin);

    switch (selection)
    {
        case FITECO_GWT_OPENNODE_POWER__OFF:
            // Disable both power sources
            gpio_pin_set(gwt.config->main_power_gpio,
                    gwt.config->main_power_pin);
            gpio_pin_set(gwt.config->battery_power_gpio,
                    gwt.config->battery_power_pin);
            break;
        case FITECO_GWT_OPENNODE_POWER__MAIN:
            // Enable main, disable battery
            gpio_pin_clear(gwt.config->main_power_gpio,
                    gwt.config->main_power_pin);
            gpio_pin_set(gwt.config->battery_power_gpio,
                    gwt.config->battery_power_pin);
            break;
        case FITECO_GWT_OPENNODE_POWER__BATTERY:
            // Disable main, enable battery
            gpio_pin_set(gwt.config->main_power_gpio,
                    gwt.config->main_power_pin);
            gpio_pin_clear(gwt.config->battery_power_gpio,
                    gwt.config->battery_power_pin);
            break;
        default:
            // Unknown mode
            return;
    }
}

void fiteco_lib_gwt_battery_charge_enable()
{
    gpio_pin_clear(gwt.config->battery_charge_gpio,
            gwt.config->battery_charge_pin);
}
void fiteco_lib_gwt_battery_charge_disable()
{
    gpio_pin_set(gwt.config->battery_charge_gpio,
            gwt.config->battery_charge_pin);
}
