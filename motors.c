/*
 * motors.c
 *
 *  Created on: Sep 27, 2015
 *      Authors: Kevin French
 *      		 Daniel Jensen
 */

#include "motors.h"
#define FRAMELENGTH 1000

// Globals
float g_pwml;//These two variables should be written to
float g_pwmr;
float pwml_active;
float pwmr_active;
uint8_t g_ldir; //global variable for left direction
uint8_t g_rdir;
uint16_t timervalue;
uint16_t timerintcode;
uint8_t motorsel; //part of the flag variable in SET_PWM controls which motor is being referenced
uint8_t motordir; //part of flag variable in SET_PWM 1 is forward, 0 is reverse


// Initilize the motors on port 2 pins 2.0, 2.2, 2.3, 2.5
//									leftIN1,rightIN1,rightIN2,leftIN2
void InitMotors() {
	P2DIR |= 0x2D; // Set 2.0,2.2,2.3,& 2.5 as outputs
	//TACTL = 0x0220; //Continuous,divide by 1, no OVF (*IN INIT SONAR*)
	TA0CCTL0 = 0x0810;
	TA0CCTL1 = 0x0810;	//No capture,synchronouscap(unnecesary),comparemode
	TA0CCTL2 = 0x0810;	//OUTput the outputbit,enable int,


}

void MotorTick() {
	// Place holder
}


#pragma vector=TIMER0_A0_VECTOR
__interrupt void PWMFRAMETICK(void)
{
	//This interrupt is the framing interrupt, set at the PWM frequency
	//Depending on desired direction we set the pwm pin high and the enable pin low

	pwml_active = g_pwml; //Sample the global pwm values at the new "frame"
	pwmr_active = g_pwmr;
	ldir_active = g_ldir;
	rdir_active = g_rdir;
	timervalue = TAR;

	if(pwml_active != 0 && ldir_active == 1)
	{
		P2OUT |= 0x01; //PWML IN1 is P2.0 - (We apply PWM to it)
		P2OUT &= 0xDF; //PWML IN2 is P2.5 -(We set it low in forward mode)
		TA0CCR1 = timervalue + pwml_active*FRAMELENGTH;  //prepare for an interrupt pwml_active*1000 cycles in the future
	}
	else if(pwml_active != 0 && ldir_active == 0)
	{
		P2OUT |= 0x20; //PWML IN2 is P2.5 - (We apply PWM to it in reverse mode)
		P2OUT &= 0xFE; //PWML IN1 is P2.0 -(We set it low in reverse mode)
		TA0CCR1 = timervalue + pwml_active*FRAMELENGTH;  //prepare for an interrupt pwml_active*1000 cycles in the future
	}
	if(pwmr_active != 0 && rdir_active == 1)
	{
		P2OUT |= 0x04; //PWMR IN1 is P 2.2
		P2OUT &= 0xF7; //PWMR IN2 is P2.3 (low in forward mode)
		TA0CCR2 = timervalue + pwmr_active*FRAMELENGTH; //prepare for an interrupt pwmr_active*1000 cycles in the future
	}
	if(pwmr_active != 0 && rdir_active == 0)
		{
		P2OUT |= 0x08; //PWMR IN2 is P2.3 (gets PWM in reverse mode)
		P2OUT &= 0xFB; //PWR  IN1 is P2.2 (set low in reverse)
		TA0CCR2 = timervalue + pwmr_active*FRAMELENGTH; //prepare for an interrupt pwmr_active*1000 cycles in the future
		}

	TA0CCR0 = timervalue + FRAMELENGTH; //sets the frame to have 1000 cycle length
	TA0CCTL0 = 0x0810;
	//TA0CTL &= 0xFFFE;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void PWMCC(void)
{
	//This interrupt is where we switch off the PWM signal
	timerintcode = TA0IV;
	if((timerintcode & 0x02) == 0x02 && ldir_active == 1)//TACCR1 controls the "left"
	{
		P2OUT &= 0xFE;//Left forward means PWM is bit 2.0 which we switch off here
	}
	if((timerintcode & 0x02) == 0x02 && ldir_active == 0)
	{
		P2OUT &= 0xDF;   //Left reverse means PWM is IN2 which was mapped to 2.1
	}
	if((timerintcode & 0x04) == 0x04 && rdir_active == 1)//TACCR2 controls the "right"
	{
		P2OUT &= 0xFB;//Right forward -> switch off IN1 -> P2.2
	}
	if((timerintcode & 0x04) == 0x04 && rdir_active == 0)
	{
		P2OUT &= 0xF7;  //Right reverse -> switch of IN2 -> P2.3
	}
}
void Set_PWM(float pwm, uint8_t flag )
{
	//LSB of flag determines which motor
	// Bit 1 of flag will control direction
	 motorsel = flag & 0x01;
	 motordir = (flag & 0x02)>>1;
	if(pwm >= 0 && pwm<= 1.0 && motorsel==0)
	{
		g_pwml = pwm;
		g_ldir = motordir;
	}
	else if(pwm >= 0 && pwm<= 1.0 && motorsel!=0)
	{
		g_pwmr = pwm;
		g_rdir = motordir;
	}
}
void Test_Motors(void)
{
	//put this code in a while(1) loop to ramp up motor speed slowly for test purposes
	float counter = 0.1;
	Set_PWM(counter,2);
		Set_PWM(counter,3); //run both "motors" forward direction
		__delay_cycles(9000000);
		if(counter<0.99)
		{
			counter = counter +0.01;

		}
		else
		{
			return;
		}
}


