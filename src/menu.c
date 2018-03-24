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
#include "menu.h"

Mode current_mode = SHOW_STATUS;
MenuMode current_menu_mode = MENU_NONE;
SubmenuMode current_submenu_mode = SUBMENU_NONE;

PROGMEM static const char MENU_KEYS[] = MENU_LABEL;
static struct tm edit_time;
static Settings edit_settings;

/**
 * Responds to user keypresses when they are editing something in an edit menu
 */
static void handle_menu_edit_keypress(Lcd*, uint8_t key, MenuDateCallback,
                                      MenuSettingsCallback);

/**
 * Incredment/decrement the given number, cycling between min and max
 *
 * @param [in] num       The number to increment/decrement
 * @param [in] positive  `true` if the number should be incremented; `false` if
 *   it should be decremented
 * @param [in] min       The minimum value. If the user decrements below it,
 *   the number will loop aroundto the `max` value
 * @param [in] max       The maximum value. If the user increments above it,
 *   the number will loop aroundto the `min` value
 */
static uint8_t inc_num(uint8_t num, bool positive, uint8_t min, uint8_t max);


void handle_menu_select(Lcd* lcd, uint8_t key, MenuFunction callback)
{
    switch (current_menu_mode) {
        case START_PUMP:  lcd_buffer_update_P(lcd, 0, PSTR("Water Plant!")); break;
        case TOGGLE_LAMP: lcd_buffer_update_P(lcd, 0, PSTR("Toggle Lamp")); break;

        case SET_DATE: lcd_buffer_update_P(lcd, 0, PSTR("Date Config")); break;
        case SET_TIME: lcd_buffer_update_P(lcd, 0, PSTR("Time Config")); break;
        case SET_LAMP: lcd_buffer_update_P(lcd, 0, PSTR("Lamp Config")); break;
        case SET_PUMP: lcd_buffer_update_P(lcd, 0, PSTR("Pump Config")); break;
        case SET_LOG:  lcd_buffer_update_P(lcd, 0, PSTR("Log Config")); break;
        case MENU_NONE: break;
    }
    lcd_buffer_update_P(lcd, 1, MENU_KEYS);

    switch(key) {
        case KEY_DOWN:
            if (current_menu_mode == MENU_MODE_LAST) {
                current_menu_mode = MENU_MODE_FIRST;
            } else {
                ++current_menu_mode;
            }
            break;
        case KEY_UP:
            if (current_menu_mode == MENU_MODE_FIRST) {
                current_menu_mode = MENU_MODE_LAST;
            } else {
                --current_menu_mode;
            }
            break;
        case KEY_OK:
            switch(current_menu_mode) {
                case START_PUMP: __attribute__((fallthrough));
                case TOGGLE_LAMP:
                    callback(current_menu_mode);
                    current_mode = SHOW_STATUS;
                    break;

                default:
                    // Initialize menu choices to their current value
                    rtc_read(&edit_time);
                    restore_settings_from_eeprom(&edit_settings);

                    lcd_blink(lcd, true);
                    current_mode = EDIT_MENU;
                    break;
            }
            break;
        case KEY_BACK:
            current_mode = SHOW_STATUS;
            break;
    }
}


