#include <stdio.h>
#include "multiboot.h"
#include "multiboot_mmap.h"
#include "page_allocator.h"
#include "memory/MemoryRange.h"

void multiboot_mmap(unsigned long magic, multiboot_info_t *mbi)
{
	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf("Invalid magic number: 0x%lx\n", magic);
		return;
	}

	printf("flags = 0x%x\n", mbi->flags);

	/* Are mem_* valid? */
	if (CHECK_FLAG(mbi->flags, 0))
	{
		printf("mem_lower = %uKiB, mem_upper = %uKiB\n", mbi->mem_lower, mbi->mem_upper);
	}

	/* Is boot_device valid? */
	if (CHECK_FLAG(mbi->flags, 1))
	{
		printf("boot_device = 0x%x\n", mbi->boot_device);
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
		printf("mods_count = %d, mods_addr = 0x%x\n", mbi->mods_count, mbi->mods_addr);
		for (i = 0, mod = (multiboot_module_t *) mbi->mods_addr; i < mbi->mods_count; i++, mod++)
		{
			printf(" mod_start = 0x%x, mod_end = 0x%x, cmdline = %s\n",
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

		printf("multiboot_aout_symbol_table: tabsize = 0x%0x, "
				"strsize = 0x%x, addr = 0x%x\n",
				multiboot_aout_sym->tabsize,
				multiboot_aout_sym->strsize,
				multiboot_aout_sym->addr);
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG(mbi->flags, 5))
	{
		multiboot_elf_section_header_table_t *multiboot_elf_sec = &(mbi->u.elf_sec);
		printf("multiboot_elf_sec: num = %u, size = 0x%x,"
				" addr = 0x%x, shndx = 0x%x\n",
				multiboot_elf_sec->num,
				multiboot_elf_sec->size,
				multiboot_elf_sec->addr,
				multiboot_elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (!CHECK_FLAG(mbi->flags, 6))
	{
		printf("mmap fields are not valid\n");
		return;
	}

	printf("mmap_addr = 0x%x, mmap_length = 0x%x\n",
			mbi->mmap_addr,
			mbi->mmap_length);

	multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
	while ((unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length)
	{
		printf("size = %x, ", mmap->size);
		printf("base_addr = %8llx, ", mmap->addr);
		printf("length = %8llx, ", mmap->len);
		printf("type = %x", mmap->type);
		printf("\n");

		const uint32_t USABLE_RAM = 1;
		if (mmap->type == USABLE_RAM)
		{
			addr_t start = mmap->addr;
			addr_t end = mmap->addr + mmap->len - 1;
			struct MemoryRange memoryRange = {start, end, 1};
			if (!register_memory_range(&memoryRange))
			{
				break;
			}
		}
		mmap = (multiboot_memory_map_t *) ((unsigned long) mmap + mmap->size + sizeof(mmap->size));
	}
}
