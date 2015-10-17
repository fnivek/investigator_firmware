#include <msp430.h> 
#include <stdio.h>
#include <msp430g2553.h>
#include <stdint.h>

#include "sonar_array.h"
#include "motors.h"
#include "encoder.h"
#include "comms.h"

/*
 * main.c
 */
//Kevin claims pins 1.1,1.2,1.4,1.5 for SPI
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer

    // Set up master clk
    DCOCTL |= DCO2 | DCO1 | DCO0;
    BCSCTL1 |= RSEL3 | RSEL2 | RSEL1 | RSEL0;

    // Init systems
    //InitSonarArray();
    //InitMotors();
    //InitComms();
    //Init_Encoder();

    __enable_interrupt();			// Sets global ifg

    P1DIR = 0x01;

    while(1)
	{
		//SonarTick();
		//MotorTick();
    	P1OUT ^=0x01; //Used to find clock freq
	}
	return 0;
}

