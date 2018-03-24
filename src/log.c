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
#include <log.h>

PROGMEM static const char STATUS_LOG_PATTERN[] = LOG_STATUS_FILENAME_PATTERN;
PROGMEM static const char STATUS_HEAD[]        = LOG_STATUS_HEADER;
PROGMEM static const char PUMP_LOG[]           = LOG_PUMP_FILENAME;
PROGMEM static const char PUMP_HEAD[]          = LOG_PUMP_HEADER;
PROGMEM static const char LAMP_LOG[]           = LOG_LAMP_FILENAME;
PROGMEM static const char LAMP_HEAD[]          = LOG_LAMP_HEADER;

PROGMEM static const char LOG_PERIOD_5_MINS[]   = "00:05";
PROGMEM static const char LOG_PERIOD_10_MINS[]  = "00:10";
PROGMEM static const char LOG_PERIOD_15_MINS[]  = "00:15";
PROGMEM static const char LOG_PERIOD_30_MINS[]  = "00:30";
PROGMEM static const char LOG_PERIOD_1_HOUR[]   = "01:00";
PROGMEM static const char LOG_PERIOD_2_HOURS[]  = "02:00";
PROGMEM static const char LOG_PERIOD_3_HOURS[]  = "03:00";
PROGMEM static const char LOG_PERIOD_4_HOURS[]  = "04:00";
PROGMEM static const char LOG_PERIOD_6_HOURS[]  = "06:00";
PROGMEM static const char LOG_PERIOD_8_HOURS[]  = "08:00";
PROGMEM static const char LOG_PERIOD_12_HOURS[] = "12:00";
PROGMEM static const char LOG_PERIOD_24_HOURS[] = "24:00";
PROGMEM static const char LOG_PERIOD_48_HOURS[] = "48:00";
PROGMEM static const char LOG_PERIOD_UNKNOWN[]  = "??:??";


struct tm log_last_time = {0};


/**
 * @return a handle to the status log file. If it doesn't exist, it will be
 *   created with the correct file header
 */
static SdFile open_status_file(SdFile* dir, struct tm current_time);

/**
 * @return a handle to the pump log file. If it doesn't exist, it will be
 *   created with the correct file header
 */
static SdFile open_pump_file(SdFile* dir);

/**
 * @return a handle to the lamp log file. If it doesn't exist, it will be
 *   created with the correct file header
 */
static SdFile open_lamp_file(SdFile* dir);


void log_status(SdFile* dir, const struct tm current_time,
                const Settings* settings, const uint8_t moisture,
                const uint8_t humidity, const uint8_t temperature,
                const BuoyLevel buoy_state, const bool lamp_is_on,
                const struct tm pump_last_time)
{
    SdFile file = open_status_file(dir, current_time);
    char str[20];

    isotime_r(&current_time, str);
    sd_file_print(&file, str);
    sd_file_send(&file, ',');

    itoa(moisture, str, 10);
    sd_file_print(&file, str);
    sd_file_send(&file, ',');

    itoa(humidity, str, 10);
    sd_file_print(&file, str);
    sd_file_send(&file, ',');

    itoa(temperature, str, 10);
    sd_file_print(&file, str);
    sd_file_send(&file, ',');

    if (pump_last_time.tm_year != 0) {
        isotime_r(&pump_last_time, str);
        sd_file_print(&file, str);
    }
    sd_file_send(&file, ',');

    snprintf_P(str, 6, PSTR("%02d:00"), settings->lamp_start);
    sd_file_print(&file, str);
    sd_file_send(&file, ',');

    itoa(settings->lamp_period, str, 10);
    sd_file_print(&file, str);
    sd_file_send(&file, ',');

    itoa(settings->pump_min, str, 10);
    sd_file_print(&file, str);
    sd_file_send(&file, ',');

    itoa(settings->pump_delay, str, 10);
    sd_file_print(&file, str);
    sd_file_send(&file, ',');

    utoa(settings->pump_10ml * 10U, str, 10);
    sd_file_print(&file, str);
    sd_file_send(&file, ',');

    utoa(log_period_seconds(settings->log_period) / 60, str, 10);
    sd_file_print(&file, str);
    sd_file_send(&file, ',');

    if (lamp_is_on) {
        sd_file_write_P(&file, PSTR("On"));
    } else {
        sd_file_write_P(&file, PSTR("Off"));
    }
    sd_file_send(&file, ',');

    switch (buoy_state) {
        case BUOY_UP:      sd_file_write_P(&file, PSTR("Up")); break;
        case BUOY_DOWN:    sd_file_write_P(&file, PSTR("Down")); break;
        case BUOY_UNKNOWN: sd_file_write_P(&file, PSTR("???")); break;
    }

    sd_file_crlf(&file);
    sd_file_sync(&file);
}


