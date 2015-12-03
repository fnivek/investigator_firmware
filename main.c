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
extern uint16_t diff1;

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

	Set_PWM(0, 0);
	Set_PWM(0, 1);

    while(1)
	{

    	SonarTick();
    	if((lastcom>TA0R) && (lastcom - TA0R < 500))
		{
			P3OUT = 0x07;
			Set_PWM(0, 0);
			Set_PWM(0, 1);
		}
	}
	return 0;
}

