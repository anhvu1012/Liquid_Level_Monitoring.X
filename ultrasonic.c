/*
 * File:   ultrasonic.c
 * Author: annzi
 *
 * Created on 7. Dezember 2022, 16:59
 */

//#include <stdio.h>
#include "ultrasonic.h"
/* in mainProg
 
// max volume fehlt => für später max volume - distance = wasserstand
int timerOverflow = 0;
long countZeit;
double distance;

ISR(TIMER1_OVF_vect){
    timerOverflow++; // timer reach 255 
}
*/
void ultrasonic_init(void){
    // trigger pin as output
    DDRB |= (1<<DDB1);
    // pull up for echo(input) enable
    PORTB |= (1<<PORTB0);
    
    sei();                  // global interrupt enable
    TIMSK1 = (1<<TOIE1);    // Timer1 overflow interrupt enable
    TCCR1A = 0;             // all bits to 0, normal operation
    
}
