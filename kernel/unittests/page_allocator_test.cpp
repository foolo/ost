#include "catch.hpp"

#include "paging/page_allocator.h"


TEST_CASE( "Test allocate_frame", "[factorial]" )
{
	intptr_t kernel_end_address = 0x37ab3fd1;


	unsigned x_first_frame_after_kernel = kernel_end_address / PAGE_SIZE;
	//=> 0x37ab3

	unsigned x_table_start = x_first_frame_after_kernel / 32 + 1;
	// => (0x37ab3 / 0x20) + 1  = 0x1BD6


	intptr_t expected_allocated_frame_number = x_table_start * 32;
	pageframe_t expected_allocated_addr = (pageframe_t)(expected_allocated_frame_number * PAGE_SIZE);

	init_map((void*)kernel_end_address);
	pageframe_t actual_allocated_addr = allocate_frame();

	REQUIRE( actual_allocated_addr == expected_allocated_addr );
}
