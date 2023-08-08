#ifndef __LIB__
#define __LIB__

#include <stdint.h>
#include <sal.h>

typedef void* (*MallocFunction)(size_t size);
typedef void(*FreeFunction)();

void init_lib(
	_In_ MallocFunction Malloc, 
	_In_ FreeFunction Free);

void* lib_malloc(size_t size);
void lib_free(void* p);

#endif