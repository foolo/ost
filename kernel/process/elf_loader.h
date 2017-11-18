namespace kernel {

struct elf32_file_header {
	uint8_t e_ident[16];
	uint16_t e_type;
	uint16_t e_machine;

	uint32_t e_version;
	uint32_t e_entry;
	uint32_t e_phoff;
	uint32_t e_shoff;
	uint32_t e_flags;

	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
};

bool load_elf(int fd, uint32_t *userspace_pagedir, elf32_file_header &fh);

} // namespace kernel