/**
 * @file
 */
#include <stdbool.h>
#include <time.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <sangster/lcd.h>
#include <sangster/pinout.h>
#include <sangster/ring_buff_8.h>
#include <sangster/rtc_1307.h>
#include <sangster/sd.h>
#include <sangster/timer0.h>
#include <sangster/twi.h>
#include <sangster/usart.h>
#include <sangster/usart_p.h>

#include "buzzer.h"
#include "log.h"
#include "menu.h"
#include "pump.h"
#include "rtc.h"
#include "sensors.h"
#include "settings.h"


/*******************************************************************************
 * Constants
 ******************************************************************************/
#define SCREEN_TIMEOUT  60 // seconds
#define STARTUP_PUMP_CHECK_TIME  10 * 60 // seconds
/* #define STARTUP_LOG_TIME         5 * 60 // seconds */
#define STARTUP_LOG_TIME         5 // seconds
#define SD_TIMEZONE_OFFSET       -8 * 3600 // seconds

// 1 second system check timer (62500 * 256) / 16e6 == 1 second
#define CHECK_SYSTEM_MS  250
#define CHECK_MHUM_MS   1000
#define SENSOR_CLKS  (62500 / (1000 / CHECK_SYSTEM_MS))
uint8_t check_system_counter = 0;

PROGMEM const char APP_TITLE[] = "Planter     v1.0"; // 16 chars for the LCD
PROGMEM const char SD_DIR[] = "Plant1_0";


/*******************************************************************************
 * Types
 ******************************************************************************/
enum pump_state
{
    DISABLED,
    ENABLED
};
typedef enum pump_state PumpState;


/*******************************************************************************
 * Functions
 ******************************************************************************/
__attribute__((always_inline)) inline void setup();
__attribute__((always_inline)) inline void loop();
__attribute__((always_inline)) inline void handle_ui();
__attribute__((always_inline)) inline void show_status_on_lcd();
void setup_buttons();
void setup_pump();
void setup_sd();
void setup_rtc();
void setup_buzzer();
void setup_lcd();
void shift_lcd_write4(const Lcd*, const uint8_t nibble);
void recheck_system();
void check_lamp();
void check_pump();
void start_pump();
void toggle_lamp();
void menu_function_dispatch(MenuMode);
void set_lcd_enabled(bool);
void set_lamp_enabled(bool);
void on_water_level_change(BuoyLevel);
void fatal_error_P(const char*, const char*);
void menu_edit_date_callback(struct tm);
void menu_edit_settings_callback(Settings);
void sd_set_file_date_time(uint16_t* date, uint16_t* time);
void check_log();


/*******************************************************************************
 * Pins
 ******************************************************************************/
// See Table 28-4 in the ATMega328p manual
#define PIN_BUTTONS_MUX  _BV(MUX1) // ADC2
#define PIN_MOIST_MUX            0 // ADC0

// Input
const Pinout pin_buoy    = {&PORTC, PORTC3};
const Pinout pin_humid   = {&PORTC, PORTC1};
const Pinout pin_moist   = {&PORTC, PORTC0};
const Pinout pin_flow    = {&PORTD, PORTD2}; // only used for INT0 interupt!
const Pinout pin_buttons = {&PORTC, PORTC2};

// Output
const Pinout pin_pump = {&PORTB, PORTB1};
const Pinout pin_lamp = {&PORTD, PORTD0};
const Pinout pin_buzz = {&PORTD, PORTD7};
const Pinout pin_lcd_data = {&PORTD, PORTD3};
const Pinout pin_lcd_clk  = {&PORTB, PORTB0};
const Pinout pin_lcd_backlight = {&PORTD, PORTD6};


// Breakout boards (pins + state)
Lcd lcd = {
    .rs = {&PORTD, PORTD5},
    .en = {&PORTD, PORTD4},
};

