/*
 * motors.c
 *
 *  Created on: Sep 27, 2015
 *      Authors: Kevin French
 *      		 Daniel Jensen
 */

#include "motors.h"

// array of motors
motor motors[] = {
		{BIT0, BIT5, 0, 0, 0, TA0IV_TACCR1, &TA0CCR1, &TA0CCTL1},
		{BIT2, BIT3, 0, 0, 0, TA0IV_TACCR2, &TA0CCR2, &TA0CCTL2}};


// Framing
#define FRAMELENGTH 1000
#define MIN_NUM_CLKS 50

// Initilize the motors on port 2 pins 2.0, 2.2, 2.3, 2.5
//									leftIN1,rightIN1,rightIN2,leftIN2
void InitMotors() {
	// Set 2.0,2.2,2.3,& 2.5 as outputs
	uint8_t i;
	for(i = NUM_MOTORS; i != 0; --i) {
		motor* m = &motors[i - 1];
		(*m->cctl_address) = SCS;	// No capture,synchronouscap(unnecesary),comparemode OUTput the outputbit,enable int,
		P2DIR |= m->in1;
		P2DIR |= m->in2;
	}
	//TACTL = 0x0220; 			// Continuous,divide by 1, no OVF (*IN INIT SONAR*)

	// Debug
	P2DIR |= 0x80;
	// End debug

	TA0CCTL0 = SCS | CCIE; 		//0x0810;
}

void MotorTick() {
	// Place holder
}

// This interrupt is the framing interrupt, set at the PWM frequency
// Depending on desired direction we set the pwm pin high and the enable pin low
#pragma vector=TIMER0_A0_VECTOR
__interrupt void PWMFRAMETICK(void)
{
	// Loop through motors
	uint8_t i;
	for(i = NUM_MOTORS; i != 0; --i) {
		motor* m = &motors[i - 1];

		// Move in the new pwm and dir values in
		m->pwm = m->next_pwm;
		m->dir &= 0xFE;
		m->dir |= (m->dir & DIRECTION_BM) >> 1;

		// Check if zero
		if(m->pwm <= MIN_NUM_CLKS) {
			P2OUT &= ~(m->in1 | m->in2);
			continue;
		}

		// Set the pwm pins
		if(m->dir & 0x1) {
			P2OUT |= m->in1;
			P2OUT &= ~m->in2;
		}
		else {
			P2OUT |= m->in2;
			P2OUT &= ~m->in1;
		}

		// Check edge cases
		if(m->pwm == FRAMELENGTH) {
			continue;
		}

		// Set the next capture compare value
		(*m->ccr_address) = TA0R + m->pwm;

		// Enable the interrupt
		//(*m->cctl_address) &= 0xFE;
		(*m->cctl_address) |= CCIE;

		// Trigger a manual interrupt if we enabled the interrupt to late
		// Since there is unsigned wrap around special considerations must be taken
		/* for edge cases
		if(TA0R < TA0CCR0) {
			// Wrap around detected
			if(TA0R  < (*m->ccr_address)) {
				// Interrupt was missed
				P2OUT &= ~(m->in1 | m->in2);
				(*m->cctl_address) &= ~CCIE;
			}
		}
		else {
			// No wrap around
			if (TA0R > (*m->ccr_address)) {
				// Interrupt was missed
				P2OUT &= ~(m->in1 | m->in2);
				(*m->cctl_address) &= ~CCIE;
			}
		}*/
	}

	// Set next interrupt time
	TA0CCR0 += FRAMELENGTH;

	// Ensure interrupt enable didn't get reset
	TA0CCTL0 |= CCIE;

	//TA0CTL &= 0xFFFE;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void PWMCC(void)
{
	// This interrupt is where we switch off the PWM signal
	// Loop through motors
	uint8_t i;
	for(i = NUM_MOTORS; i != 0; --i) {
		motor* m = &motors[i - 1];
		if((m->int_code & TA0IV) == 0) {
			continue;
		}

		// Ground both pins and disable intterupts
		P2OUT &= ~(m->in1 | m->in2);
		(*m->cctl_address) &= ~CCIE;
	}
}

void Set_PWM(float pwm, uint8_t flag )
{
	if(pwm > 1.0 | pwm < 0.0)
		return;

	// LSB of flag determines which motor
	// Bit 1 of flag will control direction
	motor* m = &motors[flag & MOTOR_BM];
	m->next_pwm = pwm * FRAMELENGTH;
	m->dir &= ~DIRECTION_BM;
	m->dir |= (flag & DIRECTION_BM);
}

void Test_Motors(void)
{
	float step = 0.001;
	float pwm = 0.5;
	uint8_t dir = DIRECTION_BM;

	while(1) {
		// Set motors
		Set_PWM(pwm, dir | LEFT_MOTOR);
		Set_PWM(pwm, dir | RIGHT_MOTOR);

		// Step
		pwm += step;
		if(pwm > 1.0) {
			step *= -1;
			pwm = 1.0;
		}
		else if(pwm < 0.0) {
			// Change dir
			dir ^= DIRECTION_BM;
			step *= -1;
			pwm = 0.0;
		}

		// Wait
		__delay_cycles(900000);
	}
}


