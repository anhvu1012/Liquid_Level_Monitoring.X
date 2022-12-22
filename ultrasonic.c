/*
 * File:   ultrasonic.c
 * Author: annzi
 *
 * Created on 7. Dezember 2022, 16:59
 */

#include <stdio.h>
#include "ultrasonic.h"

// max volume fehlt => für später max volume - distance = wasserstand
int timerOverflow = 0;
long countZeit;
double distance;

ISR(TIMER1_OVF_vect){
    timerOverflow++; // timer reach 255 
}

void ultrasonic_init(void){
    // trigger pin as output
    DDRB |= (1<<DDB1);
    // pull up for echo(input) enable
    PORTB |= (1<<PORTB0);
    
    sei();                  // global interrupt enable
    TIMSK1 = (1<<TOIE1);    // Timer1 overflow interrupt enable
    TCCR1A = 0;             // normal port operation, all bits to 0
    
}

double ultrasonic_main(void) {
    ultrasonic_init();
    
    while(1){
        // 10µs trigger pulse sent to ultrasonic
        PORTB |= (1<<PORTB1);
        _delay_us(10); //timer0 normal mode
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
        return distance;
        //printf("Distance: %1f", distance);
    }
}
