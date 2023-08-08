#include <string.h>  
#include <stddef.h>  
   
#include "../include/hash_table.h"  
#include "../include/lib.h"
   
 // Hash function is from here:  
 //https://intellipaat.com/community/10917/why-does-javas-hashcode-in-string-use-31-as-a-multiplier#:~:text=The%20value%2031%20was%20chosen,clear%2C%20but%20it%20is%20traditional  
 size_t hash_wcs(  
 	_In_ wchar_t* Wcs,   
 	_In_ size_t len) {  
 	  
 	size_t out = 0;  
   
 	for (int i = 0; i < len; i++) {  
 		out += Wcs[i]*31^(len - i - 1);  
 	}  
   
 	return out % MAX_KEYS;  
 }  
   
 void init_map(  
 	_In_ HashTable* Map,   
 	_In_ bool freeValuesOnDelete) {  
 	  
 	Map->FreeValuesOnDelete = freeValuesOnDelete;  
   
 	for (int i = 0; i < MAX_KEYS; i++) {  
 		init_list(&Map->Entries[i]);  
 	}  
 }  
   
 bool push_value_to_map(  
 	_In_ HashTable* Map,  
 	_In_ wchar_t* Key,  
 	_In_ size_t Len,  
 	_In_ ValueEntry* Entry) {  
   
 	Entry->Key =   
        lib_malloc(
 			(Len + 1) * sizeof(wchar_t)  
 		);  
   
 	Entry->Key[Len] = L'\0';  
 	  
 	memcpy_s(  
 		Entry->Key,   
 		Len * sizeof(wchar_t),  
 		Key,  
 		Len * sizeof(wchar_t));  
   
 	size_t hash = hash_wcs(  
 		Key,   
 		Len);  
 	ValueEntry* current = 
        (ValueEntry*)&Map->Entries[hash];  
 	  
 	if (current->Entry.Len > 0) {  
 		for (  
 			ValueEntry* currentListEntry =  
 				(ValueEntry*)start_iter_list(&current->Entry);  
 			currentListEntry != NULL;  
 			currentListEntry = iter_list_next(&current->Entry)) {  
   
 			if (wcscmp(currentListEntry->Key, Key) == 0) {  
   
 				return false;  
 			}  
 		}  
 	}  
   
 	push_front(  
 		&Map->Entries[hash],   
 		&Entry->Entry);  
   
 	return true;  
 }  
   
 bool query_map_value(  
 	_In_ HashTable* Map,  
 	_In_ wchar_t* Key,  
 	_In_ size_t Len,  
 	_In_ ValueEntry** Entry) {  
   
 	size_t hash = hash_wcs(  
 		Key,   
 		Len);  
 	  
 	ListEntry head = Map->Entries[hash];  
   
 	if (head.Len == 0) {  
 		return false;  
 	}  
 	if (head.Len < 2) {  
 		*Entry = (ValueEntry*)head.Next;  
 		return true;  
 	}  
   
 	for (  
 		ValueEntry* current =   
 			start_iter_list(&head);   
 		current != NULL;   
 		current = iter_list_next(&head)) {  
   
 		if (wcscmp(current->Key, Key) == 0) {  
   
 			*Entry =  current;  
 			return true;  
 		}  
 	}  
   
 	return false;  
   
 } 