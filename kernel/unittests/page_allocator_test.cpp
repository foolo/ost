#include "catch.hpp"
#include "page_allocator.h"
#include "multiboot_mmap.h"
#include "memory/MemoryRange.h"

using namespace kernel;

TEST_CASE( "init_map" )
{
	reset_page_allocator();
	register_memory_range(MemoryRange(0x00000000, 0x0000fc00));
	register_memory_range(MemoryRange(0x00100000, 0x00108000));
	register_memory_range(MemoryRange(0xB0133111, 0xB0137222));
	init_map();

	REQUIRE( allocate_frame() == (pageframe_t) 0x00001000);
	for (int i = 2; i < 0xf; i++)
	{
		REQUIRE( allocate_frame() == (pageframe_t) (i * PAGE_SIZE));
	}
	REQUIRE( allocate_frame() == (pageframe_t) 0x00100000);
	REQUIRE( allocate_frame() == (pageframe_t) 0x00101000);
	for (int i = 2; i < 8; i++)
	{
		REQUIRE( allocate_frame() == (pageframe_t) (0x00100000 + i * PAGE_SIZE));
	}
	REQUIRE( allocate_frame() == (pageframe_t) 0xB0134000);
	REQUIRE( allocate_frame() == (pageframe_t) 0xB0135000);
	REQUIRE( allocate_frame() == (pageframe_t) 0xB0136000);
	REQUIRE( allocate_frame() == (pageframe_t) 0);
}

TEST_CASE( "allocate_kernel" )
{
	reset_page_allocator();
	register_memory_range(MemoryRange(0x00100000, 0x00108000));
	init_map();
	allocate_kernel((void*)0x00100000, (void*)0x00102000);
	REQUIRE( allocate_frame() == (pageframe_t) 0x00103000);

	reset_page_allocator();
	register_memory_range(MemoryRange(0x00100000, 0x00108000));
	init_map();
	allocate_kernel((void*)0x00105111, (void*)0x00109111);
	REQUIRE( allocate_frame() == (pageframe_t) 0x00100000);
	REQUIRE( allocate_frame() == (pageframe_t) 0x00101000);
	REQUIRE( allocate_frame() == (pageframe_t) 0x00102000);
	REQUIRE( allocate_frame() == (pageframe_t) 0x00103000);
	REQUIRE( allocate_frame() == (pageframe_t) 0x00104000);
	REQUIRE( allocate_frame() == (pageframe_t) 0);

	reset_page_allocator();
	register_memory_range(MemoryRange(0x00100000, 0x00108000));
	init_map();
	allocate_kernel((void*)0x000ff111, (void*)0x00104111);
	REQUIRE( allocate_frame() == (pageframe_t) 0x00105000);
	REQUIRE( allocate_frame() == (pageframe_t) 0x00106000);
	REQUIRE( allocate_frame() == (pageframe_t) 0x00107000);
	REQUIRE( allocate_frame() == (pageframe_t) 0);
}

TEST_CASE("address_to_table_index")
{
	int page_bits = 12;
	int map_bit_bits = 5;
	int shift = page_bits + map_bit_bits;
	addr_t addr;
	addr = 0x00000000;
	REQUIRE(address_to_table_index(addr) == (addr >> shift));
	addr = 0x0000ef12;
	REQUIRE(address_to_table_index(addr) == (addr >> shift));
	addr = 0xabcdef12;
	REQUIRE(address_to_table_index(addr) == (addr >> shift));
	addr = 0xffffffff;
	REQUIRE(address_to_table_index(addr) == (addr >> shift));
}

TEST_CASE("address_to_bit_index")
{
	REQUIRE(address_to_bit_index(0xffffffff) == 31);
	REQUIRE(address_to_bit_index(0xfffe0fff) == 0);
	REQUIRE(address_to_bit_index(0x00000000) == 0);
	REQUIRE(address_to_bit_index(0x0001f000) == 31);
	REQUIRE(address_to_bit_index(0xfffe5fff) == 5);
	REQUIRE(address_to_bit_index(0x00015000) == 0x15);
}

