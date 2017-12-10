#pragma once

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

struct inode_disk {
	uint32_t sector;
	size_t size;
};

struct inode_cache {
	void *data;
	size_t size;
	size_t capacity;
};
struct inode {
	long ref_cnt;
	long open_cnt;
	struct inode_disk disk_data;
	struct inode_cache cached_data;
};

struct file {
	struct inode* inode;
	size_t pos;
};

void create_root_inode();
size_t file_read();
size_t file_write(struct file *f, void *src, size_t length);
void file_seek(struct file *f, size_t pos);
struct file dir_open_root();
bool file_open(const char *name, struct file *f, struct file *dir);
bool file_create(const char *name, struct file *dir);
