#include <stdio.h>
#include <stdbool.h>
#include "input_queue.h"

uint8_t queue[256];
uint8_t head = 0; // points to next item
uint8_t tail = 0;

uint8_t get_queue_size() {
	return head - tail;
}

uint8_t get_size_left() {
	return tail - head - 1;
}

bool is_empty() {
	return head == tail;
}

void input_queue_push(uint8_t *data, uint8_t length) {
	if (length > get_size_left()) {
		// handle error
		printf("keyboard buffer full\n");
		return;
	}
	for (uint8_t i = 0; i < length; i++) {
		queue[head++] = data[i];
		printf("data: %x\n", data[i]);
	}
}

uint8_t input_queue_pop() {
	while (is_empty()) {
		printf("keyboard buffer is empty, halt\n");
		asm ("hlt");
	}
	printf("input_queue_pop %x\n", queue[tail]);
	return queue[tail++];
}
