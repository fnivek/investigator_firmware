/*
 * sonar_array.c
 *
 *  Created on: Sep 27, 2015
 *      Authors: Kevin French
 *      		 Daniel Jensen
 */

// TODO:
//	* Remove delays in SonarTick function
//	* Convert global variables to a global struct

// Note:
//	* If interrupts are ever turned off sonar number will be out of sync

#include "sonar_array.h"

// Global variables
const uint8_t kNumSonars = 4;
const float kInchPerClk = 0.00675;

uint16_t distcycles;
float distin[kNumSonars];
uint8_t current_sonar = 0;
uint16_t before;
uint16_t after;
int sonarcount;

// Initilizes the sonar array on Port 1 pin 0-2
// Pin 0 Trigger
// Pin 1 Echo
// Pin 2 Reset
void InitSonarArray() {
	P1DIR = 0x05;    				// Set Port 1 Pin 0  & 2 as an output
	TA0CTL = 0x02E0; 					// Set Timercounter A To run in continuous mode
	P1IE = 0x02; 					// Enable interrupt Port1Pin1
	P1IES = 0x02; 					// P1.1 triggers an interrupt on a high to low transition

	P1OUT &= 0xFB; 					// Reset counter
	__delay_cycles(20); 			// Wait for the reset to kick in
	P1OUT |= 0x04;					// Set reset False (High)
}

// Updates the sonar state
void SonarTick() {

	__delay_cycles(1000);
	P1OUT &= 0xFE;
	before = TA0R;
	__delay_cycles(1000000);
}


// Interrupt for
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{

	after = TA0R;						// Set the TA0R to a variable to make subtraction work
	distcycles = after-before;			// Find width of the pulse
	distin[current_sonar] = distcycles * kInchPerClk;  // uS/148 = inches
	++current_sonar;
	if (current_sonar >= kNumSonars) {
		current_sonar = 0;
	}
	P1IFG = 0x00;						// Clear the interrupt flag
}
