#ifndef SETTINGS_H
#define SETTINGS_H

#include <avr/eeprom.h>
#include "log_period.h"


/**
 * EEPROM memory Locations. Order is significant, unless you erase the EEPROM
 * during the deploy process.
 */
// Lamp
extern EEMEM uint8_t ee_lamp_start;   // 24h
extern EEMEM uint8_t ee_lamp_period;  // Hours

// Pump
extern EEMEM uint8_t ee_pump_min;    // %
extern EEMEM uint8_t ee_pump_delay;  // Hours
extern EEMEM uint8_t ee_pump_10ml;   // 10 mL

// Log
extern EEMEM uint8_t ee_log_period;   // LogPeriod


struct settings
{
    // Lamp
    uint8_t lamp_start;   // 24h
    uint8_t lamp_period;  // Hours

    // Pump
    uint8_t pump_min;    // %
    uint8_t pump_delay;  // Hours
    uint8_t pump_10ml;   // 10 mL

    // Log
    LogPeriod log_period;
};
typedef struct settings Settings;


void restore_settings_from_eeprom(Settings*);
void update_settings_to_eeprom(Settings*);

#endif//SETTINGS_H
