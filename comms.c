/*
 * comms.c
 *
 *  Created on: Oct 7, 2015
 *      Author: fnivek
 */


#include "comms.h"

// Sets up SPI for communication with both the IMU and ODROID
// SPI P1.1 MISO, P1.2 MOSI, P1.4 CLK
// Uses UARTx on pins ...
void InitComms(void) {
	// Disable USCI
	UCA0CTL1 |= UCSWRST;

	// Set up pin functions and Directions
	P1SEL |= 0x16;
	P1SEL2 |= 0x16;
	P1DIR |= 0x14;			// P1.2, P1.4 Out
	P1DIR &= ~0x2;			// P1.1 In

	// Set mode and clock select
	UCA0CTL0 = UCMSB | UCMST | UCSYNC;
	UCA0CTL1 = UCSSEL_3 | UCSWRST;

	// Set clock divider to 255
	UCA0BR0 = 0xFF;
	UCA0BR1 = 0x0;

	// Enable interrupts
	// IE2 |= ...

	// Turn on USCI in SPI mode
	UCA0CTL1 &= ~UCSWRST;
}

void TestComms(uint8_t thing) {
	if (UCA0STAT & UCBUSY)
		return;

	UCA0TXBUF = thing;
}
