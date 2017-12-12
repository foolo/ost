#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "cpio_loader.h"
#include "fs/fs.h"

struct cpio_newc_header {
	char    c_magic[6];
	char    c_ino[8];
	char    c_mode[8];
	char    c_uid[8];
	char    c_gid[8];
	char    c_nlink[8];
	char    c_mtime[8];
	char    c_filesize[8];
	char    c_devmajor[8];
	char    c_devminor[8];
	char    c_rdevmajor[8];
	char    c_rdevminor[8];
	char    c_namesize[8];
	char    c_check[8];
};

bool is_hex_char(char c) {
	if (c > 'f') {
		return false;
	}
	if (c >= 'a') {
		return true;
	}
	if (c > 'F') {
		return false;
	}
	if (c >= 'A') {
		return true;
	}
	if (c > '9') {
		return false;
	}
	if (c >= '0') {
		return true;
	}
	return false;
}

bool validate_hex(uint8_t *s, size_t length) {
	for(uint8_t *c = s; c < s + length; c++) {
		if (!is_hex_char(*c)) {
			return false;
		}
	}
	return true;
}

int hex_to_int(char c) {
	if (c >= 'a') {
		return c - 'a' + 10;
	}
	if (c >= 'A') {
		return c - 'A' + 10;
	}
	return c - '0';
}

uint32_t hex_to_uint32_t(char *s) {
	uint32_t result = 0;
	for (char *c = s; c < s + 8; c++) {
		result <<= 4;
		result += hex_to_int(*c);
	}
	return result;
}


bool load_file(struct cpio_newc_header *header, uint8_t *start, size_t *pos, struct file *dir, size_t length) {
	if (memcmp(header->c_magic, "070702", 6) != 0) {
		return false;
	}
	uint32_t namesize = hex_to_uint32_t(header->c_namesize);
	if (namesize > 256) {
		return false;
	}
	if (length - *pos < namesize) {
		return false;
	}
	char filename[256];
	memcpy(filename, start + *pos, namesize);
	*pos += namesize;
	*pos = (*pos + 3) & ~3;

	if (filename[namesize - 1] != 0) {
		return false;
	}

	if (strcmp(filename, "TRAILER!!!") == 0) {
		*pos = length;
		return true;
	}

	uint32_t filesize = hex_to_uint32_t(header->c_filesize);
	if (length - *pos < filesize) {
		return false;
	}

	if (!file_create(filename, dir)) {
		return false;
	}
	struct file f;
	if (!file_open(filename, &f, dir)) {
		return false;
	}

	if (file_write(&f, start + *pos, filesize) != filesize) {
		return false;
	}
	*pos += filesize;
	*pos = (*pos + 3) & ~3;
	return true;
}

bool load_cpio(uint8_t *start, size_t length) {
	struct file root_dir = dir_open_root();
	size_t pos = 0;
	while(pos < length) {
		struct cpio_newc_header header;
		if (length - pos < sizeof(header)) {
			return false;
		}
		if (!validate_hex(start + pos, sizeof(header))) {
			return false;
		}
		memcpy(&header, start + pos, sizeof(header));
		pos += sizeof(header);
		if (!load_file(&header, start, &pos, &root_dir, length)) {
			return false;
		}
	}
	return true;
}
