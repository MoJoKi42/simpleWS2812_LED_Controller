#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "adc.h"
#include "light_ws2812.h"
#include "hsv_fct.h"

// Befehle :
//
// -> Kompilieren
// make all
//
// -> Flashen
// avrdude -p m328p -c usbasp -e -U flash:w:main.hex
//
// -> Fuses schreiben
// avrdude -c usbasp -p m328p -U lfuse:w:0xE2:m -U hfuse:w:0xD9:m -U efuse:w:0xFF:m
//
// -> VS Code Include Pfad
// "/usr/lib/avr/include"
//


// Prototypen
void Timer2_Init(void);
struct cRGB hsv_to_cRGB(uint16_t hsv_color);


// globale Variablen + Makros
#define PWR_LED_on   PORTC &= ~(1<<PC2)
#define PWR_LED_off  PORTC |=  (1<<PC2)
#define BUTTON !(PINC & (1<<PINC0))

volatile uint32_t millis_counter = 0;
uint16_t helligkeit = 0;

// Aktualisierungsrate LEDs (ms)
uint8_t refresh_rate_LED = 20;

// LED Farben Zwischenspeicher (60 LEDs/m)
#define max_LEDs 300
struct cRGB LED[max_LEDs];


int main(void) {

    // In/Outputs
    PORTC |= (1<<PC0); // PullUp Button
    DDRC  |= (1<<PC2); // Output LED
    PWR_LED_on;

    // ADC initalisieren
    adc_init();
    
    // Timer initalisieren
    Timer2_Init();

    // LED Init
    struct cRGB temp;
    temp.r = 0; temp.g = 0; temp.b = 0;
    for (int i=0; i<max_LEDs; i++) {
        LED[i] = temp;
    }

    // Interrups aktivieren
    sei();
    
    // Effekt Merker
    uint8_t current_effect = 0;
    #define max_PROG 9
    // 0 = bewegter Regenbogen
    // 1 = Farbwechsel
    // 2 = rot
    // 3 = grün
    // 4 = blau
    // 5 = lila
    // 6 = grün-gelb
    // 7 = orange
    // 8 = weiß


    // Softstart
    static uint8_t softstart = 1;

    while(1) {
        
        // ADC auslesen
        helligkeit = (uint16_t)(adc_read_avg(6, 15));
        float helligkeit_float = (float)helligkeit / 1023.0;
        uint8_t hell_8bit = helligkeit/4;
        
        
        // Taster abfragen
        static uint32_t time_button = 0;
        if (BUTTON && millis_counter >= time_button && !softstart) {
            current_effect++;
            if (current_effect >= max_PROG)
                current_effect = 0;
            time_button = millis_counter + 150;
        }
        
        // hsv_val Inkrement
        static uint32_t time_hsv_val = 0;
        static uint16_t hsv_val = 0;
        if (millis_counter >= time_hsv_val) {
            hsv_val++;
            if (hsv_val >= 360)
                hsv_val = 0;
            if (current_effect == 0)
                time_hsv_val = millis_counter + 15; // etwas schneller für den Regenbogen Effekt
            else
                time_hsv_val = millis_counter + 50;
        }
        
        
        /////////////////////////////////
        // 0 Regenbogen Effekt
        /////////////////////////////////
        if(current_effect == 0) {
            for (uint16_t i=0; i<max_LEDs; i++) {
                
                // Berechne passenden HSV Wert
                uint32_t offset = ((uint32_t)i * (uint32_t)360) / (uint32_t)max_LEDs;
                offset = offset + hsv_val;
                offset = offset % 360;
                
                // Neue LED Farbwerte schreiben
                LED[i].r = ((uint16_t)HSV_to_Red(offset) * (uint16_t)hell_8bit) >> 8;
                LED[i].g = ((uint16_t)HSV_to_Green(offset) * (uint16_t)hell_8bit) >> 8;
                LED[i].b = ((uint16_t)HSV_to_Blue(offset) * (uint16_t)hell_8bit) >> 8;
                
                if (softstart)
                    _delay_ms(5);
            }
        }
        
        /////////////////////////////////
        // 1 Farbwechsel Effekt
        /////////////////////////////////
        if(current_effect == 1) {
            for (uint16_t i=0; i<max_LEDs; i++) {
                LED[i].r = (float)HSV_to_Red(hsv_val) * helligkeit_float;
                LED[i].g = (float)HSV_to_Green(hsv_val) * helligkeit_float;
                LED[i].b = (float)HSV_to_Blue(hsv_val) * helligkeit_float;
            }
        }
        
        /////////////////////////////////
        // 2-7 Fixe Farben
        /////////////////////////////////
        if(current_effect >= 2 && current_effect <= 7) {
            
            // Farbe festlegen
            uint16_t color = 0;
            switch(current_effect) {
                case 2: color = 0;      break;  // rot
                case 3: color = 120;    break;  // grün
                case 4: color = 240;    break;  // blau
                case 5: color = 320;    break;  // lila
                case 6: color = 90;     break;  // grün-gelb
                case 7: color = 30;     break;  // orange
                default: break;
            }
                
            // Neue LED Farbwerte schreiben
            for (uint16_t i=0; i<max_LEDs; i++) {
                LED[i].r = (float)HSV_to_Red(color) * helligkeit_float;
                LED[i].g = (float)HSV_to_Green(color) * helligkeit_float;
                LED[i].b = (float)HSV_to_Blue(color) * helligkeit_float;
            }
        }
        
        
        /////////////////////////////////
        // 8 Weiß
        /////////////////////////////////
        if(current_effect == 8) {
            for (uint16_t i=0; i<max_LEDs; i++) {
                LED[i].r = 180 * helligkeit_float;
                LED[i].g = 180 * helligkeit_float;
                LED[i].b = 180 * helligkeit_float;
            }
        }
        
        // Softstart deaktivieren
        softstart = 0;
    }
}




// Timer2 Interrupt
ISR(TIMER2_OVF_vect) {
    
    // Millisekundenzähler inkrementieren
    millis_counter++;
    
    // LEDs aktualisieren
    static uint8_t time_LED_refresh = 0;
    time_LED_refresh++;
    if (time_LED_refresh >= refresh_rate_LED) {
        
        // Interrups deaktivieren
        //cli();
        
        // LEDs aktualisieren (~11ms für 300 LEDs)
        ws2812_setleds(LED, max_LEDs);
        
        // Interrups aktivieren
        time_LED_refresh = 0;
        //millis_counter += 10; // verpasste counts inkrementieren
        //sei();
    }
}

// Timer 2 initalisieren (für ms Zeitbasis)
void Timer2_Init(void) {
    TCCR2A = 0x00;                      // Normalmodus
    TCCR2B = (1<<CS21) | (1<<CS20);     // Prescaler = 64
    TIMSK2 = (1<<TOIE2);                // Timer0 overflow interrup enable
    TCNT2 =  6;                         // Vorladewert 6
}









