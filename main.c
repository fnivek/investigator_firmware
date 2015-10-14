#include <msp430.h> 
#include <stdio.h>
#include <msp430g2553.h>
#include <stdint.h>

#include "sonar_array.h"
#include "motors.h"
#include "comms.h"

/*
 * main.c
 */

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer

    // Set up master clk
    DCOCTL |= DCO2 | DCO1 | DCO0;
    BCSCTL1 |= RSEL3 | RSEL2 | RSEL1 | RSEL0;

    // Init systems
    //InitSonarArray();
    //InitMotors();
    InitComms();

    __enable_interrupt();			// Sets global ifg
	uint8_t test = 0;

    while(1)
	{
		//SonarTick();
		//MotorTick();
		if (TestComms(test)) {
			++ test;
		}
	}
	return 0;
}

