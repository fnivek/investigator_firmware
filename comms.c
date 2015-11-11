/*
 * comms.c
 *
 *  Created on: Oct 7, 2015
 *      Author: fnivek
 */


#include "comms.h"

// Global vars
queue TXBuf;
uint8_t motor_dir = 0;
uint32_t motor_speed = 0;

// Externals
extern uint16_t distin[];
extern const uint8_t kNumSonars;
extern uint16_t diff1;
extern uint16_t diff2;
extern uint32_t encodercount1;
extern uint32_t encodercount2;
extern uint8_t g_ldir;
extern uint8_t g_rdir;

// Number of bytes to recieve on a write cycle
uint8_t num_bytes_in = 0;

// Enumerator for spi addresses
enum addresses {
	SONAR = 1,			// Read only, four 2 byte values little end first [SONAR_0_L, SONAR_0_H, SONAR_1_L, ...]
	ENCODERS_SPEED,		// Read only, one one byte return and two 2 byte returns [direction, left, right]  bit 0 of direction is left_dir, bit 1 of direction is right_dir
	ENCODERS_POSITION,	// Read only, two 4 byte returns [left, right]
	MOTORS,				// Write only, one one byte value and 2 floating point values [direction, left, right], bit 0 of direction controls left, bit 1 of direction controls right
	SET_STATUS,			// Write only, one byte value [status]
	GET_STATUS,			// Read only, one byte returns [status]
};

// Holds the current address
enum addresses current_addr = SONAR;

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
	// UCCKPH = 0, UCCKPL = 0, works with odroid in mode 0b01
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
// Note this function has a lot in it but only a few things happen at a time.
//	if it does however become cumbersome computationally, an RX buffer can be added
//	and processing can occur in the main loop all reads should occur in here though
//	because they are time critical. So only the writes can be moved out
#pragma vector=USCIAB0RX_VECTOR
__interrupt void RXISR() {
	// Grab value
	uint8_t value = UCA0RXBUF;
	//RingbufPush(&RXBuf, value);
	//Transmit(value);

	// Check if this is a new read or write by seeing if we are waiting
	//		for more bytes to come in or if we are currently transmiting data
	if(num_bytes_in == 0 && !QueueSize(&TXBuf)) {
		// New read or write
		// Remove the read write bit of the address, that is only used for the IMU
		value &= 0x7F;
		current_addr = value;
		// Switch through all the addresses
		switch (current_addr) {
		case SONAR:
			{
				// Add the four sonars
				uint8_t i;
				for(i = 0; i < kNumSonars; ++i)
				{
					// Little end
					if (!QueueInsert(&TXBuf, distin[i])) {
						// TODO: Handle TXBuf overflow
					}
					// Big end
					if (!QueueInsert(&TXBuf, distin[i] >> 8)) {
						// TODO: Handle TXBuf overflow
					}
					IE2 |= UCA0TXIE;
				}
			}
			break;
		case ENCODERS_SPEED:
			// Direction
			if (!QueueInsert(&TXBuf, g_ldir | (g_rdir << 1))) {
				// TODO: Handle TXBuf overflow
			}

			// LEFT
			// Little end
			if (!QueueInsert(&TXBuf, diff1)) {
				// TODO: Handle TXBuf overflow
			}
			// Big end
			if (!QueueInsert(&TXBuf, diff1 >> 8)) {
				// TODO: Handle TXBuf overflow
			}

			// RIGHT
			if (!QueueInsert(&TXBuf, diff2)) {
				// TODO: Handle TXBuf overflow
			}
			// Big end
			if (!QueueInsert(&TXBuf, diff2 >> 8)) {
				// TODO: Handle TXBuf overflow
			}
			IE2 |= UCA0TXIE;
			break;
		case ENCODERS_POSITION:
			// LEFT
			if (!QueueInsert(&TXBuf, encodercount1)) {
				// TODO: Handle TXBuf overflow
			}
			if (!QueueInsert(&TXBuf, encodercount1 >> 8)) {
				// TODO: Handle TXBuf overflow
			}
			if (!QueueInsert(&TXBuf, encodercount1 >> 16)) {
				// TODO: Handle TXBuf overflow
			}
			if (!QueueInsert(&TXBuf, encodercount1 >> 24)) {
				// TODO: Handle TXBuf overflow
			}

			// RIGHT
			if (!QueueInsert(&TXBuf, encodercount2)) {
				// TODO: Handle TXBuf overflow
			}
			if (!QueueInsert(&TXBuf, encodercount2 >> 8)) {
				// TODO: Handle TXBuf overflow
			}
			if (!QueueInsert(&TXBuf, encodercount2 >> 16)) {
				// TODO: Handle TXBuf overflow
			}
			if (!QueueInsert(&TXBuf, encodercount2 >> 24)) {
				// TODO: Handle TXBuf overflow
			}
			IE2 |= UCA0TXIE;
			break;
		case MOTORS:
			num_bytes_in = 9;
			break;
		case SET_STATUS:
			num_bytes_in = 1;
			break;
		case GET_STATUS:
			// TODO: Add status
			if (!QueueInsert(&TXBuf, 0x55)) {
				// TODO: Handle TXBuf overflow
			}
			IE2 |= UCA0TXIE;
			break;
		default:
			// Not a valid address ignore it
			// TODO: Set comms error
			break;
		}
	}
	else if (num_bytes_in != 0) {
		switch(current_addr) {
		case MOTORS:
			if(num_bytes_in == 9) {
				motor_dir = value;
				motor_speed = 0;
			}
			else if(num_bytes_in < 9 && num_bytes_in >= 5) {
				uint8_t shift = 8 * (8 - num_bytes_in);
				uint32_t temp = *(uint32_t *)(&value);			// Reinterpret_cast
				motor_speed |= temp << shift;
				if(num_bytes_in == 5) {
					float test = *(float *)(motor_speed);		// Reinterpret_cast
					Set_PWM(test, (motor_dir & 0x1) << 1);
					motor_speed = 0;
				}
			}
			else if(num_bytes_in < 5) {
				uint8_t shift = 8 * (8 - num_bytes_in);
				uint32_t temp = *(uint32_t *)(&value);			// Reinterpret_cast
				motor_speed |= temp << shift;
				if(num_bytes_in == 1) {
					float test = *(float *)(motor_speed);		// Reinterpret_cast
					Set_PWM(test, (motor_dir & 0x2) | 0x1);
				}
			}
			num_bytes_in--;
			break;
		case SET_STATUS:
			// TODO: Add status system
			num_bytes_in--;
			break;
		default:
			break;	// Shouldn't ever get here
		}
	}
	// else we are writing data so just ignore the incoming data

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
