#ifndef MENU_H
#define MENU_H

#include <stdio.h>
#include <time.h>
#include <avr/pgmspace.h>
#include <sangster/lcd.h>
#include <sangster/rtc_1307.h>
#include <sangster/usart_p.h>

#include "keys.h"
#include "log.h"
#include "settings.h"

enum mode
{
    SHOW_STATUS,
    SELECT_MENU,
    EDIT_MENU,
};
typedef enum mode Mode;
#define MODE_FIRST  SHOW_STATUS
#define MODE_LAST   EDIT_MENU


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
    SET_PUMP_MIN,   // The minimum moisture to trigger the pump
    SET_PUMP_DELAY, // The minimum amount of time between two waterings
    SET_PUMP_10ML,  // The amount of water to pump, in multiples of 10 mL
#define SET_PUMP_LAST   SET_PUMP_10ML

#define SET_LOG_FIRST   SET_LOG_PERIOD
    SET_LOG_PERIOD,
#define SET_LOG_LAST    SET_LOG_PERIOD
};
typedef enum submenu_mode SubmenuMode;

typedef void (*MenuFunction)(MenuMode menu);
typedef void (*MenuDateCallback)(struct tm);
typedef void (*MenuSettingsCallback)(Settings);


extern PROGMEM const char MENU_KEYS[];
extern Mode current_mode;
extern MenuMode current_menu_mode;
extern SubmenuMode current_submenu_mode;

struct tm edit_time;
Settings edit_settings;


void handle_menu_select(Lcd*, uint8_t key, MenuFunction);
void handle_menu_edit(Lcd*, uint8_t key, MenuDateCallback, MenuSettingsCallback);


#endif//MENU_H
