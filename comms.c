/*
 * comms.c
 *
 *  Created on: Oct 7, 2015
 *      Author: fnivek
 */


#include "comms.h"

// Global vars
queue RXBuf;
queue TXBuf;

// Sets up SPI for communication with both the IMU and ODROID
// SPI P1.1 MISO, P1.2 MOSI, P1.4 CLK, P1.5 UCA0STE
// Uses UARTx on pins ...
void InitComms(void) {
	// Disable USCI
	UCA0CTL1 |= UCSWRST;

	// Set up pin functions and Directions
	P1SEL |= 0x36;
	P1SEL2 |= 0x36;
	P1DIR |= 0x14;			// P1.2, P1.4, P1.5 In
	P1DIR &= ~0x2;			// P1.1 Out

	// Set slave mode active low cs
	UCA0CTL0 = UCMSB | UCSYNC | UCMODE_2;
	UCA0CTL1 = UCSSEL_3 | UCSWRST;
	UCA0BR0 = 0x00;
	UCA0BR1 = 0x00;

	// Enable interrupts
	//IE2 |= UCA0RXIE | UCA0TXBUF;

	// Turn on USCI in SPI mode
	UCA0CTL1 &= ~UCSWRST;
}

// Transfer one byte over SPI
void Transmit(uint8_t out) {
	if (!QueueInsert(&TXBuf, out)) {
		// Handle Queue overflow
		__disable_interrupt();
		for (;;); // This is here to intentionally break the code
	}

	// Turn the transmit interrupt back on
	if(QueueSize(&TXBuf) == 1) {
		IE2 |= UCA0TXIE;
	}
}

// Transmit interrupt
// While TXBuf has data transmit
// When there is no data left in TXBuf turn off the interupt enable
#pragma vector=USCIAB0TX_VECTOR
__interrupt void TXISR() {

	// UART
	uint8_t out = 0;
	if(QueuePop(&TXBuf, &out)) {
		UCA0TXBUF = out;
	}
	else {
		// Done transfering disable transmit interrupt
		IE2 &= ~UCA0TXIE;
	}
}

// Recieve Interrupt
#pragma vector=USCIAB0RX_VECTOR
__interrupt void RXISR() {
	if (!QueueInsert(&RXBuf, UCA0RXBUF)) {
		// TODO: Handle RXBuf overflow
	}
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