bool sd_card_available = false;
SdFile log_dir;
SdClass SD = {
    .card = {
        .chip_select_pin = {&PORTB, PORTB2},
        .mosi_pin        = {&PORTB, PORTB3},
        .miso_pin        = {&PORTB, PORTB4},
        .clock_pin       = {&PORTB, PORTB5},
    }
};
Rtc1307 rtc = {
    .twi = {
        .pin_sda = {&PORTC, PORTC4},
        .pin_scl = {&PORTC, PORTC5},
    }
};


/*******************************************************************************
 * State
 ******************************************************************************/
bool fatal_error_active = false;
char fatal_error_lcd0[16];
char fatal_error_lcd1[16];

Settings settings;
struct tm current_time;
volatile bool recheck_scheduled = false;

PumpState pump_state = DISABLED;
uint8_t moisture_before_pumping = 0;

bool lamp_override = false;
bool lamp_override_on = false;

char lcd_buffer[LCD_ROWS][LCD_COLS];
struct tm lcd_timeout_at;
bool lcd_is_on = false;

RING_BUFF_8_DEF(moisture, 4);
RING_BUFF_8_DEF(humidity, 4);
RING_BUFF_8_DEF(temperature, 4);

/*******************************************************************************
 * Interrupts
 ******************************************************************************/
ISR(INT0_vect)
{
    pump_flow_callback();
}

ISR(TIMER0_OVF_vect)
{
    timer0_interrupt_callback();
}

ISR(TIMER1_COMPA_vect)
{
    recheck_scheduled = true;
}

ISR(TIMER2_COMPA_vect)
{
    pinout_toggle(pin_buzz);
}

ISR(TWI_vect)
{
    twi_handle_vect(); // for the RTC
}


/*******************************************************************************
 * Main Program
 ******************************************************************************/
__attribute__((OS_main))
int main(void)
{
    // Usart for console logging/debugging
    usart_init(9600, FORMAT_8N1);
    UCSR0B |= _BV(TXEN0);
    usart_println_P(APP_TITLE);

    setup();
    for(;;) {
        loop(); // TODO: is this really inline?
    }
}


void setup()
{
    pinout_make_input(pin_flow);
    pinout_make_pullup_input(pin_buoy);
    pinout_make_pullup_input(pin_humid);
    pinout_make_pullup_input(pin_buttons); // max value == no buttons clicked

    pinout_make_output(pin_pump);
    pinout_make_output(pin_lamp);
    pinout_make_output(lcd.rs);
    pinout_make_output(lcd.en);
    pinout_make_output(pin_lcd_data);
    pinout_make_output(pin_lcd_clk);
    pinout_make_output(pin_lcd_backlight);
    pinout_make_output(pin_buzz);

    restore_settings_from_eeprom(&settings);

    sei();

    // Setup devices
    setup_pump();
    setup_lcd();
    lcd_buffer_update_P(&lcd, 0, APP_TITLE);
    setup_rtc();
    setup_buzzer();
    setup_buttons();

    // Start system check timer
    OCR1A = SENSOR_CLKS;
    TCCR1B |= _BV(WGM12) | _BV(CS12); // CTC mode, 256 prescaler
    TIMSK1 |= _BV(OCIE1A);

    recheck_system();

    // Setup SD Card
    setup_sd();
    usart_println_P(sd_card_available ? PSTR("mounted") : PSTR("ejected"));

    // avoid pumping immediately
    time_t last_pump = mk_gmtime(&current_time) - settings.pump_delay * 3600
                           + STARTUP_PUMP_CHECK_TIME;
    gmtime_r(&last_pump, &pump_last_time);

    // avoid logging immediately
    time_t last_log = mk_gmtime(&current_time) - log_period_seconds(settings.log_period)
                           + STARTUP_LOG_TIME;
    gmtime_r(&last_log, &log_last_time);

    set_lcd_enabled(true);
}