void handle_menu_edit(Lcd* lcd, uint8_t key, MenuDateCallback date_callback,
                      MenuSettingsCallback settings_callback)
{
    char str_in[25], str_date[13], period[6], *str_time;

    asctime_r(&edit_time, str_in);
    str_in[19] = '\0'; // split date and time
    strncpy(&str_date[0], &str_in[20], 4); // YYYY
    str_date[4] = ' ';
    strncpy(&str_date[5], &str_in[4], 7);  // MMM DD
    str_date[11] = '\0';

    str_time = &str_in[11];
    str_time[5] = '\0';

    switch(current_menu_mode) {
        case SET_DATE:
            lcd_buffer_update(lcd, 0, str_date);
            if (current_submenu_mode == SUBMENU_NONE) {
                current_submenu_mode = SET_DATE_FIRST;
            }
            break;
        case SET_TIME:
            lcd_buffer_update(lcd, 0, str_time);
            if (current_submenu_mode == SUBMENU_NONE) {
                current_submenu_mode = SET_TIME_FIRST;
            }
            break;
        case SET_LAMP:
            if (current_submenu_mode == SUBMENU_NONE) {
                current_submenu_mode = SET_LAMP_FIRST;
            }
            break;
        case SET_PUMP:
            if (current_submenu_mode == SUBMENU_NONE) {
                current_submenu_mode = SET_PUMP_FIRST;
            }
            break;
        case SET_LOG:
            if (current_submenu_mode == SUBMENU_NONE) {
                current_submenu_mode = SET_LOG_FIRST;
            }
            break;
        case MENU_NONE:
            break;
        case START_PUMP:  __attribute__((fallthrough));
        case TOGGLE_LAMP: __attribute__((fallthrough));
    }

    switch (current_submenu_mode) {
        // Date
        case SET_DATE_YEAR:
            lcd_move_cursor(lcd, 0, 3);
            break;
        case SET_DATE_MONTH:
            lcd_move_cursor(lcd, 0, 7);
            break;
        case SET_DATE_DAY:
            lcd_move_cursor(lcd, 0, 10);
            break;

        // Time
        case SET_TIME_HOUR:
            lcd_move_cursor(lcd, 0, 1);
            break;
        case SET_TIME_MINUTE:
            lcd_move_cursor(lcd, 0, 4);
            break;

        // Lamp
        case SET_LAMP_START:
            snprintf_P(str_in, 16, PSTR("Start: %02d:00"),
                       edit_settings.lamp_start);
            lcd_buffer_update(lcd, 0, str_in);
            lcd_move_cursor(lcd, 0, 8);
            break;
        case SET_LAMP_PERIOD:
            snprintf_P(str_in, 16, PSTR("Period: %2d hrs"),
                       edit_settings.lamp_period);
            lcd_buffer_update(lcd, 0, str_in);
            lcd_move_cursor(lcd, 0, 9);
            break;

        // Pump
        case SET_PUMP_MIN:
            snprintf_P(str_in, 16, PSTR("Min: M%02d%%"),
                       edit_settings.pump_min);
            lcd_buffer_update(lcd, 0, str_in);
            lcd_move_cursor(lcd, 0, 7);
            break;
        case SET_PUMP_DELAY:
            snprintf_P(str_in, 16, PSTR("Delay: %2d hrs"),
                       edit_settings.pump_delay);
            lcd_buffer_update(lcd, 0, str_in);
            lcd_move_cursor(lcd, 0, 8);
            break;
        case SET_PUMP_10ML:
            snprintf_P(str_in, 16, PSTR("Amount: %02d0 mL"),
                       edit_settings.pump_10ml);
            lcd_buffer_update(lcd, 0, str_in);
            lcd_move_cursor(lcd, 0, 9);
            break;

        // Log
        case SET_LOG_PERIOD:
            strncpy_P(period, log_period_label_P(edit_settings.log_period), 6);
            snprintf_P(str_in, 16, PSTR("Period: %s"), period);
            lcd_buffer_update(lcd, 0, str_in);
            lcd_move_cursor(lcd, 0, 12);

        case SUBMENU_NONE:
            break;
    }
    lcd_buffer_update_P(lcd, 1, MENU_KEYS);

    handle_menu_edit_keypress(lcd, key, date_callback, settings_callback);
}


