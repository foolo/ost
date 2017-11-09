extern "C" {
#include "md5.h"
}
#include "md5digest.h"
#include <stdio.h>

void md5digest(const void *data, unsigned long size, unsigned char *digest_out) {
	MD5_CTX context;
	MD5_Init (&context);
	MD5_Update (&context, data, size);
	MD5_Final (digest_out, &context);
}

void print_md5(unsigned char *digest) {
	for (int i = 0; i < 16; i++) {
		printf ("%02x", digest[i]);
	}
	printf ("\n");
}