void loop()
{
    if (fatal_error_active) {
        lcd_buffer_update(&lcd, 0, fatal_error_lcd0);
        lcd_buffer_update(&lcd, 1, fatal_error_lcd1);
    } else {
        if (recheck_scheduled) {
            recheck_scheduled = false;

            if (pinout_is_clr(pin_pump)) {
                recheck_system();
                check_lamp();

                if (sd_card_available) {
                    check_log();
                }
            }
            check_pump();
        }

        if (pinout_is_set(pin_pump)) {
            lcd_buffer_update_P(&lcd, 0, PSTR("    Pumping!"));
            lcd_buffer_update_P(&lcd, 1, PSTR("  Please Wait  "));
        } else {
            handle_ui();
        }
    }
    // vvv Regardless of Error state vvv

    if (buzzer_is_on) {
        buzzer_sound(pin_buzz);
    }
}


void recheck_system()
{
    // The DHT11 moisture/humidity sensor can only be checked every second, so
    // we only read it every N cycles.
    static uint8_t mhum_cycle = 0;

    rtc_read(&current_time);

    // turrn off LCD, if inactive
    const int32_t time_left = difftime(mk_gmtime(&lcd_timeout_at),
                                       mk_gmtime(&current_time));
    if (time_left <= 0) {
        set_lcd_enabled(false);
    }

    read_buoyancy(pin_buoy, &on_water_level_change);
    read_moisture(PIN_MOIST_MUX, &moisture);

    if (mhum_cycle == (CHECK_MHUM_MS / CHECK_SYSTEM_MS - 1) &&
        !read_humidity_temperature(pin_humid, &humidity, &temperature)) {
        usart_println_P(PSTR("Error reading MHUM-01"));
    }
    mhum_cycle = (mhum_cycle + 1) % (CHECK_MHUM_MS / CHECK_SYSTEM_MS);
}


void check_lamp()
{
    uint8_t curr  = current_time.tm_hour,
            start = settings.lamp_start,
            end   = (start + settings.lamp_period) % 24;

    if (lamp_override) {
        // The user forced the lamp on/off, but we only honor this until the
        // next start/stop time in the lamp's schedule.
        if (lamp_override_on && curr == end ) {
            set_lamp_enabled(false);
            lamp_override = false;
        } else if (!lamp_override_on && curr == start) {
            set_lamp_enabled(true);
            lamp_override = false;
        }
    } else {
        if (end > start) {
            // lamp scheduled to turn on then off in the same calendar day
            set_lamp_enabled(curr > start && curr < end);
        } else {
            // lamp scheduled to turn on one day then off after midnight
            set_lamp_enabled(curr > start || curr < end);
        }
    }
}


void check_pump()
{
    if (pinout_is_set(pin_pump)) {
        int16_t total_ml = pump_check(pin_pump, &settings, buoy_state);

        switch(total_ml) {
            case PUMP_STATUS_PUMPING:
                break;
            case PUMP_STATUS_UNPLUGGED:
                fatal_error_P(PSTR("  Flow Sensor"),
                              PSTR("  Unplugged!"));
                break;
            default:
                if (sd_card_available) {
                    log_pump(&log_dir, current_time, total_ml,
                             moisture_before_pumping);
                } else {
                    usart_println_P(PSTR("SD unavailable. Cannot log pump."));
                }
                break;
        }

    } else {
        uint16_t delay = settings.pump_delay * 3600;
        int32_t time_until_pump = difftime(mk_gmtime(&pump_last_time) + delay,
                                            mk_gmtime(&current_time));

        if (time_until_pump <= 0) {
            pump_last_time = current_time;

            if (buoy_state == BUOY_UP) {
                if (ring_buff_8_avg(&moisture) <= settings.pump_min) {
                    pump_start(pin_pump);
                }
            }
        }
    }
}


void start_pump()
{
    moisture_before_pumping = ring_buff_8_avg(&moisture);
    pump_start(pin_pump);
}


void toggle_lamp()
{
    set_lamp_enabled(pinout_is_clr(pin_lamp));
}


