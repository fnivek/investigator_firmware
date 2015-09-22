#include <msp430.h> 
#include <stdio.h>
#include<msp430g2553.h>
#include<stdint.h>
/*
 * main.c
 * Interfaces with the Sonar sensor and a timer counter using pin interrupts
 */
uint16_t distcycles;
float distin;
uint16_t before;
uint16_t after;
int sonarcount;
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    P1DIR = 0x01;    //Set Port 1 Pin 0 as an output
    TACTL = 0x220; //Set Timercounter A To run in continuous mode
    P1IE = 0x02; // Enable interrupt Port1Pin1
    P1IES = 0x02; //P1.1 triggers an interrupt on a high to low transition
    __enable_interrupt();// sets global ifg

	while(1)
	{
		P1OUT = 0x01;
		__delay_cycles(500);
		P1OUT &= 0x00;
		before = TA0R;
		__delay_cycles(1000000);
	}
	return 0;
}
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{

	after = TA0R;					// Set the TA0R to a variable to make subtraction work
	distcycles = after-before;		//Find width of the pulse
	distin = distcycles * 0.00675;  //  uS/148 = inches
	P1IFG = 0x00;					//Clear the interrupt flag
}

