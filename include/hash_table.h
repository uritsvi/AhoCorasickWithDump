#ifndef __HASH__TABLE__  
 #define __HASH__TABLE__  
   
 #define MAX_KEYS (1 << 16)  
   
 #include <sal.h>  
 #include <stdint.h>  
 #include <stdbool.h>  
   
 #include "list.h"  
   
typedef struct{  
 	ListEntry Entry;  
 	wchar_t* Key;  
 } ValueEntry;
   
typedef struct {  
 	ListEntry Entries[MAX_KEYS];  
 	bool FreeValuesOnDelete;  
 }HashTable;
   
 void init_map(  
 	_In_ HashTable* Map,   
 	_In_ bool freeValuesOnDelete);  
   
 bool push_value_to_map(  
 	_In_ HashTable* Map,  
 	_In_ wchar_t* Key,  
 	_In_ size_t Len,  
 	_In_ ValueEntry* Entry);  
   
 bool query_map_value(  
 	_In_ HashTable* Map,  
 	_In_ wchar_t* Key,  
 	_In_ size_t Len,  
 	_In_ ValueEntry** Entry);  
   
 #endif  
 