void handle_ui()
{
    const KeyCode key = keycode(PIN_BUTTONS_MUX);

    // Turn on LCD
    if (key != KEY_NONE) {
        bool lcd_was_off = !lcd_is_on;
        set_lcd_enabled(true);
        if (lcd_was_off) {
            return; // turning on the screen has consumed this keypress
        }
    }

    if (!lcd_is_on) {
        return; // User is AFK
    }

    switch (current_mode) {
        case SHOW_STATUS:
            if (key) {
                current_mode = SELECT_MENU;
                current_menu_mode = MENU_MODE_FIRST;
            }

            show_status_on_lcd();
            break;

        case SELECT_MENU:
            handle_menu_select(&lcd, key, &menu_function_dispatch);
            break;

        case EDIT_MENU:
            handle_menu_edit(&lcd, key, menu_edit_date_callback,
                             menu_edit_settings_callback);
            break;
    }
}


void menu_edit_date_callback(struct tm new)
{
    int32_t offset = difftime(mk_gmtime(&new), mk_gmtime(&current_time));
    time_t tmp_time;
    char str[25];
    asctime_r(&new, str);

    usart_print_P(PSTR("Updating time to: "));
    usart_println(str);

    rtc_set(&new);

    // Log time
    tmp_time = mk_gmtime(&log_last_time) + offset;
    gmtime_r(&tmp_time, &log_last_time);

    // Pump time
    tmp_time = mk_gmtime(&pump_last_time) + offset;
    gmtime_r(&tmp_time, &pump_last_time);

    // LCD time
    set_lcd_enabled(true);
}


void menu_edit_settings_callback(Settings new)
{
    const LogPeriod prev_period = settings.log_period;

    settings = new;
    update_settings_to_eeprom(&settings);

    if (settings.log_period != prev_period) {
        log_last_time = current_time;
    }
}


void menu_function_dispatch(MenuMode menu)
{
    switch (menu) {
        case START_PUMP:
            start_pump();
            break;
        case TOGGLE_LAMP:
            lamp_override = true;
            lamp_override_on = !pinout_is_set(pin_lamp);
            toggle_lamp();
            break;
        default:
            // noop
            break;
    }
}


void show_status_on_lcd()
{
    char line[16];

    sensors_to_s(line, ring_buff_8_avg(&moisture),
            ring_buff_8_avg(&humidity),
            ring_buff_8_avg(&temperature));
    lcd_buffer_update(&lcd, 0, line);

    char lamp_icon = pinout_is_set(pin_lamp) ? LCD_CAPITAL_OMEGA : ' ';
    lcd_buffer_send_at(&lcd, 0, 15, lamp_icon);

    if (buoy_state == BUOY_UP) {
        lcd_show_time(&lcd, 1, current_time);
    } else {
        lcd_buffer_update_P(&lcd, 1, PSTR("\xBC\xBC Water Low! \xBB\xBB"));
    }
}


void setup_buttons()
{
    ADMUX |= _BV(REFS0) | (PIN_BUTTONS_MUX);  // AVcc <- external capacitor
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1);        // Prescaler: 64
    ADCSRA |= _BV(ADEN);                      // Enable ADC
}


void setup_pump()
{
    // INT0, falling edge
    PIND |= _BV(pin_flow.pin); // enable pullup on INT0 pin
    EIMSK |= _BV(INT0);
    EICRA |= _BV(ISC01);
}


void setup_sd()
{
    usart_print_P(PSTR("Init SD card... "));
    if (!sd_begin(&SD, sd_set_file_date_time)) {
        char num_str[4];

        sd_card_available = false;

        usart_print_P(PSTR("SD CARD init failure: "));
        usart_dump_array_8(&(SD.card.error_code), 1);

        lcd_buffer_update_P(&lcd, 0, PSTR("SD Card Failure"));
        lcd_buffer_update_P(&lcd, 1, PSTR("error code: "));

        itoa(SD.card.error_code, num_str, 10);
        lcd_buffer_update_at(&lcd, 1, 11, num_str);

        _delay_ms(10000);
        return;
    }

    // Open logging directory
    char path[12];
    strncpy_P(path, SD_DIR, 11);

    sd_mkdir(&SD, path);
    log_dir = sd_open(&SD, path, O_READ);
    sd_file_sync(&log_dir);

    sd_card_available = true;
}


