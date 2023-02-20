/*
 * File:   T_main.c
 * Author: annzi
 *
 * Created on 30. November 2022, 15:56
 */


#include "temperatur.h"

void adc_init(void){
    // ADC0 port as input
    DDRC &= ~(1<<DDC0);
    
    //Configure ADC, 8 Bit Genauigkeit => nur ADCH Register lesen
    
    //without interrupt
    ADMUX = 0x60; // left adjust, A0 is input, internal Vref 5V
    ADCSRA = 0x87; // enable ADC, Prescaler 128, no interrupt, no auto trigger 
    ADCSRB = 0;
    
    
    /*
    ADMUX = 0x60; // left adjust, A0 is input, internal Vref 5V
    ADCSRA = 0xcf; // enable ADC, start conversion, Prescaler 128, interrupt, no auto trigger 
    ADCSRB = 0;
    
    sei(); 
    */
}

//without interrupt
int adc_value(){
    //Start ADC
    ADCSRA |= (1<<ADSC); // start converting
    
    //ADSC will read as one as long as a conversion is in progress. 
    //When the conversion is complete, it returns to zero
    while ((ADCSRA &(1<<ADIF))==0); // end of conversion
    ADCSRA |= (1<<ADIF); // write 1 to set this bit
    return(ADCH);      // return conversion result 
}

/*
// occur when conversion ends
ISR(ADC_vect) {
    value = ADCH;
}

double getTemperatur(){
    value = adc_value();   no interrupt
    
    
    // Widerstand berechnen, U = 5V, 8 Bit ADC => 256 
    // Formel: Rptc = Rv * AD / (256 - AD) 
    res = 100 * value / (256 - value);
    
    // Temperatur berechnen
    // Formel: t = (R(t)/R(t0) - 1) / alpha(t0)
    // t = (res/100 - 1) / 3.850 * 0.001 => Ausgabe in Kelvin
    temp = (res / 100 - 1) / 3.850 * 0.001;
    
    // Umrechnen nach Celsius
    // 0 K = -273,15 °C und 0°C = 273,15 K
    temp = temp - 273.15;
    return temp;
}

*/
