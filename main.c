#include <msp430.h> 
#include <stdio.h>
#include <msp430g2553.h>
#include <stdint.h>

#include "sonar_array.h"
#include "motors.h"
#include "encoder.h"
/*
 * main.c
 */
//Kevin claims pins 1.1,1.2,1.4,1.5 for SPI
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
    DCOCTL |= 7<<DCO0;
    BCSCTL1 |= 0x0F<<RSEL0 ;
   // InitSonarArray();
   // InitMotors();
    Init_Encoder();
    __enable_interrupt();			// Sets global ifg
    P1DIR = 0x01;
	while(1)
	{
		P1OUT ^=0x01; //Used to find clock freq
	}
	return 0;
}

