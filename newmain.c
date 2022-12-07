/*
 * File:   newmain.c
 * Author: annzi
 *
 * Created on 7. Dezember 2022, 09:12
 */


#include <avr/io.h>
#include "display.h"

void main(void) {
    SPI_init();
    Display_init();
    
    const uint16_t window1[] ={
        0xEF08, 0x1803, //Portrait modus 180 gedreht
        0x1270, //x1
        0x139C, //y1
        0x1514, //x2
        0x1614  //y2
    };
    
    const uint16_t window2[] ={
        0xEF08, 0x1803, //Portrait modus 180 gedreht
        0x126B, //x1
        0x1397, //y1
        0x1519, //x2
        0x1619  //y2
    };  
    const uint16_t liquid[] ={
        0xEF08, 0x1803, //Portrait modus 180 gedreht
        0x1266, //x1
        0x1392, //y1
        0x151E, //x2
        0x161E  //y2
    };
    
    for (uint16_t i = 0; i < 132*176; i++){
        SPISend16Bit(0x0000);          // send black
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
    //comand sequence for the liquid box
    SendCommandSeq(liquid, 6);

    for(uint16_t i = 0; i < 73*116; i++){
    SPISend16Bit(0xF800); //send red
    }
    //SPISend16Bit(0x001F); //send blue
    //SPISend16Bit(0xFFE0); //send yellow
   
    while(1){;}
    return;
}
