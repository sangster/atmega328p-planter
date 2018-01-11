#include "pump.h"

struct tm pump_last_time = {0};
bool flow_sensor_plugged_in = false;

static uint16_t last_check;

int16_t pump_check(const Pinout pin, const Settings* settings,
                   const BuoyLevel buoy_state)
{
    // Cut off pump if it's flowing past our limit
    uint32_t limit_ul = settings->pump_10ml * 10000UL;
    uint16_t now = timer0_ms();
    if (buoy_state != BUOY_UP || pump_total_ul > limit_ul) {
        pinout_clr(pin);
        int16_t finished_total_ml = pump_total_ul / 1000;
        pump_reset();

        usart_32(finished_total_ml);
        usart_println_P(PSTR(" mL pumped"));
        return finished_total_ml;
    } else if (!flow_sensor_plugged_in
                   && now > pump_started_at + PUMP_UNPLUGGED_CHECK_DELAY) {
        pinout_clr(pin);
        pump_reset();
        return PUMP_STATUS_UNPLUGGED;
    } else {
        const uint16_t elapsed_ms = now - last_check;
        const float count_hz = pump_count * (1000.0 / (elapsed_ms));
        const float ml_per_min = count_hz * 1000 / YF_S402_FREQUENCY_FACTOR;
        const float ul_per_ms = ml_per_min / 60;

        pump_total_ul += elapsed_ms * ul_per_ms;

        /* usart_print_P(PSTR("count: ")); */
        /* usart_16(pump_count); */
        /* usart_print_P(PSTR(",  ms: ")); */
        /* usart_32(now - last_check); */
        /* usart_print_P(PSTR(", ")); */
        /* usart_32(pump_total_ul); */
        /* usart_println_P(PSTR(" uL")); */

        if (pump_count) {
            flow_sensor_plugged_in = true;
        }
        pump_count = 0; // start new count
        last_check = now;
        return PUMP_STATUS_PUMPING;
    }
}


void pump_start(const Pinout pin)
{
    pump_reset();
    pinout_set(pin);
    pump_started_at = timer0_ms();
    last_check = pump_started_at;
}
