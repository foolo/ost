#include <stdio.h>
#include "multiboot.h"
#include "multiboot_mmap.h"
#include <stdint.h>

namespace kernel
{

void multiboot_mmap(unsigned long magic, multiboot_info_t *mbi)
{
	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf("Invalid magic number: 0x%x\n", (unsigned) magic);
		return;
	}

	/* Print out the flags. */
	printf("flags = 0x%x\n", (unsigned) mbi->flags);

	/* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0))
		printf("mem_lower = %uKB, mem_upper = %uKB\n",
				(unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

	/* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		printf("boot_device = 0x%x\n", (unsigned) mbi->boot_device);

	/* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
		printf("cmdline = %s\n", (char *) mbi->cmdline);

	/* Are mods_* valid? */
	if (CHECK_FLAG (mbi->flags, 3))
	{
		multiboot_module_t *mod;
		unsigned int i;

		printf("mods_count = %d, mods_addr = 0x%x\n", (int) mbi->mods_count,
				(int) mbi->mods_addr);
		for (i = 0, mod = (multiboot_module_t *) mbi->mods_addr;
				i < mbi->mods_count; i++, mod++)
			printf(" mod_start = 0x%x, mod_end = 0x%x, cmdline = %s\n",
					(unsigned) mod->mod_start, (unsigned) mod->mod_end,
					(char *) mod->cmdline);
	}

	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
	{
		printf("Both bits 4 and 5 are set.\n");
		return;
	}

	/* Is the symbol table of a.out valid? */
	if (CHECK_FLAG (mbi->flags, 4))
	{
		multiboot_aout_symbol_table_t *multiboot_aout_sym = &(mbi->u.aout_sym);

		printf("multiboot_aout_symbol_table: tabsize = 0x%0x, "
				"strsize = 0x%x, addr = 0x%x\n",
				(unsigned) multiboot_aout_sym->tabsize,
				(unsigned) multiboot_aout_sym->strsize,
				(unsigned) multiboot_aout_sym->addr);
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG (mbi->flags, 5))
	{
		multiboot_elf_section_header_table_t *multiboot_elf_sec =
				&(mbi->u.elf_sec);

		printf("multiboot_elf_sec: num = %u, size = 0x%x,"
				" addr = 0x%x, shndx = 0x%x\n",
				(unsigned) multiboot_elf_sec->num,
				(unsigned) multiboot_elf_sec->size,
				(unsigned) multiboot_elf_sec->addr,
				(unsigned) multiboot_elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6))
	{
		multiboot_memory_map_t *mmap;

		printf("mmap_addr = 0x%x, mmap_length = 0x%x\n",
				mbi->mmap_addr, (unsigned) mbi->mmap_length);


		mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
		while ((unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length)
		{
			uint32_t field_size = (uint32_t)mmap->size;
			uint32_t addr_lo = (uint32_t)(mmap->addr & 0xffffffff);
			uint32_t addr_hi = (uint32_t)(mmap->addr >> 32);
			uint32_t length_lo = (uint32_t)(mmap->len & 0xffffffff);
			uint32_t length_hi = (uint32_t)(mmap->len >> 32);
			uint32_t type = (uint32_t)mmap->type;
			printf("size = %lx, ", field_size);
			printf("base_addr = %lx:%8lx, ", addr_hi, addr_lo);
			printf("length = %lx:%8lx, ", length_hi, length_lo);
			printf("type = %lx", type);
			printf("\n");
			mmap = (multiboot_memory_map_t *) ((unsigned long) mmap + mmap->size + sizeof(mmap->size));
		}
	}
}

} // namespace kernel
