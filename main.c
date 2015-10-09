#include <msp430.h> 
#include <stdio.h>
#include <msp430g2553.h>
#include <stdint.h>

#include "sonar_array.h"
#include "motors.h"

/*
 * main.c
 */

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
    DCOCTL |= 7<<DCO0;
    BCSCTL1 |= 0x0F<<RSEL0 ;
    InitSonarArray();
    InitMotors();
    float counter = 0.1;
    __enable_interrupt();			// Sets global ifg
	while(1)
	{

	Set_PWM(counter,2);
	Set_PWM(counter,3); //run both "motors" at half speed
	__delay_cycles(9000000);
	if(counter<0.99)
	{
		counter = counter +0.01;
	}
	else
	{
		counter = 0.1;
	}
	}
	return 0;
}

