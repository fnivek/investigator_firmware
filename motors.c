/*
 * motors.c
 *
 *  Created on: Sep 27, 2015
 *      Authors: Kevin French
 *      		 Daniel Jensen
 */

#include "motors.h"
#define FRAMELENGTH 2000
#define MIN_PULSE_WIDTH 5
#define MAX_PULSE_WIDTH (FRAMELENGTH - 25)

// Globals
uint16_t pulse_width_l;//These two variables should be written to
uint16_t pulse_width_r;
uint16_t pulse_width_l_active;
uint16_t pulse_width_r_active;
uint8_t g_dir_l; //global variable for left direction
uint8_t g_dir_r;
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
	uint16_t timervalue = TAR;
	pulse_width_l_active = pulse_width_l; //Sample the global pwm values at the new "frame"
	pulse_width_r_active = pulse_width_r;
	// Set capture channels
	TA0CCR1 = timervalue + pulse_width_l_active;
	TA0CCR2 = timervalue + pulse_width_r_active;

	// Update dirs
	ldir_active = g_dir_l;
	rdir_active = g_dir_r;

	if(pulse_width_l_active < MIN_PULSE_WIDTH) //pick a cutoff speed to set to zero
	{
		P2OUT &= 0xFE; // PWML IN1 is P2.0 - just set it low
		P2OUT &= 0xDF; // Set PWML IN2 low for "forward"
		return; //don't bother checking the other cases
	}
	else if(pulse_width_l_active > MAX_PULSE_WIDTH)
	{
		TA0CCTL1 &= ~CCIE;
	}
	else
	{
		TA0CCTL1 |= CCIE;
	}
	if(pulse_width_r_active < MIN_PULSE_WIDTH)
	{
		P2OUT &= 0xFB; //PMWR IN1 is P2.2 - just set it low
		P2OUT &= 0xF7; //Set PWMR IN2 low (low in forward mode)
		return; //don't bother on the other cases
	}
	else if(pulse_width_r_active > MAX_PULSE_WIDTH)
	{
		TA0CCTL2 &= ~CCIE;
	}
	else
	{
		TA0CCTL2 |= CCIE;
	}

	// Left
	if(ldir_active == 1)
	{
		P2OUT |= 0x01; //PWML IN1 is P2.0 - (We apply PWM to it)
		P2OUT &= 0xDF; //PWML IN2 is P2.5 -(We set it low in forward mode)
	}
	else
	{
		P2OUT |= 0x20; //PWML IN2 is P2.5 - (We apply PWM to it in reverse mode)
		P2OUT &= 0xFE; //PWML IN1 is P2.0 -(We set it low in reverse mode)
	}

	// Right
	if(rdir_active == 1)
	{
		P2OUT |= 0x04; //PWMR IN1 is P 2.2
		P2OUT &= 0xF7; //PWMR IN2 is P2.3 (low in forward mode)
	}
	else
	{
		P2OUT |= 0x08; //PWMR IN2 is P2.3 (gets PWM in reverse mode)
		P2OUT &= 0xFB; //PWR  IN1 is P2.2 (set low in reverse)
	}

	TA0CCR0 += FRAMELENGTH; //sets the frame to have FRAMELENGTH cycle length
	TA0CCTL0 |= CCIE;
	//TA0CTL &= 0xFFFE;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void PWMCC(void)
{
	//This interrupt is where we switch off the PWM signal
	uint16_t timerintcode = TA0IV;
	if((timerintcode & 0x02) == 0x02)//TACCR1 controls the "left"
	{
		P2OUT &= 0xDE;//Left forward means PWM is bit 2.0 which we switch off here
	}

	if((timerintcode & 0x04) == 0x04)//TACCR2 controls the "right"
	{
		P2OUT &= 0xF3;//Right forward -> switch off IN1 -> P2.2
	}
}

void Set_PWM(uint16_t pulse_width, uint8_t flag )
{
	//LSB of flag determines which motor
	// Bit 1 of flag will control direction
	motorsel = flag & 0x01;
	motordir = (flag & 0x02)>>1;

	// Make sure a valid value is being sent
	if(pulse_width > FRAMELENGTH) return;

	if(motorsel == 0) {
		pulse_width_l = pulse_width;
		g_dir_l = motordir;
	}
	else {
		pulse_width_r = pulse_width;
		g_dir_r = motordir;
	}
}

void Test_Motors(void)
{
	int8_t step = 1;
	uint16_t counter = 0;
	while(1)
	{
		Set_PWM(counter, 2);
		Set_PWM(counter, 3); //run both "motors" Forward direction
		__delay_cycles(90000);
		counter += step;
		if(counter >= FRAMELENGTH) {
			step *= -1;
			counter = FRAMELENGTH;
		}
		else if(counter <= 0){
			step *= -1;
			counter = 0;
		}
	}
}


