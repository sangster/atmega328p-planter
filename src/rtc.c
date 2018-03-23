#include "rtc.h"


void lcd_show_time(Lcd* lcd, uint8_t row, struct tm current_time)
{
    char rtc_string[25], line[14];

    current_time.tm_sec = 0;

    asctime_r(&current_time, rtc_string);

    strncpy(line, &rtc_string[4], 6);
    line[6] = ',';
    line[7] = ' ';
    strncpy(&line[8], &rtc_string[11], 5);
    line[13] = '\0';

    lcd_buffer_update(lcd, row, line);
}
