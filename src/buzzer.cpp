#include "buzzer.hh"
#include <Arduino.h>

namespace Buzzer {
    volatile uint16_t toggles_left = 0;
    constexpr uint16_t MILLISECONDS = 1000;
    constexpr uint16_t PRESCALER = 256;
    constexpr uint8_t PRESCALER_BITS = (1 << CS22) | (1 << CS21);

    constexpr uint16_t PADDLE_FREQ = 2200;
    constexpr uint8_t PADDLE_TIME_MS = 30;

    constexpr uint16_t SCORE_FREQ = 1500;
    constexpr uint8_t SCORE_TIME_MS = 80;

    constexpr uint16_t SETTING_FREQ = 1700;
    constexpr uint8_t SETTING_TIME_MS = 50;

    void stopPWM() {
        uint8_t old_sreg = SREG;
        cli();
        TIMSK2 = 0;
        TCCR2A = 0;
        TCCR2B = 0;
        TCNT2 = 0;
        PORTB &= ~(1 << PB4); // D10 -> LOW
        SREG = old_sreg;
    }

    void startPWM(uint16_t freq, uint16_t time) {
        if (freq == 0 || time == 0) { stopPWM(); return; }
        // TODO (?): Think of edge cases that could under/overflow (the freq and time is set manually above so not so big issue)
        uint8_t ocrnx = (F_CPU / (2UL * PRESCALER * freq)) - 1; // from output frequency formula
        uint16_t toggles_total = (2UL * freq * time) / MILLISECONDS; // total LOW <--> HIGH switches in the set time

        uint8_t old_sreg = SREG;
        cli();
        TCCR2B = 0;
        TCNT2 = 0;
        toggles_left = toggles_total;
        OCR2A = ocrnx;
        TCCR2A = (1 << WGM21) | (1 << COM2A0); // CTC mode ON and OC2A toggle match
        TIFR2 = (1 << OCF2A); // clear interrupt flag (if there would be old one hanging)
        TIMSK2 = (1 << OCIE2A); // to compare interrupt
        TCCR2B = PRESCALER_BITS;
        SREG = old_sreg;
    }

    void play(Sound sound) {
        switch (sound) {
        case PADDLE: startPWM(PADDLE_FREQ, PADDLE_TIME_MS); break;
        case SCORE: startPWM(SCORE_FREQ, SCORE_TIME_MS); break;
        case SETTING: startPWM(SETTING_FREQ, SETTING_TIME_MS); break;
        default: break;
        }
    }

    void initialization() {
        DDRB |= (1 << PB4); // D10 -> OUTPUT
        stopPWM();
    }
}

ISR(TIMER2_COMPA_vect) {
    if (Buzzer::toggles_left > 0) {
        Buzzer::toggles_left--;
    } else {
        Buzzer::stopPWM();
    }
}