#ifndef LOG_PERIOD_H
#define LOG_PERIOD_H

enum log_period
{
#define LOG_PERIOD_FIRST  LOG_5_MINS
    LOG_5_MINS,
    LOG_10_MINS,
    LOG_15_MINS,
    LOG_30_MINS,
    LOG_1_HOUR,
    LOG_2_HOURS,
    LOG_3_HOURS,
    LOG_4_HOURS,
    LOG_6_HOURS,
    LOG_8_HOURS,
    LOG_12_HOURS,
    LOG_24_HOURS,
    LOG_48_HOURS,
#define LOG_PERIOD_LAST  LOG_48_HOURS
};
typedef enum log_period LogPeriod;

#endif//LOG_PERIOD_H
