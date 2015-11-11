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

typedef struct ringbuf {
	uint8_t position;
	uint8_t data[RINGBUF_SIZE];
} ringbuf;

void RingbufPush(ringbuf* buf, uint8_t data);
void RingbufPop(ringbuf* buf, uint8_t* data);

#endif /* RINGBUF_H_ */
