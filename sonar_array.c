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
const float kInchPerClk = 0.0016875;

uint16_t distin[kNumSonars];
uint8_t current_sonar = 0;
uint16_t before;
uint8_t starttrig = 1;
uint8_t waiting = 0;
uint16_t toggletime = 0;

// Initilizes the sonar array on Port 1 pin 0,3,6
// Pin 0 Trigger
// Pin 3 Echo
// Pin 6 Reset
void InitSonarArray() {
	P1DIR |= 0x41;    				// Set Port 1 Pin 0 & 6 as an output
	P1DIR &= 0xF7;					// Set P1.3 as an input
	TA0CTL = 0x02E0; 				// Set Timercounter A To run in continuous mode
	P1IE = 0x08; 					// Enable interrupt Port1Pin3 for the echo
	P1IES = 0x08; 					// P1.3 triggers an interrupt on a high to low transition

	P1OUT &= 0xBF; 					// Reset counter
	__delay_cycles(20); 			// Wait for the reset to kick in
	P1OUT |= 0x40;					// Set reset False (High)
}

// Updates the sonar state
void SonarTick() {
/*
	__delay_cycles(1000);
	P1OUT &= 0xFE;
	before = TA0R;
	__delay_cycles(1000000);
	*/
	if(starttrig == 1 && waiting == 0)
	{
		P1OUT |= 0x01; //set the trigger high
		waiting = 1;
		toggletime = TA0R + 20000;
	}
	if( (starttrig == 1) && (waiting == 1) && (toggletime - TA0R > 0) &&(toggletime - TA0R < 5000))
	{
		P1OUT &= 0xFE; //set the trigger low
		starttrig = 0; //reset the trigger flag
		before = TA0R;
		waiting = 0;   //indicates we are not waiting with the trigger high
	}

}


// Interrupt for
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{

	uint16_t after = TA0R;						// Set the TA0R to a variable to make subtraction work
	distin[current_sonar] = after - before;		// Get number of clocks

	++current_sonar;							// Increment sonars
	if (current_sonar >= kNumSonars) {
		current_sonar = 0;
	}

	P1IFG = 0x00;								// Clear the interrupt flag
	starttrig = 1; 								//Set the flag for the trigger

}
