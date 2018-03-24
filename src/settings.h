#ifndef SETTINGS_H
#define SETTINGS_H
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

#include <avr/eeprom.h>
#include "log_period.h"


/** The system's runtime configuration */
struct settings
{
    /**
     * @defgrounp ee_lamp Lamp Settings
     */
    /// @{
    /// The hour of the day when the lamp first turns on (24h)
    uint8_t lamp_start;   // 24h

    /// The duration the lamp remains on (hours)
    uint8_t lamp_period;  // Hours
    /// @}

    /**
     * @defgrounp ee_pump Pump Settings
     */
    /// @{
    /// The moisture trigger (%). Pumping won't begin if the moisture is greater
    uint8_t pump_min;    // %

    /// The minimum number of hours between two waterings
    uint8_t pump_delay;  // Hours

    /// The amount of water to pump each time (10's of mL. ex: 5 == 50 mL)
    uint8_t pump_10ml;   // 10 mL
    /// @}

    /**
     * @defgrounp ee_log Log Settings
     */
    /// @{
    LogPeriod log_period;
    /// @}
};
typedef struct settings Settings;


/** Populate the given Settings from the microcontroller's EEPROM */
void restore_settings_from_eeprom(Settings*);

/** Store the given Settings in the microcontroller's EEPROM */
void update_settings_to_eeprom(const Settings*);

#endif//SETTINGS_H