void handle_menu_edit_keypress(Lcd* lcd, uint8_t key,
                               MenuDateCallback date_callback,
                               MenuSettingsCallback settings_callback)
{
    const bool dir = key == KEY_UP;
    const uint8_t month_len = month_length((uint16_t) 1900 + edit_time.tm_year,
                                           edit_time.tm_mon + 1);

    switch(key) {
        case KEY_DOWN: __attribute__((fallthrough));
        case KEY_UP:
            switch (current_submenu_mode) {
                // Date
                case SET_DATE_YEAR:
                    edit_time.tm_year = inc_num(edit_time.tm_year, dir, 0, 199);
                    break;
                case SET_DATE_MONTH:
                    edit_time.tm_mon = inc_num(edit_time.tm_mon, dir, 0, 11);
                    break;
                case SET_DATE_DAY:
                    edit_time.tm_mday = inc_num(edit_time.tm_mday, dir, 1, month_len);
                    break;

                // Time
                case SET_TIME_HOUR:
                    edit_time.tm_hour = inc_num(edit_time.tm_hour, dir, 0, 23);
                    break;
                case SET_TIME_MINUTE:
                    edit_time.tm_min = inc_num(edit_time.tm_min, dir, 0, 59);
                    break;

                // Lamp
                case SET_LAMP_START:
                    edit_settings.lamp_start =
                        inc_num(edit_settings.lamp_start, dir, 0, 23);
                    break;
                case SET_LAMP_PERIOD:
                    edit_settings.lamp_period =
                        inc_num(edit_settings.lamp_period, dir, 0, 20);
                    break;

                // Pump
                case SET_PUMP_MIN:
                    edit_settings.pump_min =
                        // 20-80 are DHT11 accuracy limits
                        inc_num(edit_settings.pump_min, dir, 20, 80);
                    break;
                case SET_PUMP_DELAY:
                    edit_settings.pump_delay =
                        inc_num(edit_settings.pump_delay, dir, 1, 48);
                    break;
                case SET_PUMP_10ML:
                    edit_settings.pump_10ml =
                        inc_num(edit_settings.pump_10ml, dir, 1, 25);
                    break;

                // Log
                case SET_LOG_PERIOD:
                    edit_settings.log_period =
                        inc_num(edit_settings.log_period, dir,
                                LOG_PERIOD_FIRST, LOG_PERIOD_LAST);
                    break;

                case SUBMENU_NONE:
                    break;
            }
            break;

        // Go to the previous submenu, or move to back up a level if already on
        // the first submenu
        case KEY_BACK:
            switch (current_submenu_mode) {
                case SET_DATE_FIRST: __attribute__((fallthrough));
                case SET_TIME_FIRST: __attribute__((fallthrough));
                case SET_LAMP_FIRST: __attribute__((fallthrough));
                case SET_PUMP_FIRST: __attribute__((fallthrough));
                case SET_LOG_FIRST:
                    lcd_blink(lcd, false);
                    current_mode = SELECT_MENU;
                    current_submenu_mode = SUBMENU_NONE;
                    break;
                default:
                    --current_submenu_mode;
                    break;
            }
            break;

        // Go to the next submnu, or if on the last submenu, commit the users
        // choices then move back up a level
        case KEY_OK:
            switch (current_submenu_mode) {
                // Commit time
                case SET_DATE_LAST: __attribute__((fallthrough));
                case SET_TIME_LAST:
                    edit_time.tm_sec = 0;
                    mktime(&edit_time);
                    date_callback(edit_time);

                    lcd_blink(lcd, false);
                    current_mode = SHOW_STATUS;
                    current_submenu_mode = SUBMENU_NONE;
                    break;

                // Commit settings to EEPROM
                case SET_LAMP_LAST: __attribute__((fallthrough));
                case SET_PUMP_LAST: __attribute__((fallthrough));
                case SET_LOG_LAST:
                    settings_callback(edit_settings);

                    lcd_blink(lcd, false);
                    current_mode = SHOW_STATUS;
                    current_submenu_mode = SUBMENU_NONE;
                    break;

                default:
                    ++current_submenu_mode;
                    break;
            }
            break;
    }
}


uint8_t inc_num(uint8_t num, bool positive, uint8_t min, uint8_t max)
{
    if (positive) {
        return num == max ? min : num + 1;
    }
    return num == min ? max : num - 1;
}
