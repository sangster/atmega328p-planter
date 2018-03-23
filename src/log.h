#ifndef LOG_H
#define LOG_H

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <avr/pgmspace.h>
#include <sangster/ring_buff_8.h>
#include <sangster/sd.h>

#include "log_period.h"
#include "sensors.h"
#include "settings.h"


extern PROGMEM const char STATUS_LOG_PATTERN[];
extern PROGMEM const char STATUS_HEAD[];
extern PROGMEM const char PUMP_LOG[];
extern PROGMEM const char PUMP_HEAD[];
extern PROGMEM const char LAMP_LOG[];
extern PROGMEM const char LAMP_HEAD[];

extern PROGMEM const char LOG_PERIOD_5_MINS[];
extern PROGMEM const char LOG_PERIOD_10_MINS[];
extern PROGMEM const char LOG_PERIOD_15_MINS[];
extern PROGMEM const char LOG_PERIOD_30_MINS[];
extern PROGMEM const char LOG_PERIOD_1_HOUR[];
extern PROGMEM const char LOG_PERIOD_2_HOURS[];
extern PROGMEM const char LOG_PERIOD_3_HOURS[];
extern PROGMEM const char LOG_PERIOD_4_HOURS[];
extern PROGMEM const char LOG_PERIOD_6_HOURS[];
extern PROGMEM const char LOG_PERIOD_8_HOURS[];
extern PROGMEM const char LOG_PERIOD_12_HOURS[];
extern PROGMEM const char LOG_PERIOD_24_HOURS[];
extern PROGMEM const char LOG_PERIOD_48_HOURS[];
extern PROGMEM const char LOG_PERIOD_UNKNOWN[];


extern struct tm log_last_time;

void log_status(SdFile* dir, struct tm current_time, const Settings* settings,
                uint8_t moisture, uint8_t humidity, uint8_t temperature,
                BuoyLevel buoy_state, bool lamp_is_on,
                struct tm pump_last_time);

void log_pump(SdFile* dir, struct tm current_time, uint16_t total_ml,
              uint8_t moisture);

void log_lamp(SdFile* dir, struct tm current_time, bool is_on);

uint16_t log_period_seconds(LogPeriod);

struct tm log_next_time(struct tm, LogPeriod);

const char* log_period_label_P(LogPeriod);

#endif//LOG_H
