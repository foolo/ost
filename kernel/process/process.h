#pragma once
#include <stdint.h>

struct process_info {
	unsigned flags;
	void *current_break;
	uint32_t *pgdir;
};

unsigned create_new_process_id();
void set_current_process(unsigned pid);
unsigned get_current_process();
struct process_info *get_current_process_info();
struct process_info *get_process_info(unsigned pid);
