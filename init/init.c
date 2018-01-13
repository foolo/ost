#include <stdio.h>
#include <string.h>

int main()
{
	printf("init\n");
	char line[256];

	while(1) {
		char *c = fgets(line, sizeof(line), stdin);
		if (c) {
			size_t len = strlen(c);
			printf("read %i bytes: %s\n", len, c);
		}
		else {
			printf("read failed\n");
		}
	}
	return 0;
}

