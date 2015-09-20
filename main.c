#include <msp430.h> 
#include <stdio.h>
#include<msp430g2553.h>
#include<stdint.h>
/*
 * main.c
 */
uint16_t distcycles;
float distin;
uint16_t before;
int sonarcount = 0;
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
		before = TACCR0;  //rising edge
	}
	else
	{
		distcycles = TACCR0 - before;  //falling edge
		distin = distcycles * 0.00676;
	}
	sonarcount++;
}

/*P1OUT = 0x01;

		__delay_cycles(500);
		P1OUT &= 0x00;
		while(P1IN && 0x02 == 0)
		{
			__delay_cycles(1);  ///I should configure a pin interrupt in the int set a vaariable to the change and get out/disable interrupt
		}
		before = TA0R;
		while(P1IN && 0x02 == 0x02)
			{
			__delay_cycles(1);  //The dummy program gets stuck here if the ultrasonic sensor doesn't respond the first time
			}
		distcycles = TA0R - before;
		distin = distcycles*.000422; //fclock = 16mhz and microseconds/148 -> inches
		__delay_cycles(1000000);*/
