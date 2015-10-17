/*
 * comms.h
 *
 *  Created on: Oct 7, 2015
 *      Author: fnivek
 */

#ifndef COMMS_H_
#define COMMS_H_

#include <msp430g2553.h>
#include <stdint.h>

void InitComms(void);

uint8_t TestComms(uint8_t thing);

#endif /* COMMS_H_ */
