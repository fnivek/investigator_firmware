/*
 * ringbuf.h
 *
 *  Created on: Nov 10, 2015
 *      Author: fnivek
 */

#ifndef RINGBUF_H_
#define RINGBUF_H_

#include <stdint.h>

#define RINGBUF_SIZE 25

typedef struct uint8_ringbuf {
	uint8_t position;
	uint8_t data[RINGBUF_SIZE];
} uint8_ringbuf;

void RingbufPushUint8(uint8_ringbuf* buf, uint8_t data);
void RingbufPopUint8(uint8_ringbuf* buf, uint8_t* data);

typedef struct float_ringbuf {
	uint8_t position;
	float data[RINGBUF_SIZE];
} float_ringbuf;

void RingbufPushFloat(float_ringbuf* buf, float data);
void RingbufPopFloat(float_ringbuf* buf, float* data);

#endif /* RINGBUF_H_ */
