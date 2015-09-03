#include "catch.hpp"
#include "page_allocator.h"
#include "multiboot_mmap.h"

using namespace kernel;

namespace kernel
{
	const int g_MEM_RANGES_COUNT = 5;
	MemoryRange g_mem_ranges[g_MEM_RANGES_COUNT];
}

TEST_CASE( "Test allocate_frame", "[factorial]" )
{
	uintptr_t kernel_end_address = 0x37ab3fd1;

	unsigned x_first_frame_after_kernel = kernel_end_address / PAGE_SIZE;
	//=> 0x37ab3

	unsigned x_table_start = x_first_frame_after_kernel / 32 + 1;
	// => (0x37ab3 / 0x20) + 1  = 0x1BD6


	uintptr_t expected_allocated_frame_number = x_table_start * 32;
	pageframe_t expected_allocated_addr = (pageframe_t)(expected_allocated_frame_number * PAGE_SIZE);


	init_map((void*)kernel_end_address);

	pageframe_t actual_allocated_addr = allocate_frame();
	REQUIRE( actual_allocated_addr == expected_allocated_addr );


	for (int i = 0; i < 100; i++) {
		expected_allocated_addr = (pageframe_t)((uintptr_t)expected_allocated_addr + PAGE_SIZE);
		actual_allocated_addr = allocate_frame();
		REQUIRE( actual_allocated_addr == expected_allocated_addr );
	}
}

TEST_CASE( "Test is_page_aligned", "[what]" )
{
	REQUIRE(is_page_aligned(0x00000000) == true);
	REQUIRE(is_page_aligned(0x00000002) == false);
	REQUIRE(is_page_aligned(0x00000040) == false);
	REQUIRE(is_page_aligned(0x00000800) == false);

	REQUIRE(is_page_aligned(0x00001000) == true);
	REQUIRE(is_page_aligned(0x00001001) == false);

	REQUIRE(is_page_aligned(0x10000000) == true);
	REQUIRE(is_page_aligned(0xc0000004) == false);

	REQUIRE(is_page_aligned(0x45671000) == true);
	REQUIRE(is_page_aligned(0x45671800) == false);
}

TEST_CASE( "round_down_to_page" )
{
	REQUIRE(round_down_to_page(0x00000000) == 0x00000000);
	REQUIRE(round_down_to_page(0x00000007) == 0x00000000);
	REQUIRE(round_down_to_page(0x00000800) == 0x00000000);

	REQUIRE(round_down_to_page(0x00000fff) == 0x00000000);
	REQUIRE(round_down_to_page(0x00001000) == 0x00001000);
	REQUIRE(round_down_to_page(0x00001001) == 0x00001000);
	REQUIRE(round_down_to_page(0x00001fff) == 0x00001000);
	REQUIRE(round_down_to_page(0x00002000) == 0x00002000);
	REQUIRE(round_down_to_page(0x00002005) == 0x00002000);

	REQUIRE(round_down_to_page(0x11110000) == 0x11110000);
	REQUIRE(round_down_to_page(0x22222007) == 0x22222000);
	REQUIRE(round_down_to_page(0x33333800) == 0x33333000);

	REQUIRE(round_down_to_page(0x44444fff) == 0x44444000);
	REQUIRE(round_down_to_page(0x44441000) == 0x44441000);
	REQUIRE(round_down_to_page(0x44441001) == 0x44441000);
	REQUIRE(round_down_to_page(0xffffffff) == 0xfffff000);
	REQUIRE(round_down_to_page(0xfffff000) == 0xfffff000);
	REQUIRE(round_down_to_page(0xfffff005) == 0xfffff000);
}

TEST_CASE( "round_up_to_page" )
{
	REQUIRE(round_up_to_page(0x00000000) == 0x00000000);
	REQUIRE(round_up_to_page(0x00000007) == 0x00001000);
	REQUIRE(round_up_to_page(0x00000800) == 0x00001000);

	REQUIRE(round_up_to_page(0x00000fff) == 0x00001000);
	REQUIRE(round_up_to_page(0x00001000) == 0x00001000);
	REQUIRE(round_up_to_page(0x00001001) == 0x00002000);
	REQUIRE(round_up_to_page(0x00001fff) == 0x00002000);
	REQUIRE(round_up_to_page(0x00002000) == 0x00002000);
	REQUIRE(round_up_to_page(0x00002005) == 0x00003000);

	REQUIRE(round_up_to_page(0x11110000) == 0x11110000);
	REQUIRE(round_up_to_page(0x22222007) == 0x22223000);
	REQUIRE(round_up_to_page(0x33333800) == 0x33334000);

	REQUIRE(round_up_to_page(0x44444fff) == 0x44445000);
	REQUIRE(round_up_to_page(0x44441000) == 0x44441000);
	REQUIRE(round_up_to_page(0x44441001) == 0x44442000);
	REQUIRE(round_up_to_page(0xffffeabc) == 0xfffff000);
	REQUIRE(round_up_to_page(0xfffff000) == 0xfffff000);
	REQUIRE(round_up_to_page(0xfffff005) == 0x00000000);
	REQUIRE(round_up_to_page(0xffffffff) == 0x00000000);
}
