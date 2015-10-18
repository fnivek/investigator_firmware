#include <msp430.h> 
#include <stdio.h>
#include <msp430g2553.h>
#include <stdint.h>

#include "sonar_array.h"
#include "motors.h"
#include "encoder.h"
#include "comms.h"
//extern const uint8_t kNumSonars = 4;

extern float distin[];

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
    InitSonarArray();
    InitMotors();
    //InitComms();
    Init_Encoder();

    __enable_interrupt();			// Sets global ifg

   // P1DIR = 0x01;

    while(1)
	{
    	__delay_cycles(100);
    	SonarTick();
    	Set_PWM(distin[1]/80,0);
	}
	return 0;
}

