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

// Function forward declerations
void InitMotors();
void MotorTick();
void Set_PWM(float pwm, uint8_t bit);
void Test_Motors(void);

// Global vars
uint8_t ldir_active; //the current direction of the motor
uint8_t	rdir_active;

#endif /* MOTORS_H_ */
