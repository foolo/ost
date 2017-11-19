#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include "memory/ia32/paging.h"
#include "elf_loader.h"

#include <string.h>

extern uint8_t *binfile;
extern uint32_t binfile_size;
size_t offset = 0;

int lseek_binfile(int ptr) {
	offset = ptr;
	return offset;
}

int read_binfile(char *dst, int len) {
	int bytes_left = binfile_size - offset;
	int bytes_to_read = (len < bytes_left) ? len : bytes_left;
	memcpy(dst, binfile + offset, bytes_to_read);
	offset += bytes_to_read;
	return bytes_to_read;
}


namespace kernel {

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

bool load_program_header(int fd, elf32_program_header &ph, uint32_t *userspace_pagedir) {
	if ((ph.p_offset & 0xfff) != (ph.p_vaddr & 0xfff)) {
		return false;
	}
	// todo check if phdr->p_offset > file size
	if (ph.p_memsz < ph.p_filesz) {
		return false;
	}
	if (ph.p_memsz == 0) {
		return true;
	}
	if (ph.p_vaddr + ph.p_memsz < ph.p_vaddr) {
		return false;
	}
	if (ph.p_vaddr < 0x1000) {
		return false;
	}

	//bool writable = (ph.p_flags & PF_W);

	set_up_userspace_page_tables(userspace_pagedir, ph.p_vaddr, ph.p_memsz);

	if (lseek(fd, ph.p_offset, SEEK_SET) != ph.p_offset) {
		return false;
	}
	if (read(fd, (void*)ph.p_vaddr, ph.p_filesz) != (int)ph.p_filesz) {
		return false;
	}
	uint32_t zero_bytes = ph.p_memsz - ph.p_filesz;
	memset((void*)(ph.p_vaddr + ph.p_filesz), 0, zero_bytes);
	return true;
}

bool load_elf(int fd, uint32_t *userspace_pagedir, elf32_file_header &fh) {

	if (read(fd, &fh, sizeof(fh)) != sizeof(fh)) {
		printf("read file header failed\n");
		return false;
	}

	// executable, x86, elf version 1
	if (fh.e_type != 2 || fh.e_machine != 3 || fh.e_version != 1) {
		printf("invalid file header\n");
		return false;
	}

	uint32_t offset = fh.e_phoff;
	for (int i = 0; i < fh.e_phnum; i++) {

		if (lseek(fd, offset, SEEK_SET) < 0) {
			return false;
		}

		elf32_program_header ph;
		if (read(fd, &ph, sizeof(ph)) != sizeof(ph)) {
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
			if (!load_program_header(fd, ph, userspace_pagedir)) {
				printf("load program header failed\n");
				return false;
			}
		}
	}
	return true;
}

} // namespace kernel