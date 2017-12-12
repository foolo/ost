#include <stdio.h>
#include <stdarg.h>

void halt_message(const char *file, int line, const char *function, const char *message, ...) {
	printf("%s:%i in %s: ", file, line, function);
	va_list args;
	va_start (args, message);
    vprintf (message, args);
    printf ("\n");
    va_end (args);
	while(1){};
}
