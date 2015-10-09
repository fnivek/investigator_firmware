/*
 * motors.h
 *
 *  Created on: Sep 27, 2015
 *      Author: Kevin French
 */

#ifndef MOTORS_H_
#define MOTORS_H_

#include <msp430.h>
#include <stdio.h>
#include <msp430g2553.h>
#include <stdint.h>

void InitMotors();
void MotorTick();
void Set_PWM(float pwm, uint8_t bit);

#endif /* MOTORS_H_ */
