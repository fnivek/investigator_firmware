/*
 * motors.h
 *
 *  Created on: Sep 27, 2015
 *      Authors: Kevin French
 *      		 Daniel Jensen
 */

#ifndef MOTORS_H_
#define MOTORS_H_

#include <msp430.h>
#include <stdio.h>
#include <msp430g2553.h>
#include <stdint.h>

#define NUM_MOTORS 2

typedef struct motor {
	uint8_t in1;
	uint8_t in2;
	uint16_t pwm;
	uint16_t next_pwm;
	uint8_t dir;		// Bit 0 is current dur and bit one is next dir
	uint8_t int_code;	// Code for the intterrupt for CCRx
	volatile unsigned int* ccr_address;
	volatile unsigned int* cctl_address;
} motor;

enum motor_flags {
	MOTOR_BM = 1,
	DIRECTION_BM = 2,
	RIGHT_MOTOR = 1,
	LEFT_MOTOR = 0,
	FORWARD = 0,
	BACKWARDS = 2
};

// Function forward declerations
void InitMotors();
void MotorTick();
void Set_PWM(float pwm, uint8_t bit);
void Test_Motors(void);

// Global vars
uint8_t ldir_active; //the current direction of the motor
uint8_t	rdir_active;

#endif /* MOTORS_H_ */
