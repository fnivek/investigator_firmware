#include <msp430.h> 
#include <stdio.h>
#include <msp430g2553.h>
#include <stdint.h>

#include "sonar_array.h"
#include "motors.h"
#include "encoder.h"
#include "comms.h"
//extern const uint8_t kNumSonars = 4;

extern uint16_t distin[];

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
    TA0CTL = MC_2 | ID_3 | TASSEL_2; 		// (0x02E0) set Timercounter A To run in continuous mode
    InitSonarArray();
    InitMotors();
    InitComms();
    Init_Encoder();

    __enable_interrupt();			// Sets global ifg
    IE2 |= UCA0RXIE | UCA0TXBUF;

   // P1DIR = 0x01;
    //Set_PWM(1000, 1);
   // Set_PWM(2000, 0);
    while(1)
	{
    	__delay_cycles(1000);

    	//Set_PWM(0, 1);
    	//Test_Motors();
    	SonarTick();
    	//Set_PWM(distin[1]/80,0);
    	//TestComms(0x55);
	}
	return 0;
}

