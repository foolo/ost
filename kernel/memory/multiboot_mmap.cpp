#include <stdio.h>
#include <stdint.h>
#include "multiboot.h"
#include "multiboot_mmap.h"
#include "memory/MemoryRange.h"

namespace kernel
{

const int g_MEM_RANGES_COUNT = 5;
MemoryRange g_mem_ranges[g_MEM_RANGES_COUNT];

void multiboot_mmap(unsigned long magic, multiboot_info_t *mbi)
{
	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf("Invalid magic number: 0x%lx\n", magic);
		return;
	}

	printf("flags = 0x%lx\n", mbi->flags);

	/* Are mem_* valid? */
	if (CHECK_FLAG(mbi->flags, 0))
	{
		printf("mem_lower = %luKiB, mem_upper = %luKiB\n", mbi->mem_lower, mbi->mem_upper);
	}

	/* Is boot_device valid? */
	if (CHECK_FLAG(mbi->flags, 1))
	{
		printf("boot_device = 0x%lx\n", mbi->boot_device);
	}

	/* Is the command line passed? */
	if (CHECK_FLAG(mbi->flags, 2))
	{
		printf("cmdline = %s\n", (char *) mbi->cmdline);
	}

	/* Are mods_* valid? */
	if (CHECK_FLAG(mbi->flags, 3))
	{
		multiboot_module_t *mod;
		unsigned int i;
		printf("mods_count = %ld, mods_addr = 0x%lx\n", mbi->mods_count, mbi->mods_addr);
		for (i = 0, mod = (multiboot_module_t *) mbi->mods_addr; i < mbi->mods_count; i++, mod++)
		{
			printf(" mod_start = 0x%lx, mod_end = 0x%lx, cmdline = %s\n",
					mod->mod_start,
					mod->mod_end,
					(char *) mod->cmdline);
		}
	}

	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG(mbi->flags, 5))
	{
		printf("Both bits 4 and 5 are set.\n");
		return;
	}

	/* Is the symbol table of a.out valid? */
	if (CHECK_FLAG(mbi->flags, 4))
	{
		multiboot_aout_symbol_table_t *multiboot_aout_sym = &(mbi->u.aout_sym);

		printf("multiboot_aout_symbol_table: tabsize = 0x%0lx, "
				"strsize = 0x%lx, addr = 0x%lx\n",
				multiboot_aout_sym->tabsize,
				multiboot_aout_sym->strsize,
				multiboot_aout_sym->addr);
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG(mbi->flags, 5))
	{
		multiboot_elf_section_header_table_t *multiboot_elf_sec = &(mbi->u.elf_sec);
		printf("multiboot_elf_sec: num = %lu, size = 0x%lx,"
				" addr = 0x%lx, shndx = 0x%lx\n",
				multiboot_elf_sec->num,
				multiboot_elf_sec->size,
				multiboot_elf_sec->addr,
				multiboot_elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (CHECK_FLAG(mbi->flags, 6))
	{
		multiboot_memory_map_t *mmap;
		printf("mmap_addr = 0x%lx, mmap_length = 0x%lx\n",
				mbi->mmap_addr,
				mbi->mmap_length);

		mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
		unsigned range_index = 0;
		while ((unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length)
		{
			printf("size = %lx, ", mmap->size);
			printf("base_addr = %8llx, ", mmap->addr);
			printf("length = %8llx, ", mmap->len);
			printf("type = %lx", mmap->type);
			printf("\n");

			const uint32_t USABLE_RAM = 1;
			if (mmap->type == USABLE_RAM)
			{
				uint32_t start = mmap->addr;
				uint32_t end = mmap->addr + mmap->len - 1;
				g_mem_ranges[range_index++] = MemoryRange(start, end);
				if (range_index >= g_MEM_RANGES_COUNT)
				{
					break;
				}
			}
			mmap = (multiboot_memory_map_t *) ((unsigned long) mmap + mmap->size + sizeof(mmap->size));
		}
	}
}

} // namespace kernel
