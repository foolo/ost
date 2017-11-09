#include <stdint.h>

void md5digest(const void *data, unsigned long size, unsigned char *digest_out);
void print_md5(unsigned char *digest);
