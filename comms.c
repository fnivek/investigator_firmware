/*
 * comms.c
 *
 *  Created on: Oct 7, 2015
 *      Author: fnivek
 */


#include "comms.h"

// Sets up SPI for communication with both the IMU and ODROID
// SPI P1.1 MISO, P1.2 MOSI, P1.4 CLK, P1.5 UCA0STE
// Uses UARTx on pins ...
void InitComms(void) {
	// Disable USCI
	UCA0CTL1 |= UCSWRST;

	// Set up pin functions and Directions
	P1SEL |= 0x36;
	P1SEL2 |= 0x36;
	P1DIR &= ~0x34;			// P1.2, P1.4, P1.5 In
	P1DIR |= 0x2;			// P1.1 Out

	// Set slave mode active low cs
	UCA0CTL0 = UCMSB | UCSYNC | UCMODE_2 | UCCKPH;

	// Enable interrupts
	// IE2 |= ...

	// Turn on USCI in SPI mode
	UCA0CTL1 &= ~UCSWRST;
}

// Function to test comms
// Fill Transmit buffer if possible and returns 1
// If busy will return 0
uint8_t TestComms(uint8_t to_write) {
	if (!(IFG2 & UCA0TXIFG))
		return 0;

	UCA0TXBUF = to_write;
	return 1;
}
