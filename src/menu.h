#ifndef MENU_H
#define MENU_H
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

#include <stdio.h>
#include <time.h>
#include <avr/pgmspace.h>
#include <sangster/lcd.h>
#include <sangster/rtc_1307.h>
#include <sangster/usart_p.h>

#include "keys.h"
#include "log.h"
#include "settings.h"

// 2 spaces + symbol will ensure the symbols align with the physical buttons
#define MENU_LABEL  "  \xc5   \xc6   \xc4   \xf7"

/** The UI's modes */
enum mode
{
#define MODE_FIRST  SHOW_STATUS
    SHOW_STATUS,  ///< Show the plant's current statistics. The default mode
    SELECT_MENU,  ///< Choose an "edit menu"
    EDIT_MENU,    ///< Edit a particular system setting
#define MODE_LAST   EDIT_MENU
};
typedef enum mode Mode;


enum menu_mode
{
#define MENU_MODE_FIRST  START_PUMP
    MENU_NONE,
    START_PUMP,
    TOGGLE_LAMP,
    SET_DATE,
    SET_TIME,
    SET_LAMP,
    SET_PUMP,
    SET_LOG,
#define MENU_MODE_LAST   SET_LOG
};
typedef enum menu_mode MenuMode;

enum submenu_mode
{
    SUBMENU_NONE,

#define SET_DATE_FIRST  SET_DATE_YEAR
    SET_DATE_YEAR,
    SET_DATE_MONTH,
    SET_DATE_DAY,
#define SET_DATE_LAST   SET_DATE_DAY

#define SET_TIME_FIRST  SET_TIME_HOUR
    SET_TIME_HOUR,
    SET_TIME_MINUTE,
#define SET_TIME_LAST   SET_TIME_MINUTE

#define SET_LAMP_FIRST  SET_LAMP_START
    SET_LAMP_START,
    SET_LAMP_PERIOD,
#define SET_LAMP_LAST   SET_LAMP_PERIOD

#define SET_PUMP_FIRST  SET_PUMP_MIN
    SET_PUMP_MIN,   ///< The minimum moisture to trigger the pump
    SET_PUMP_DELAY, ///< The minimum amount of time between two waterings
    SET_PUMP_10ML,  ///< The amount of water to pump, in multiples of 10 mL
#define SET_PUMP_LAST   SET_PUMP_10ML

#define SET_LOG_FIRST   SET_LOG_PERIOD
    SET_LOG_PERIOD,
#define SET_LOG_LAST    SET_LOG_PERIOD
};
typedef enum submenu_mode SubmenuMode;

/** A callback called when the user chooses an action from the menu. */
typedef void (*MenuFunction)(MenuMode);

/** A callback called when the user changes the system date/time */
typedef void (*MenuDateCallback)(struct tm);

/** A callback called when the user changes the system settings */
typedef void (*MenuSettingsCallback)(Settings);


extern Mode current_mode;
extern MenuMode current_menu_mode;
extern SubmenuMode current_submenu_mode;


/** Respond to the user selecting an "edit menu" */
void handle_menu_select(Lcd*, uint8_t key, MenuFunction);

/** Respond to the user using an "edit menu" */
void handle_menu_edit(Lcd*, uint8_t key, MenuDateCallback, MenuSettingsCallback);


#endif//MENU_H
