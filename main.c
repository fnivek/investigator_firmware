#include <msp430.h> 
#include <stdio.h>
#include<msp430g2553.h>
#include<stdint.h>
/*
 * main.c
 * Interfaces with the Sonar sensor and a timer counter just fine, but needs to be rewritten to use pin interrupts
 * instead of input capture
 */
uint16_t distcycles;
float distin;
uint16_t before;
int sonarcount;
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    P1DIR = 0x01;    //Set Port 1 Pin 1 as an output
    P1SEL |= 0x02; //set P1.1 as CCInput
    P1SEL2 = 0x00;  //Samesies
    TACTL = 0x220; //Set Timercounter A To run in continuous mode
    TACCTL0 = 0xC910;
    __enable_interrupt();// sets global ifg

	while(1)
	{
		P1OUT = 0x01;
		__delay_cycles(500);
		P1OUT &= 0x00;
		__delay_cycles(1000000);
	}
	return 0;
}
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
	if(sonarcount%2 == 0)
	{
		before = TA0CCR0;  //rising edge
	}
	else
	{
		distcycles = TA0CCR0 - before;  //automatically calculates the difference due to unsigned arith. no sign problems
		distin = distcycles * 0.00676;  //Divide by 148 to conver microseconds to inches with this part
	}
	sonarcount++;

	/*
	 * To get this working consider the following initializations:
	 *
	 * uint16_t distcycles;
		float distin;
		uint16_t before;
	 *  P1DIR = 0x01;    //Set Port 1 Pin 1 as an output
    	P1SEL |= 0x02; //set P1.1 as CCInput
    	P1SEL2 = 0x00;  //Samesies
    	TACTL = 0x220; //Set Timercounter A To run in continuous mode
    	TACCTL0 = 0xC910;
    	__enable_interrupt();// sets global ifg
	 */
}

