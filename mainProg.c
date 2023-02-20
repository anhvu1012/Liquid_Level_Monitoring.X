/*
 * File:   newmain.c
 * Author: annzi
 *
 * Created on 7. Dezember 2022, 09:12
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include "display.h"
#include "ultrasonic.h"
#include "led.h"
#include "temperatur.h"

// Variables for distance
int level_empty = 13; // Abstand zum Boden
int level_full = 10; // Kapazität 
int liquid_level;
int timerOverflow = 0;
long countZeit;
double distance = 0;
//Variables for temperature
uint16_t value;
uint16_t res;
uint16_t temp;
uint16_t temp_c;
uint16_t color;

int led = 0;

void draw_liquid(double dist, uint16_t c){
    SendCommandSeq(liquid, 6);
    if (dist <= 4){ // Wasserstand bei 10, volle Kapazität
        for(uint16_t i = 0; i < 8468; i++){
            SPISend16Bit(c); 
        }
    }
    else if (dist <= 6 && dist > 4){ // wasserstand bei 8, 4/5 
        for(uint16_t i = 0; i < 6862; i++){
            SPISend16Bit(c); 
        }
    }
    else if (dist <= 8 && dist > 6){ // wasserstand bei 6, 3/5 
        for(uint16_t i = 0; i < 5110; i++){
            SPISend16Bit(c); 
        }
    }
    else if (dist <= 10 && dist > 8){ // wasserstand bei 4, 2/5 
        for(uint16_t i = 0; i < 3504; i++){
            SPISend16Bit(c); 
        }
    }
    else if(dist < 12 && dist > 10){ // wasserstand bei 2, 1/5 
        for(uint16_t i = 0; i < 1752; i++){
            SPISend16Bit(c); 
        }
    }
    
}
void LED_PWM(double dis){
    if (dis < 5) {
        if(led == 0){
            for (int bright = 0; bright < 255; bright +=1){
                SetPWMOutput(bright);
                wait();
            } 
            led = 1;
        }

    } else {
        if(led == 1){
            for (int bright = 255; bright > 0; bright-=1){
                SetPWMOutput(bright);
                wait();
            }
            led = 0;
        }

    }
}


void delayFunction_Timer2(int a)    //8 bit timer
{
    OCR2A = a;    //counter limit
    TCNT2 = 0x00;        //initialise Timer2 bits
    TCCR2A |= (1<<WGM21);        //Timer2, set to CTC mode
    TCCR2B |= (1<<CS20);        //Timer2, CS00 = no pre-scaler
    while ((TIFR2 & (1<<OCF2A)) == 0) {    //count up to OCF2 value
    }
    TCCR2B = 0x00;        //stop clock when OCF2 value is reached
    TIFR2 = (1<<OCF2A);        //set flag to 1
}

ISR(TIMER1_OVF_vect){
    timerOverflow++; // timer reach 255 
}

void main(void) {
    SPI_init();
    Display_init();
    ultrasonic_init();
    adc_init();
    LED_init();
    
    for (uint16_t i = 0; i < 132*176; i++){
        SPISend16Bit(0x0000); // send black background
    }
    
    //comand sequence for the box 1
    SendCommandSeq(window1, 6);
    for(uint16_t i = 0; i < 93*136; i++){
        SPISend16Bit(0xFFFF); //send white
    }
    
    //comand sequence for the box 2
    SendCommandSeq(window2, 6);
    for(uint16_t i = 0; i < 83*126; i++){
        SPISend16Bit(0x0000); //send black
    }
    
    while(1){
        // Ultraschallsensor
        // 10µs trigger pulse sent to ultrasonic
        PORTB |= (1<<PORTB1);
        delayFunction_Timer2(10);
        //_delay_us(10); //timer0 normal mode
        PORTB &= ~(1<<PORTB1);
        
        // Input capture
        TCNT1 = 0;          // clear timer counter
        TCCR1B = 0x41;      // capture input on rising edge, no prescaler
        TIFR1 = 1<<ICF1;    // clear input capture flag
        TIFR1 = 1<<TOV1;    // clear timer overflow flag
        
        // Calculating width of echo signal 
        while((TIFR1 & (1<<ICF1)) == 0); //waiting for rising edge
        TCNT1 = 0;              // clear timer counter
        TCCR1B = 0x01;          // capure on falling edge, no prescaler
        TIFR1 = 1<<ICF1;    // clear input capture flag
        TIFR1 = 1<<TOV1;    // clear timer overflow flag
        timerOverflow = 0;  // clear timer overflow count
        
        while((TIFR1 & (1<<ICF1)) == 0); //waiting for rising edge
        countZeit = ICR1 + (65535 * timerOverflow); // Take count
        // 16MHz Timer frequenz, sound velocity = 343m/s
        distance = (double) countZeit / 941.12;
        
        //Temperatur
        value = adc_value();   //no interrupt
        // Widerstand berechnen, U = 5V, 8 Bit ADC => 256 
        // Formel: Rptc = Rv * AD / (256 - AD) 
        res = 100 * (uint32_t)value / (256 - value);
        // Temperatur berechnen
        // Formel: t = (R(t)/R(t0) - 1) / alpha(t0)
        // t = (res/100 - 1) / 3.850 * 0.001 => Ausgabe in Kelvin
        temp = (res / 100 - 1) / 3.850 * 0.001;
        // Umrechnen nach Celsius
        // 0 K = -273,15 °C und 0°C = 273,15 K
        temp_c = temp - 273.15;
        
        // decide for a color
        if(temp_c < 25){ 
            color = 0x001F; //blue 
        }else if (temp_c >= 25 && temp_c <60){ 
            color = 0xFFE0; //yellow
        }else if (temp_c >= 60){ 
            color = 0xF800; //red
        }
        
        // Draw on TFT
        draw_liquid(distance, color);
        
        // operate led
        LED_PWM(distance);
    }
    
}
