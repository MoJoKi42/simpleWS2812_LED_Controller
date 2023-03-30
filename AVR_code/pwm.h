#include <avr/io.h>
#include <stdlib.h>

// Simple PWM Lib
// ACHTUNG: Die PWM1 Funktionen müssen noch getestet werden!!! evtl. passt es nicht
//          weil es 16 Bit Timer sind!

#define PWM0_N_1    (_BV(CS00))
#define PWM0_N_8    (_BV(CS01))
#define PWM0_N_64   (_BV(CS01)|_BV(CS00))
#define PWM0_N_256  (_BV(CS02))
#define PWM0_N_1024 (_BV(CS02)|_BV(CS00))

#define PWM1_N_1    (_BV(CS10))
#define PWM1_N_8    (_BV(CS11))
#define PWM1_N_64   (_BV(CS11)|_BV(CS10))
#define PWM1_N_256  (_BV(CS12))
#define PWM1_N_1024 (_BV(CS12)|_BV(CS10))


// Init
void pwm0_init(void);
void pwm1_init(void);

// Set frequency
void pwm0_set_frequency(uint32_t N);
void pwm1_set_frequency(uint32_t N);

// Set value
void pwm0_set_value(char output, uint8_t value);
void pwm1_set_value(char output, uint8_t value);






void pwm0_set_value(char output, uint8_t value) {
    if (output == 'A') {
        OCR0A = value;
        if (value == 0) { TCCR0A &= ~(1<<COM0A1); } else { TCCR0A |= (1<<COM0A1); }
    }
    if (output == 'B') {
        OCR0B = value;
        if (value == 0) { TCCR0A &= ~(1<<COM0B1); } else { TCCR0A |= (1<<COM0B1); }
    }
}

void pwm1_set_value(char output, uint8_t value) {
    if (output == 'A') {
        OCR1A = value;
        if (value == 0) { TCCR1A &= ~(1<<COM1A1); } else { TCCR1A |= (1<<COM1A1); }
    }
    if (output == 'B') {
        OCR1B = value;
        if (value == 0) { TCCR1A &= ~(1<<COM1B1); } else { TCCR1A |= (1<<COM1B1); }
    }
}

void pwm0_set_frequency(uint32_t N) {
    TCCR0B = (TCCR0B & ~((1<<CS02)|(1<<CS01)|(1<<CS00))) | N; // set prescaler
}

void pwm1_set_frequency(uint32_t N) {
    TCCR1B = (TCCR1B & ~((1<<CS12)|(1<<CS11)|(1<<CS10))) | N; // set prescaler
}

void pwm0_init(void) {
    OCR0A = 0;
    OCR0B = 0;
    TCCR0A   = (1<<WGM01)  | (1<<WGM00);
    TCCR0A	|= (1<<COM0A1) | (1<<COM0B1); // Enable OC0A + OC0B Output Pin
}

void pwm1_init(void) {
    OCR1A = 0;
    OCR1B = 0;
    TCCR1A	 = (1<<WGM11)  | (1<<WGM10);
    TCCR1A	|= (1<<COM1A1) | (1<<COM1B1); // Enable OC1A + OC1B Output Pin
}

