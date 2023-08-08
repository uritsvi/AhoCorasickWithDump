#include <stddef.h>

#include "../include/list.h"
   
 void init_list(_Out_ ListEntry* Head) {  
 	Head->Next = NULL;  
 	Head->IterCurrent = NULL;  
 	Head->Len = 0;  
 }  
   
 void push_front(  
 	_In_ ListEntry* Head,  
 	_In_ ListEntry* Entry) {  
   
 	struct ListEntry* temp = Head->Next;  
   
 	Entry->Next = NULL;  
 	Entry->IterCurrent = NULL;  
   
 	Head->Next = (struct ListEntry*)Entry;  
 	Entry->Next = temp;  
   
 	Head->Len++;  
 }  
   
 void* start_iter_list(_In_ ListEntry* Head) {  
 	Head->IterCurrent = Head->Next;  
 	return Head->IterCurrent;  
 }  
 void* iter_list_next(_In_ ListEntry* Head) {

     ListEntry* iterCurrent = 
         (ListEntry*)Head->IterCurrent;

 	Head->IterCurrent = iterCurrent->Next;
 	return Head->IterCurrent;  
 } 