static SdFile open_status_file(SdFile* dir, struct tm current_time)
{
    SdFile file;
    sd_file_init(&file);

    char path[12];
    snprintf_P(path, 12, STATUS_LOG_PATTERN, 1900 + current_time.tm_year,
               current_time.tm_mon);

    if (sd_exists_in_dir(dir, path)) {
        sd_file_open(&file, dir, path, O_WRITE | O_APPEND);
    } else {
        sd_file_open(&file, dir, path, O_WRITE | O_CREAT);
        sd_file_writeln_P(&file, STATUS_HEAD);
    }
    return file;
}


void log_pump(SdFile* dir, const struct tm current_time, const uint16_t total_ml,
              const uint8_t moisture)
{
    SdFile file = open_pump_file(dir);

    char str[20];

    isotime_r(&current_time, str);
    sd_file_print(&file, str);
    sd_file_send(&file, ',');

    itoa(moisture, str, 10);
    sd_file_print(&file, str);
    sd_file_send(&file, ',');

    utoa(total_ml, str, 10);
    sd_file_print(&file, str);
    sd_file_crlf(&file);

    sd_file_sync(&file);
}


SdFile open_pump_file(SdFile* dir)
{
    SdFile file;
    sd_file_init(&file);

    char path[12];
    strncpy_P(path, PUMP_LOG, 11);

    if (sd_exists_in_dir(dir, path)) {
        sd_file_open(&file, dir, path, O_WRITE | O_APPEND);
    } else {
        sd_file_open(&file, dir, path, O_WRITE | O_CREAT);
        sd_file_writeln_P(&file, PUMP_HEAD);
    }
    return file;
}


void log_lamp(SdFile* dir, const struct tm current_time, const bool is_on)
{
    SdFile file = open_lamp_file(dir);
    char str[20];

    isotime_r(&current_time, str);
    sd_file_print(&file, str);
    sd_file_send(&file, ',');

    if (is_on) {
        sd_file_write_P(&file, PSTR("On"));
    } else {
        sd_file_write_P(&file, PSTR("Off"));
    }
    sd_file_crlf(&file);

    sd_file_sync(&file);
}


SdFile open_lamp_file(SdFile* dir)
{
    SdFile file;
    sd_file_init(&file);

    char path[12];
    strncpy_P(path, LAMP_LOG, 11);

    if (sd_exists_in_dir(dir, path)) {
        sd_file_open(&file, dir, path, O_WRITE | O_APPEND);
    } else {
        sd_file_open(&file, dir, path, O_WRITE | O_CREAT);
        sd_file_writeln_P(&file, LAMP_HEAD);
    }
    return file;
}


uint16_t log_period_seconds(const LogPeriod period)
{
    switch (period) {
        case LOG_5_MINS:   return  5 *  60;
        case LOG_10_MINS:  return 10 *  60;
        case LOG_15_MINS:  return 15 *  60;
        case LOG_30_MINS:  return 30 *  60;

        case LOG_1_HOUR:   return  1 * 360;
        case LOG_2_HOURS:  return  2 * 360;
        case LOG_3_HOURS:  return  3 * 360;
        case LOG_4_HOURS:  return  4 * 360;
        case LOG_6_HOURS:  return  6 * 360;
        case LOG_8_HOURS:  return  8 * 360;
        case LOG_12_HOURS: return 12 * 360;
        case LOG_24_HOURS: return 24 * 360;
        case LOG_48_HOURS: return 48 * 360;

        default: return log_period_seconds(LOG_1_HOUR);
    }
}


struct tm log_next_time(struct tm time, LogPeriod period)
{
    struct tm next;
    time_t next_time = mk_gmtime(&time) + log_period_seconds(period);
    gmtime_r(&next_time, &next);
    return next;
}


PGM_P log_period_label_P(const LogPeriod period)
{
    switch (period) {
        case LOG_5_MINS:   return LOG_PERIOD_5_MINS;
        case LOG_10_MINS:  return LOG_PERIOD_10_MINS;
        case LOG_15_MINS:  return LOG_PERIOD_15_MINS;
        case LOG_30_MINS:  return LOG_PERIOD_30_MINS;

        case LOG_1_HOUR:   return LOG_PERIOD_1_HOUR;
        case LOG_2_HOURS:  return LOG_PERIOD_2_HOURS;
        case LOG_3_HOURS:  return LOG_PERIOD_3_HOURS;
        case LOG_4_HOURS:  return LOG_PERIOD_4_HOURS;
        case LOG_6_HOURS:  return LOG_PERIOD_6_HOURS;
        case LOG_8_HOURS:  return LOG_PERIOD_8_HOURS;
        case LOG_12_HOURS: return LOG_PERIOD_12_HOURS;
        case LOG_24_HOURS: return LOG_PERIOD_24_HOURS;
        case LOG_48_HOURS: return LOG_PERIOD_48_HOURS;

        default: return LOG_PERIOD_UNKNOWN;
    }
}
