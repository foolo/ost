#include <stddef.h>

void *operator new(size_t /*size*/)
{
	//return malloc(size); // todo
	return NULL;
}

void *operator new[](size_t /*size*/)
{
	//return malloc(size); // todo
	return NULL;
}

void operator delete(void * /*p*/)
{
	//free(p); // todo
}

void operator delete[](void * /*p*/)
{
	//free(p); // todo
}
