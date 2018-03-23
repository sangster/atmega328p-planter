#include "sensors.h"

BuoyLevel buoy_state = BUOY_NO_READING;


void read_buoyancy(Pinout pin_buoy, BuoyOnChange callback)
{
    const uint8_t reading = !pinout_is_set(pin_buoy);

    if (reading != buoy_state) {
        buoy_state = (reading == BUOY_UP) ? BUOY_UP : BUOY_DOWN;
        callback(buoy_state);
    }
}


void read_moisture(uint8_t adc_mask, RingBuff8* moisture)
{
    ADMUX = (ADMUX & 0xF0) | adc_mask;     // Select pin ADC2 (A2)
    ADCSRA |= _BV(ADSC);                   // start comparison
    loop_until_bit_is_clear(ADCSRA, ADSC); // wait until conversion complete
    ring_buff_8_push(moisture, map_moisture(ADC));
}


void sensors_to_s(char line[16], uint8_t moisture, uint8_t humidity,
                  uint8_t temperature)
{
    char *p;
    p = &line[0];

    *(p++) = 'M';
    utoa(moisture, p, 10);
    p += strnlen(p, 4);
    *(p++) = '%';
    *(p++) = ' ';

    *(p++) = 'H';
    utoa(humidity, p, 10);
    p += strnlen(p, 4);
    *(p++) = '%';
    *(p++) = ' ';

    utoa(temperature, p, 10);
    p += strnlen(p, 4);
    *(p++) = (char) LCD_DEGREE;

    *(p++) = '\0';
}


bool read_humidity_temperature(Pinout pin, RingBuff8* h, RingBuff8* t)
{
    if (mhum_read(pin, &humidity_f, &temperature_f)) {
        ring_buff_8_push(h, humidity_f.fixed.integral);
        ring_buff_8_push(t, temperature_f.fixed.integral);
        return true;
    }
    return false;
}
