/*
 * File:   LEDmain.c
 * Author: 1512l
 *
 * Created on November 28, 2022, 3:12 PM
 */

#include "led.h"

void LED_init(void) {
    // LED as output
    DDRD |= (1<<DDD5);
    
    // PWM Timer
        // Set PWM mode
    TCCR0A |= (1<<WGM01)|(1<<WGM00); 
    TCCR0B &= ~(1<<WGM02); 
        // Prescaler 64
    TCCR0B |= (1<<CS00) | (1<<CS01); 
    TCCR0B &= ~(1<<CS02);
        // Set Timer Match B
        // Clear OC0B on compare match, set OC0B at BOTTOM,(non-inverting mode)
    TCCR0A |= (1<<COM0B1);
    TCCR0A &= ~(1<<COM0B0);
}

void SetPWNOutput(int duty) {
    OCR0B = duty;
}

void wait(){
    _delay_loop_2(3200);
}
void LED_main(void) {
    LED_init();
    while(1) {
        for (int bright = 0; bright < 255; bright ++){
            SetPWNOutput(bright);
            wait();
        }
        
        for (int bright = 255; bright > 0; bright--){
            SetPWNOutput(bright);
            wait();
        }
    }
}
