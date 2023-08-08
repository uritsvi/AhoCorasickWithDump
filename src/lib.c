#include "../include/lib.h"

MallocFunction g_MallocFunction;
FreeFunction g_FreeFunctions;


void init_lib(
	_In_ MallocFunction Malloc,
	_In_ FreeFunction Free) {

	g_MallocFunction = Malloc;
	g_FreeFunctions = Free;
}

void* lib_malloc(size_t size) {
	return g_MallocFunction(size);
}
void lib_free(void* p) {
	g_FreeFunctions(p);
}