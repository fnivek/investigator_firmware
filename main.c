#include <msp430.h> 
#include <stdio.h>
#include <msp430g2553.h>
#include <stdint.h>

#include "sonar_array.h"
/*
 * main.c
 */

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer

    InitSonarArray();

    __enable_interrupt();			// Sets global ifg
	while(1)
	{
		SonarTick();
	}
	return 0;
}

