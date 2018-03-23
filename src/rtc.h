#ifndef RTC_H
#define RTC_H

#include <time.h>
#include <avr/io.h>
#include <sangster/lcd.h>
#include <sangster/rtc_1307.h>

void lcd_show_time(Lcd*, uint8_t row, struct tm);

#endif//RTC_H
