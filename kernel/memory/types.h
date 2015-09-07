#ifndef MEMORY_TYPES_H_
#define MEMORY_TYPES_H_

#if defined __i386__
	typedef uint32_t addr_t;
#elif defined __x86_64__
	typedef uint64_t addr_t;
#else
	#error unknown architechture
#endif

#endif // MEMORY_TYPES_H_
