#include "ringbuf.h"

void RingbufPushUint8(uint8_ringbuf* buf, uint8_t data) {
	buf->data[buf->position] = data;
	++buf->position;
	if(buf->position >= RINGBUF_SIZE) {
		buf->position = 0;
	}
}

void RingbufPopUint8(uint8_ringbuf* buf, uint8_t* data) {
	--buf->position;
	if(buf->position >= RINGBUF_SIZE) {
		buf->position = RINGBUF_SIZE - 1;
	}
	(*data) = buf->data[buf->position];
}

void RingbufPushFloat(float_ringbuf* buf, float data) {
	buf->data[buf->position] = data;
	++buf->position;
	if(buf->position >= RINGBUF_SIZE) {
		buf->position = 0;
	}
}

void RingbufPopFloat(float_ringbuf* buf, float* data) {
	--buf->position;
	if(buf->position >= RINGBUF_SIZE) {
		buf->position = RINGBUF_SIZE - 1;
	}
	(*data) = buf->data[buf->position];
}
