/*
 * File:   display.c
 * Author: annzi
 *
 * Created on 27. November 2022, 16:26
 */

#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include "display.h"

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

void SPI_init(){
	//set CS, MOSI and SCK to output:
	SPI_DDR |= (1 << CS) | (1 << MOSI) | (1 << SCK);

	//enable SPI, set as master, and clock to fosc/4:
	SPCR = (1 << SPE) | (1 << MSTR);
}

void SPISend16Bit(uint16_t data){
    uint8_t SendeByte;
    SendeByte = (data >> 8) & 0xFF;	//High-Byte des Kommandos
	SPISend8Bit(SendeByte);
	SendeByte = data & 0xFF;			//Low-Byte des Kommandos
	SPISend8Bit(SendeByte);
}

void SPISend8Bit(uint8_t data){
	PORTB &= ~(1<<CS);				//CS low
	SPDR = data;					//load data into register
	while(!(SPSR & (1 << SPIF)));	//wait for transmission complete
	PORTB |= (1<<CS);     			//CS high
}

void SendCommandSeq(const uint16_t * data, uint16_t Anzahl){
	uint16_t index;
	uint8_t SendeByte;
	for (index=0; index<Anzahl; index++){
		PORTD |= (1<<D_C);						//Data/Command auf High => Kommando-Modus
		SendeByte = (data[index] >> 8) & 0xFF;	//High-Byte des Kommandos
		SPISend8Bit(SendeByte);
		SendeByte = data[index] & 0xFF;			//Low-Byte des Kommandos
		SPISend8Bit(SendeByte);
		PORTD &= ~(1<<D_C);						//Data/Command auf Low => Daten-Modus
	}
}

void Display_init(void) {
    DDRD |= (1<<D_C)|(1<<Reset);		//output: PD2 -> Data/Command; PD3 -> Reset
    
	const uint16_t InitData[] ={
		//Initialisierungsdaten fuer 16Bit-Farben-Modus
		0xFDFD, 0xFDFD,
		/* pause */
		0xEF00, 0xEE04, 0x1B04, 0xFEFE, 0xFEFE,
		0xEF90, 0x4A04, 0x7F3F, 0xEE04, 0x4306,
		/* pause */
		0xEF90, 0x0983, 0x0800, 0x0BAF, 0x0A00,
		0x0500, 0x0600, 0x0700, 0xEF00, 0xEE0C,
		0xEF90, 0x0080, 0xEFB0, 0x4902, 0xEF00,
		0x7F01, 0xE181, 0xE202, 0xE276, 0xE183,
		0x8001, 0xEF90, 0x0000,
        /* pause */
        0xEF08, 0x1803, //Portrait modus 180 gedreht
        0x1283, 0x13AF, 0x1500, 0x1600 //Full Display
	};
	
	_delay_ms(300);
	PORTD &= !(1<<Reset);			//Reset-Eingang des Displays auf Low => Beginn Hardware-Reset
	_delay_ms(75);
	PORTB |= (1<<CS);				//SSEL auf High
	_delay_ms(75);
	PORTD |= (1<<D_C);				//Data/Command auf High
	_delay_ms(75);
	PORTD |= (1<<Reset);			//Reset-Eingang des Displays auf High => Ende Hardware Reset
	_delay_ms(75);
	SendCommandSeq(&InitData[0], 2);
	_delay_ms(75);
	SendCommandSeq(&InitData[2], 10);
	_delay_ms(75);
	SendCommandSeq(&InitData[12], 23);
    _delay_ms(75);
	SendCommandSeq(&InitData[35], 6);
}
