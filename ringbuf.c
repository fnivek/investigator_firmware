#include "ringbuf.h"

void RingbufPush(ringbuf* buf, uint8_t data) {
	buf->data[buf->position] = data;
	++buf->position;
	if(buf->position >= RINGBUF_SIZE) {
		buf->position = 0;
	}
}

void RingbufPop(ringbuf* buf, uint8_t* data){
	--buf->position;
	if(buf->position >= RINGBUF_SIZE) {
		buf->position = RINGBUF_SIZE - 1;
	}
	(*data) = buf->data[buf->position];
}

