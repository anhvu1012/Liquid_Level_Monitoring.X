/*
 * File:   T_main.c
 * Author: annzi
 *
 * Created on 30. November 2022, 15:56
 */


#include "temperatur.h"

void adc_init(void){
    //Configure ADC
    ADMUX = 0x40; // right adjust, A0 is input, internal Vref 5V
    ADCSRA = 0xFF; // enable ADC, free running mode, Prescaler 128, interrupt enable, Autoconv enable
    ADCSRB = 0;
}

void Temp_main(void) {
    return;
}
