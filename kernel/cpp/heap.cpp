#include <stddef.h>
#include <unistd.h>

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


namespace std
{
	void __throw_bad_alloc()
	{
		_exit(1);
	}
}
