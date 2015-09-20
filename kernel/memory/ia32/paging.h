#pragma once

#include <stdint.h>

namespace kernel
{

static const uint32_t PDFLAG_PAGE_SIZE_SELECT = (1 << 7);
static const uint32_t PDFLAG_ACCESSED =         (1 << 5);
static const uint32_t PDFLAG_CACHE_DISABLED =   (1 << 4);
static const uint32_t PDFLAG_WRITE_THROUGH =    (1 << 3);
static const uint32_t PDFLAG_USER_PREVILEGES =  (1 << 2);
static const uint32_t PDFLAG_WRITABLE =         (1 << 1);
static const uint32_t PDFLAG_PRESENT =          (1 << 0);

void set_up_paging();

} // namespace kernel
