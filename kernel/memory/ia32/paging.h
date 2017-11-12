#pragma once

#include <stdint.h>
#include "memory/types.h"

extern "C" void activate_page_directory(unsigned int*);

namespace kernel
{

static const uint32_t PDFLAG_PAGE_SIZE_SELECT = (1 << 7);
static const uint32_t PDFLAG_ACCESSED =         (1 << 5);
static const uint32_t PDFLAG_CACHE_DISABLED =   (1 << 4);
static const uint32_t PDFLAG_WRITE_THROUGH =    (1 << 3);
static const uint32_t PDFLAG_USER_PREVILEGES =  (1 << 2);
static const uint32_t PDFLAG_WRITABLE =         (1 << 1);
static const uint32_t PDFLAG_PRESENT =          (1 << 0);

void map_kernelspace_in_process(uint32_t *process_pgdir, uint32_t *kernel_pgdir);
void set_up_userspace_page_tables(uint32_t *userspace_pagedir, uint32_t virtual_start_address, uint32_t size);
uint32_t *initialize_page_directory(uint32_t flags);
uint32_t *create_kernel_pgdir();
uint32_t *create_process_pgdir(uint32_t virtual_start_address, uint32_t size, uint32_t *kernel_pagedir);

} // namespace kernel