void sd_set_file_date_time(uint16_t* date, uint16_t* time)
{
    const time_t tz_time = mk_gmtime(&current_time) - SD_TIMEZONE_OFFSET;
    struct tm ftime;
    gmtime_r(&tz_time, &ftime);

    *date = FAT_DATE(ftime.tm_year + 1900, ftime.tm_mon, ftime.tm_mday);
    *time = FAT_TIME(ftime.tm_hour, ftime.tm_min, ftime.tm_sec);
}


void on_water_level_change(BuoyLevel level)
{
    if (level == BUOY_UP) {
        if (buzzer_is_on) {
            buzzer_turn_off();
        }
    } else {
        // No water!
        pump_state = DISABLED;
        set_lcd_enabled(true);
        buzzer_turn_on();
    }
}


void setup_rtc()
{
    if (!rtc_init(&rtc)) {
        usart_println_P(PSTR("RTC init error"));
        lcd_clear(&lcd);
        lcd_buffer_update_P(&lcd, 0, PSTR("RTC init error"));
        for(;;)
            ; /* block FOREVER */
    }
}


void setup_buzzer()
{
    TCCR2A |= _BV(WGM21);  // Mode CTC: OCRA
}


/*
 * Control the LCD data pins with 2 pins, instead of 4, via the shift register
 */
void shift_lcd_write4(__attribute__((unused)) const Lcd* lcd,
                      const uint8_t nibble)
{
    for (uint8_t i = 0; i < 4; ++i) {
        if ((nibble << i) & 0x08) {
            pinout_set(pin_lcd_data);
        } else {
            pinout_clr(pin_lcd_data);
        }
        pinout_set(pin_lcd_clk);
        pinout_clr(pin_lcd_clk);
    }
}


void setup_lcd()
{
    pinout_set(pin_lcd_backlight);

    lcd.back_buffer = &lcd_buffer;
    lcd_init(&lcd, 2, shift_lcd_write4);
}


void set_lcd_enabled(const bool enabled)
{
    if (enabled) {
    // updates screen timeout
    time_t timeout_at = mk_gmtime(&current_time) + SCREEN_TIMEOUT;
    gmtime_r(&timeout_at, &lcd_timeout_at);

    if (lcd_is_on) {
        return;
    }
    lcd_is_on = true;

    pinout_set(pin_lcd_backlight);
    lcd_display(&lcd, true);
    } else {
        if (!lcd_is_on) {
            return;
        }
        lcd_is_on = false;
        pinout_clr(pin_lcd_backlight);
        lcd_display(&lcd, false);
    }
}


void set_lamp_enabled(const bool enabled)
{
    if (pinout_is_set(pin_lamp) == enabled) {
        return;
    }

    if (enabled) {
        pinout_set(pin_lamp);
    } else {
        pinout_clr(pin_lamp);
    }
    if (sd_card_available) {
        log_lamp(&log_dir, current_time, enabled);
    }
}


void fatal_error_P(const char* lcd0, const char* lcd1)
{
    fatal_error_active = true;
    buzzer_turn_on();
    strncpy_P(fatal_error_lcd0, lcd0, 16);
    strncpy_P(fatal_error_lcd1, lcd1, 16);
}


void check_log()
{
    const struct tm next_time =
        log_next_time(log_last_time, settings.log_period);
    const int32_t time_left = difftime(mk_gmtime(&next_time),
            mk_gmtime(&current_time));
    if (time_left < 0) {
        log_last_time = current_time;

        log_status(&log_dir, current_time, &settings,
                ring_buff_8_avg(&moisture), ring_buff_8_avg(&humidity),
                ring_buff_8_avg(&temperature), buoy_state,
                pinout_is_set(pin_lamp), pump_last_time);
    }
}
