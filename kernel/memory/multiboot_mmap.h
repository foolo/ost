#pragma once

#include "multiboot.h"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

struct MemoryRange;

void multiboot_mmap(unsigned long magic, multiboot_info_t *mbi);