TEST_CASE("jump_to_next_map")
{
	// one map is 32 (0x20) pages
	// one page is 0x1000 bytes
	// one map is 0x20*0x1000 = 0x20000 bytes
	addr_t MAP_SIZE = PAGE_SIZE * 32;
	addr_t MAP_SIZE_MASK = PAGE_SIZE * 32 - 1;
	REQUIRE(MAP_SIZE == 0x20000);
	REQUIRE(MAP_SIZE_MASK == 0x1ffff);

	REQUIRE(jump_to_next_map(0x00000000) == MAP_SIZE);
	REQUIRE(jump_to_next_map(0x00014bf2) == MAP_SIZE);
	REQUIRE(jump_to_next_map(0x0001ffff) == MAP_SIZE);
	REQUIRE(jump_to_next_map(0x00020000) == 2*MAP_SIZE);
	REQUIRE(jump_to_next_map(0x00020ddd) == 2*MAP_SIZE);
	REQUIRE(jump_to_next_map(0x00520000) == 0x54/2*MAP_SIZE);
	REQUIRE(jump_to_next_map(0x0052eeee) == 0x54/2*MAP_SIZE);
}

TEST_CASE("register_memory_range")
{
	reset_page_allocator();
	REQUIRE(register_memory_range(MemoryRange(0,0x0000Afff)) == true);
	REQUIRE(register_memory_range(MemoryRange(0,0)) == true);
	REQUIRE(register_memory_range(MemoryRange(0,0)) == true);
	REQUIRE(register_memory_range(MemoryRange(0,0)) == true);
	REQUIRE(register_memory_range(MemoryRange(0,0)) == true);
	REQUIRE(register_memory_range(MemoryRange(0,0)) == true);
	REQUIRE(register_memory_range(MemoryRange(0,0)) == true);
	REQUIRE(register_memory_range(MemoryRange(0,0x0000Afff)) == true);
	REQUIRE(register_memory_range(MemoryRange(0,0x0000Afff)) == true);
	REQUIRE(register_memory_range(MemoryRange(0,0x0000Afff)) == true);
	REQUIRE(register_memory_range(MemoryRange(0,0x0000Afff)) == true);
	REQUIRE(register_memory_range(MemoryRange(0,0x0000Afff)) == false);
	REQUIRE(register_memory_range(MemoryRange(0,0x0000Afff)) == false);
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

TEST_CASE( "page_align_mem_range" )
{
	MemoryRange res;

	// mem_range.GetEnd() < (PAGE_SIZE - 1)
	res = page_align_mem_range(MemoryRange(0x00000000, 0x00000123));
	REQUIRE(res.IsValid() == false);
	res = page_align_mem_range(MemoryRange(0x00000003, 0x00000123));
	REQUIRE(res.IsValid() == false);
	res = page_align_mem_range(MemoryRange(0xfffff000, 0x00000123));
	REQUIRE(res.IsValid() == false);
	res = page_align_mem_range(MemoryRange(0x00000000, 0x00000ffe));
	REQUIRE(res.IsValid() == false);
	res = page_align_mem_range(MemoryRange(0x00000003, 0x00000ffe));
	REQUIRE(res.IsValid() == false);
	res = page_align_mem_range(MemoryRange(0xfffff000, 0x00000ffe));
	REQUIRE(res.IsValid() == false);

	// mem_range.GetEnd() == (PAGE_SIZE - 1)
	res = page_align_mem_range(MemoryRange(0x00000000, 0x00000fff));
	REQUIRE(res.IsValid() == true);
	res = page_align_mem_range(MemoryRange(0x00000001, 0x00000fff));
	REQUIRE(res.IsValid() == false);
	res = page_align_mem_range(MemoryRange(0x23456000, 0x00000fff));
	REQUIRE(res.IsValid() == false);

	// less than one full page possible
	res = page_align_mem_range(MemoryRange(0x0000A000, 0x0000A123));
	REQUIRE(res.IsValid() == false);
	res = page_align_mem_range(MemoryRange(0x0000A333, 0x0000A555));
	REQUIRE(res.IsValid() == false);
	res = page_align_mem_range(MemoryRange(0x0000A333, 0x0000B555));
	REQUIRE(res.IsValid() == false);


	// one page possible (including border cases)
	res = page_align_mem_range(MemoryRange(0x0000B001, 0x0000C111));
	REQUIRE(res.IsValid() == false);

	res = page_align_mem_range(MemoryRange(0x0000B000, 0x0000C111));
	REQUIRE(res.IsValid() == true);
	REQUIRE(res.GetStart() == 0x0000B000);
	REQUIRE(res.GetEnd() ==   0x0000B000);

	res = page_align_mem_range(MemoryRange(0x0000A333, 0x0000Bffe));
	REQUIRE(res.IsValid() == false);

	res = page_align_mem_range(MemoryRange(0x0000A333, 0x0000Bfff));
	REQUIRE(res.IsValid() == true);
	REQUIRE(res.GetStart() == 0x0000B000);
	REQUIRE(res.GetEnd() ==   0x0000B000);

	res = page_align_mem_range(MemoryRange(0x0000B000, 0x0000Bfff));
	REQUIRE(res.IsValid() == true);
	REQUIRE(res.GetStart() == 0x0000B000);
	REQUIRE(res.GetEnd() ==   0x0000B000);

	res = page_align_mem_range(MemoryRange(0x0000A333, 0x0000C111));
	REQUIRE(res.IsValid() == true);
	REQUIRE(res.GetStart() == 0x0000B000);
	REQUIRE(res.GetEnd() ==   0x0000B000);


	// more than one page
	res = page_align_mem_range(MemoryRange(0x0000A333, 0x1234C111));
	REQUIRE(res.IsValid() == true);
	REQUIRE(res.GetStart() == 0x0000B000);
	REQUIRE(res.GetEnd() ==   0x1234B000);

	// whole memory
	res = page_align_mem_range(MemoryRange(0x00000000, 0xffffffff));
	REQUIRE(res.IsValid() == true);
	REQUIRE(res.GetStart() == 0x00000000);
	REQUIRE(res.GetEnd() ==   0xfffff000);

	// whole memory except last page
	res = page_align_mem_range(MemoryRange(0x00000000, 0xfffffffe));
	REQUIRE(res.IsValid() == true);
	REQUIRE(res.GetStart() == 0x00000000);
	REQUIRE(res.GetEnd() ==   0xffffe000);

	// whole memory except first page and last page
	res = page_align_mem_range(MemoryRange(0x00000001, 0xfffffffe));
	REQUIRE(res.IsValid() == true);
	REQUIRE(res.GetStart() == 0x00001000);
	REQUIRE(res.GetEnd() ==   0xffffe000);

	// random range
	res = page_align_mem_range(MemoryRange(0x00751df2, 0x09cd4567));
	REQUIRE(res.IsValid() == true);
	REQUIRE(res.GetStart() == 0x00752000);
	REQUIRE(res.GetEnd() ==   0x09cd3000);

	// random but page-aligned range
	res = page_align_mem_range(MemoryRange(0x07e5d000, 0x09cd4fff));
	REQUIRE(res.IsValid() == true);
	REQUIRE(res.GetStart() == 0x07e5d000);
	REQUIRE(res.GetEnd() ==   0x09cd4000);

	// random but page-aligned range, except last page
	res = page_align_mem_range(MemoryRange(0x07e5d000, 0x09cd4ffe));
	REQUIRE(res.IsValid() == true);
	REQUIRE(res.GetStart() == 0x07e5d000);
	REQUIRE(res.GetEnd() ==   0x09cd3000);

	// random but page-aligned range, except first page
	res = page_align_mem_range(MemoryRange(0x07e5d001, 0x09cd4fff));
	REQUIRE(res.IsValid() == true);
	REQUIRE(res.GetStart() == 0x07e5e000);
	REQUIRE(res.GetEnd() ==   0x09cd4000);

	// random range, negative range
	res = page_align_mem_range(MemoryRange(0x09cd4567, 0x00751df2));
	REQUIRE(res.IsValid() == false);

	// random but page-aligned range, negative range
	res = page_align_mem_range(MemoryRange(0x09cd4fff, 0x07e5d000));
	REQUIRE(res.IsValid() == false);

	// zero-range at zero
	res = page_align_mem_range(MemoryRange(0x00000000, 0x00000000));
	REQUIRE(res.IsValid() == false);

	// all ones-range
	res = page_align_mem_range(MemoryRange(0xffffffff, 0xffffffff));
	REQUIRE(res.IsValid() == false);

	// zero-range at random position
	res = page_align_mem_range(MemoryRange(0xfd30b9c0, 0xfd30b9c0));
	REQUIRE(res.IsValid() == false);

	// zero-range at random page-aligned position
	res = page_align_mem_range(MemoryRange(0xfd30b000, 0xfd30b000));
	REQUIRE(res.IsValid() == false);
}
