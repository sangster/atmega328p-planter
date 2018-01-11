#include "buzzer.h"

bool buzzer_is_on = false;
const uint16_t tune[] = {
    NOTE_D0, NOTE_D1, NOTE_D2, NOTE_D3, NOTE_D4, NOTE_D5, NOTE_D6, NOTE_D7,
    NOTE_DL1, NOTE_DL2, NOTE_DL3, NOTE_DL4, NOTE_DL5, NOTE_DL6, NOTE_DL7,
    NOTE_DH1, NOTE_DH2, NOTE_DH3, NOTE_DH4, NOTE_DH5, NOTE_DH6, NOTE_DH7,
};

const float duration[] = {
    WHOLE, QUARTER, WHOLE, QUARTER, WHOLE, QUARTER, WHOLE, QUARTER, WHOLE,
    QUARTER, WHOLE, QUARTER, WHOLE, QUARTER, WHOLE, QUARTER, WHOLE, QUARTER,
    WHOLE, QUARTER, WHOLE, QUARTER
};
static const uint8_t buzzer_tune_length = sizeof(tune) / sizeof(tune[0]);

static uint8_t note_index = 0;
static uint16_t note_ends_at = 0;



static uint16_t tone_prescale(const uint16_t frequency);
static void set_prescaler(const uint16_t prescale);


void buzzer_turn_on()
{
    buzzer_is_on = true;
    timer0_start();
}


void buzzer_turn_off()
{
    TIMSK2 &= ~_BV(OCIE2A);
    buzzer_is_on = false;
    note_index = 0;
    note_ends_at = 0;
}


void buzzer_sound(Pinout buzzer_pin)
{
    if (!buzzer_is_on) {
        note_index = 0;
        note_ends_at = 0;
        pinout_clr(buzzer_pin);
        return;
    }

    const uint16_t now = timer0_ms();
    if (now > note_ends_at) {
        pinout_clr(buzzer_pin);

        const uint16_t prescale = tone_prescale(tune[note_index]);
        set_prescaler(prescale);

        OCR2A = F_CPU / tune[note_index] / 2 / prescale - 1;
        TIMSK2 |= _BV(OCIE2A);

        note_ends_at = now + WHOLE_MILLIS * duration[note_index];
        note_index = (note_index + 1) % buzzer_tune_length;
    }
}



uint16_t tone_prescale(const uint16_t frequency)
{
    static const uint16_t scales[] = {1, 8, 32, 64, 128, 256, 1024};
    const uint32_t base = F_CPU / frequency / 2;

    for (uint8_t i = 0; i < 6; ++i) {
        if (base / scales[i] <= 256) {
            return scales[i];
        }
    }
    return scales[6];
}


void set_prescaler(const uint16_t prescale)
{
    switch (prescale) {
        case 1:
            TCCR2B = _BV(CS20);
            break;
        case 8:
            TCCR2B = _BV(CS21);
            break;
        case 32:
            TCCR2B = _BV(CS21) | _BV(CS20);
            break;
        case 64:
            TCCR2B = _BV(CS22);
            break;
        case 128:
            TCCR2B = _BV(CS22) | _BV(CS20);
            break;
        case 256:
            TCCR2B = _BV(CS22) | _BV(CS21);
            break;
        default: //1024
            TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20);
            break;
    }
}
