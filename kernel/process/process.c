#include <stdio.h>
#include "process.h"
#include "util/assert.h"

unsigned current_process_id;

struct process_info process_info_table[10] = {{0}};
unsigned process_info_table_size = 10;

unsigned create_new_process_id() {
	for (unsigned i = 0; i < process_info_table_size; i++){
		if (process_info_table[i].flags == 0) {
			process_info_table[i].flags |= 1;
			return i;
		}
	}
	halt("process_info_table full\n");
}

void set_current_process(unsigned pid) {
	current_process_id = pid;
}

unsigned get_current_process() {
	return current_process_id;
}

struct process_info *get_current_process_info() {
	return &process_info_table[current_process_id];
}

struct process_info *get_process_info(unsigned pid) {
	return &process_info_table[pid];
}
