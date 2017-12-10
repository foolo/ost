#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "memory/ia32/paging.h"
#include "elf_loader.h"



struct elf32_program_header {
	uint32_t p_type;
	uint32_t p_offset;
	uint32_t p_vaddr;
	uint32_t p_paddr;
	uint32_t p_filesz;
	uint32_t p_memsz;
	uint32_t p_flags;
	uint32_t p_align;
};

enum program_header_types {
	PT_NULL    = 0, // unused element
	PT_LOAD    = 1, // loadable segment
	PT_DYNAMIC = 2, // dynamic linking information
	PT_INTERP  = 3, // interpreter path
	PT_NOTE    = 4, // auxiliary information
	PT_SHLIB   = 5, // reserved
	PT_PHDR    = 6, // program header location
};

enum program_header_flags {
	PF_X = 1,
	PF_W = 2,
	PF_R = 4,
};

bool load_program_header(struct file *f, struct elf32_program_header *ph, uint32_t *userspace_pagedir) {
	if ((ph->p_offset & 0xfff) != (ph->p_vaddr & 0xfff)) {
		return false;
	}
	// todo check if phdr->p_offset > file size
	if (ph->p_memsz < ph->p_filesz) {
		return false;
	}
	if (ph->p_memsz == 0) {
		return true;
	}
	if (ph->p_vaddr + ph->p_memsz < ph->p_vaddr) {
		return false;
	}
	if (ph->p_vaddr < 0x1000) {
		return false;
	}

	//bool writable = (ph->p_flags & PF_W);

	set_up_userspace_page_tables(userspace_pagedir, ph->p_vaddr, ph->p_memsz);

	file_seek(f, ph->p_offset);

	if (file_read(f, (void*)ph->p_vaddr, ph->p_filesz) != ph->p_filesz) {
		return false;
	}
	uint32_t zero_bytes = ph->p_memsz - ph->p_filesz;
	memset((void*)(ph->p_vaddr + ph->p_filesz), 0, zero_bytes);
	return true;
}

bool load_elf(struct file *f, uint32_t *userspace_pagedir, struct elf32_file_header *fh, struct process_info *p) {

	if (file_read(f, fh, sizeof(struct elf32_file_header)) != sizeof(struct elf32_file_header)) {
		printf("read file header failed\n");
		return false;
	}

	// executable, x86, elf version 1
	if (fh->e_type != 2 || fh->e_machine != 3 || fh->e_version != 1) {
		printf("invalid file header\n");
		return false;
	}

	uint32_t brk = 0;
	uint32_t offset = fh->e_phoff;
	for (int i = 0; i < fh->e_phnum; i++) {
		file_seek(f, offset);
		struct elf32_program_header ph;
		if (file_read(f, &ph, sizeof(ph)) != sizeof(ph)) {
			printf("read program header failed\n");
			return false;
		}
		offset += sizeof(ph);

		switch (ph.p_type) {
		case PT_NULL:
		case PT_NOTE:
		case PT_PHDR:
			break;
		case PT_DYNAMIC:
		case PT_INTERP:
		case PT_SHLIB:
		default:
			printf("invalid program header\n");
			return false;
		case PT_LOAD:
			if (!load_program_header(f, &ph, userspace_pagedir)) {
				printf("load program header failed\n");
				return false;
			}
			uint32_t segment_end_address = ph.p_vaddr + ph.p_memsz;
			if (segment_end_address > brk) {
				brk = segment_end_address;
			}
		}
	}
	p->current_break = (void*)brk;
	return true;
}
