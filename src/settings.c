/* @file
 *
 *  "Planter" is a device that control a houseplant's water and light schedules.
 *  Copyright (C) 2018  Jon Sangster
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "settings.h"

/**
 * EEPROM memory Locations. Order is significant, unless you erase the EEPROM
 * during the deploy process.
 */

// Lamp
EEMEM uint8_t ee_lamp_start  =  8;  // 24h
EEMEM uint8_t ee_lamp_period = 10;  // Hours

// Pump
EEMEM uint8_t ee_pump_min   = 20;  // %
EEMEM uint8_t ee_pump_delay =  4;  // Hours
EEMEM uint8_t ee_pump_10ml  =  5;  // 5 (50 mL)

// Log
EEMEM uint8_t ee_log_period = (uint8_t) LOG_1_HOUR;  // LogPeriod


void restore_settings_from_eeprom(Settings* settings)
{
    // Lamp
    settings->lamp_start  = eeprom_read_byte((const void*) &ee_lamp_start);
    settings->lamp_period = eeprom_read_byte((const void*) &ee_lamp_period);

    // Pump
    settings->pump_min   = eeprom_read_byte((const void*) &ee_pump_min);
    settings->pump_delay = eeprom_read_byte((const void*) &ee_pump_delay);
    settings->pump_10ml  = eeprom_read_byte((const void*) &ee_pump_10ml);

    // Log
    settings->log_period  = eeprom_read_byte((const void*) &ee_log_period);
}


void update_settings_to_eeprom(const Settings* settings)
{
    // Lamp
    eeprom_update_byte(&ee_lamp_start, settings->lamp_start);
    eeprom_update_byte(&ee_lamp_period, settings->lamp_period);

    // Pump
    eeprom_update_byte(&ee_pump_min, settings->pump_min);
    eeprom_update_byte(&ee_pump_delay, settings->pump_delay);
    eeprom_update_byte(&ee_pump_10ml, settings->pump_10ml);

    // Log
    eeprom_update_byte(&ee_log_period, (uint8_t) settings->log_period);
}
