#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "fs.h"
#include "malloc.h"
#include "util/assert.h"

#define MAX_INODES 10


struct inode *inodes[MAX_INODES] = {0};

size_t inode_read(struct inode *i, void *dst, size_t start, size_t length) {
	if (i->cached_data.data != NULL) {
		if (start > i->cached_data.size) {
			return 0;
		}
		size_t bytes_left = i->cached_data.size - start;
		size_t bytes_to_read = (length > bytes_left) ? (bytes_left) : (length);
		memcpy(dst, i->cached_data.data + start, bytes_to_read);
		return bytes_to_read;
	}
	return 0;
}

size_t inode_write(struct inode *i, void *src, size_t start, size_t length) {
	if (i->cached_data.data != NULL) {
		if (start > i->cached_data.size) {
			return 0;
		}
		size_t required_capacity = start + length;
		if (required_capacity > i->cached_data.size) {
			if (required_capacity > i->cached_data.capacity) {
				i->cached_data.capacity = 2 * required_capacity;
				void *p = realloc(i->cached_data.data, i->cached_data.capacity);
				if (p) {
					i->cached_data.data = p;
				}
				else {
					halt("realloc failed");
				}
			}
			i->cached_data.size = required_capacity;
		}
		memcpy(i->cached_data.data + start, src, length);
		return length;
	}
	return 0;
}

size_t inode_get_size(struct inode *i) {
	if (i->cached_data.data != NULL) {
		return i->cached_data.size;
	}
	else {
		return i->disk_data.size;
	}
}

void inode_open(size_t inode_number) {
	inodes[inode_number]->open_cnt++;
}

void inode_close(size_t inode_number) {
	if (inodes[inode_number]->open_cnt > 0) {
		inodes[inode_number]->open_cnt--;
	}
}

void inode_init(struct inode **i) {
	*i = calloc(1, sizeof(struct inode));
	if(*i == NULL) {
		halt("calloc failed");
	}
	(*i)->cached_data.data = malloc(4);
	assert((*i)->cached_data.data != NULL)
	(*i)->cached_data.capacity = 4;
}

void create_root_inode() {
	if (inodes[0]) {
		return;
	}
	inode_init(&inodes[0]);
}

unsigned inode_create() {
	for (unsigned i = 0; i < MAX_INODES; i++) {
		if (inodes[i] == NULL) {
			inode_init(&inodes[i]);
			return i;
		}
	}
	return 0;
}

size_t file_read(struct file *f, void *dst, size_t length) {
	size_t bytes_read = inode_read(f->inode, dst, f->pos, length);
	f->pos += bytes_read;
	return bytes_read;
}

size_t file_write(struct file *f, void *src, size_t length) {
	size_t bytes_written = inode_write(f->inode, src, f->pos, length);
	f->pos += bytes_written;
	return bytes_written;
}

void file_seek_start(struct file *f) {
	f->pos = 0;
}

void file_seek_end(struct file *f) {
	f->pos = inode_get_size(f->inode);
}

size_t file_tell(struct file *f) {
	return f->pos;
}

void file_seek(struct file *f, size_t pos) {
	f->pos = pos;
}

struct file dir_open_root() {
	struct file result;
	result.inode = inodes[0];
	result.pos = 0;
	return result;
}

uint32_t read_inode_number(struct file *dir) {
	uint8_t s[4];
	if (file_read(dir, s, 4) != 4) {
		return 0;
	}
	return ((uint32_t)s[3] << 24) + ((uint32_t)s[2] << 16) + ((uint32_t)s[1] << 8) + ((uint32_t)s[0]);
}

uint32_t dir_lookup(struct file *dir, const char *name) {
	file_seek_start(dir);
	uint8_t filename_length;
	while (file_read(dir, &filename_length, 1) == 1) {
		char s[256];
		if (file_read(dir, s, filename_length) != filename_length) {
			return 0;
		}
		s[filename_length] = 0;
		if (strcmp(s, name) == 0) {
			return read_inode_number(dir);
		}
		else {
			file_seek(dir, file_tell(dir) + 4);
		}
	}
	return 0;
}

bool dir_add_file(struct file *dir, const char *name, uint32_t inode_number) {
	size_t name_length = strlen(name);
	if (name_length == 0 || name_length > 255) {
		return false;
	}
	file_seek_end(dir);
	uint8_t name_length_byte = (uint8_t)name_length;
	file_write(dir, &name_length_byte, 1);
	file_write(dir, (void*)name, name_length);
	uint8_t inode_bytes[4];
	inode_bytes[0] = inode_number;
	inode_bytes[1] = inode_number >> 8;
	inode_bytes[2] = inode_number >> 16;
	inode_bytes[3] = inode_number >> 24;
	return (file_write(dir, inode_bytes, 4) == 4);
}

bool file_open(const char *name, struct file *f, struct file *dir) {
	uint32_t i = dir_lookup(dir, name);
	if (i == 0) {
		return false;
	}
	f->inode = inodes[i];
	f->pos = 0;
	return true;
}

bool file_create(const char *name, struct file *dir) {
	if (dir_lookup(dir, name) > 0) {
		return false;
	}
	unsigned inode_number = inode_create();
	if (inode_number == 0){
		return false;
	}
	return dir_add_file(dir, name, inode_number);
}
