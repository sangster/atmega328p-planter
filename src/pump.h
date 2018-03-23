#ifndef PUMP_H
#define PUMP_H

#include <stdbool.h>
#include <time.h>
#include <sangster/pinout.h>
#include <sangster/timer0.h>
#include <sangster/usart_p.h>

#include "sensors.h"
#include "settings.h"

#define PUMP_UNPLUGGED_CHECK_DELAY  2000
#define YF_S402_FREQUENCY_FACTOR    73.0
#define PUMP_STATUS_PUMPING         -1
#define PUMP_STATUS_UNPLUGGED       -2


enum pump_state
{
    PUMP_DISABLED,
    PUMP_ENABLED
};
typedef enum pump_state PumpState;


volatile uint8_t pump_count;
double pump_total_ul;
uint16_t pump_started_at;
extern struct tm pump_last_time;


void pump_start(const Pinout);

/**
 * @return The total mL pumped. `-1` if the pump is still going or `-2` if it
 *   is unplugged
 */
int16_t pump_check(Pinout, const Settings*, BuoyLevel);


__attribute__((always_inline))
inline void pump_flow_callback()
{
    ++pump_count;
}


__attribute__((always_inline))
inline void pump_reset()
{
    pump_count = 0;
    pump_total_ul = 0;
}

#endif//PUMP_H
