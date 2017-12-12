void halt_message(const char *file, int line, const char *function, const char *message, ...) __attribute__ ((noreturn));

#define assert(condition) if (!(condition)) { halt_message(__FILE__, __LINE__, __func__, "assertion failed: " #condition); };
#define halt(...) halt_message(__FILE__, __LINE__, __func__, __VA_ARGS__)
