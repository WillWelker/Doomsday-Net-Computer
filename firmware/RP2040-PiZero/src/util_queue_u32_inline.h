#ifndef _UTIL_QUEUE_U32_INLINE_H
#define _UTIL_QUEUE_U32_INLINE_H

#include "pico/util/queue.h"

static inline uint32_t queue_peek_blocking_inline(queue_t *q) {
	uint32_t val;
	queue_peek_blocking(q, &val);
	return val;
}

static inline uint32_t queue_try_peek_inline(queue_t *q, uint32_t *val) {
	return queue_try_peek(q, val);
}

#endif
