#include <string.h>

#include "../include/aho_corasick.h"
#include "../include/hash_table.h"
#include "../include/lib.h"

#define INT_NULL_VALUE -100

typedef struct {
    int NumOfIndexes;
    HashTable Prefixes;
}FullContext;

typedef struct {
    FullContext* FullContex;

    int TravsFromRoot;
    size_t CurrentMaxLen;
}WordContext;

typedef struct{
    ListEntry Entry;
    wchar_t Prefix[1];
} Prefix;

typedef struct{
    ValueEntry Entry;
    BuildTrieEntry* TrieEntry;
} MapValue;

static void handle_word(
    _In_ wchar_t* Buffer,
    _In_ int Index,
    _In_ BuildTrieEntry* Trie,
    _In_ WordContext* WordContext,
    _In_ int WordIndex) {

    WordContext->TravsFromRoot++;

    BuildTrieEntry* target_entry =
        (BuildTrieEntry*)Trie->Leaves[Buffer[Index]];

    if (target_entry == NULL) {
        
        target_entry =
            (BuildTrieEntry*)
            lib_malloc(sizeof(BuildTrieEntry));

        memset(
            target_entry,
            0,
            sizeof(BuildTrieEntry));

        init_list(&target_entry->Prefixes);

        target_entry->C = Buffer[Index];
        target_entry->TravsFromRoot = WordContext->TravsFromRoot;
        target_entry->OutputNode = false;
        target_entry->Index = WordContext->FullContex->NumOfIndexes++;


        int c = WordContext->TravsFromRoot - 1;
        do {
            Prefix* prefix = (Prefix*)lib_malloc(
                sizeof(Prefix) +
                (WordContext->TravsFromRoot) * sizeof(wchar_t)
            );

            memset(
                prefix,
                0,
                sizeof(Prefix) +
                (WordContext->TravsFromRoot) * sizeof(wchar_t));


            memcpy_s(
                (void*)prefix->Prefix,
                (WordContext->TravsFromRoot * sizeof(wchar_t)),
                Buffer + 1,
                (c) * sizeof(wchar_t));

            push_front(
                &target_entry->Prefixes,
                &prefix->Entry);


        } while (--c >= 0);


        MapValue* value = (MapValue*)lib_malloc(
            sizeof(MapValue)
        );

        value->TrieEntry = target_entry;

        push_value_to_map(
            &WordContext->FullContex->Prefixes,
            Buffer,
            WordContext->TravsFromRoot,
            &value->Entry);

        Trie->Leaves[Buffer[Index]] = (struct BuildTrieEntry*)target_entry;
    }

    if (Index == WordContext->CurrentMaxLen - 1) {
        target_entry->WordIndex = WordIndex;
        target_entry->OutputNode = true;
        return;
    }

    handle_word(
        Buffer,
        Index + 1,
        target_entry,
        WordContext,
        WordIndex);
}

static bool handle_prefix(
    _In_ FullContext* Context,
    _In_ ListEntry* Head,
    _In_ Prefix* Current,
    _Inout_ BuildTrieEntry* CurrentLeaf) {

    if (Current != NULL) {
        bool res = handle_prefix(
            Context,
            Head,
            iter_list_next(Head),
            CurrentLeaf);
        if (res) {
            return res;
        }

        MapValue* entry;
        res = query_map_value(
            &Context->Prefixes,
            Current->Prefix,
            wcslen(Current->Prefix),
            (ValueEntry**)&entry);

        if (!res) {
            return false;
        }

        CurrentLeaf->FailureLink =
            (struct BuildTrieEntry*)entry->TrieEntry;

        return true;
    }

    return false;
}

static void handle_make_faliure_links(
    _In_ BuildTrieEntry* Trie,
    _In_ FullContext* Context) {

    ListEntry* prefixes = NULL;
    for (int i = 0; i < MAX_KEYS; i++) {
        if (Trie->Leaves[i] == NULL) {
            continue;
        }

        BuildTrieEntry* current = 
            (BuildTrieEntry*)Trie->Leaves[i];
        
        prefixes = &current->Prefixes;

        handle_prefix(
            Context,
            prefixes,
            (Prefix*)start_iter_list(prefixes),
            current);

        handle_make_faliure_links(
            (BuildTrieEntry*)Trie->Leaves[i],
            Context);

    }

    if (prefixes == NULL) {
        return;
    }

}

/*
* ';' Seperator between words
*/
void add_leaves(
    _In_ const wchar_t* Words,
    _In_ int NumWords,
    _Inout_ BuildTrieEntry* TrieEntry,
    _Out_ int* NumOfTries){

    *NumOfTries = 0;

    size_t words_buffer_len = 
        (wcslen(Words) + 1);

    wchar_t* words_buffer = 
        (wchar_t*)lib_malloc(words_buffer_len * sizeof(wchar_t));
    
    wcscpy_s(
        words_buffer, 
        words_buffer_len, 
        Words);

    
    FullContext* full_context = 
        lib_malloc(sizeof(FullContext));
    
    full_context->NumOfIndexes = 1;
    
    
    init_map(
        &full_context->Prefixes, 
        true);
    
    wchar_t* wcsstokContext = {0};
    wchar_t* current = wcstok_s(
        words_buffer,
        L";",
        &wcsstokContext);

    int c = 0;
    while (current != NULL)
    {
        WordContext word_context = { 0 };
        word_context.FullContex = full_context;
        word_context.CurrentMaxLen = wcslen(current);
        
        handle_word(
            current, 
            0, 
            TrieEntry, 
            &word_context,
            c++);
            
        current = wcstok_s(
            NULL, 
            L";", 
            &wcsstokContext);
    }

    handle_make_faliure_links(
        TrieEntry, 
        full_context);
    
    TrieEntry->Root = true;
    *NumOfTries = full_context->NumOfIndexes;
    
}

static void convert_node_to_final(
    _In_ BuildTrieEntry* BuildEntry, 
    _In_ void* Buffer) {

    FinalTrieEntry* entry = 
       (FinalTrieEntry*)((char*)Buffer + (BuildEntry->Index * sizeof(FinalTrieEntry)));

    for (int i = 0; i < MAX_LEAVES; i++) {
        entry->Leaves[i] = INT_NULL_VALUE;
    }

    BuildTrieEntry* failure = 
        (BuildTrieEntry*)BuildEntry->FailureLink;

    entry->C = BuildEntry->C;
    entry->OutputNode = BuildEntry->OutputNode;
    entry->Root = BuildEntry->Root;
  
    entry->FailureLink = 0;
    if (failure != NULL) {
        entry->FailureLink = failure->Index;

    }

    for (int i = 0; i < MAX_LEAVES; i++) {
        if (BuildEntry->Leaves[i] == NULL) {
            continue;
        }

        BuildTrieEntry* leaf = 
            (BuildTrieEntry*)BuildEntry->Leaves[i];
        
        entry->Leaves[i] = leaf->Index;

        convert_node_to_final(
            (BuildTrieEntry*)BuildEntry->Leaves[i], 
            Buffer);


    }
    

}

void make_final_trie(
    _In_ BuildTrieEntry* BuildEntry,
    _In_ int NumOfNodes,
    _Out_ FinalTrieEntry** FinalEntry,
    _Out_ int* BufferSize) {

    *BufferSize = NumOfNodes * sizeof(FinalTrieEntry);
    *FinalEntry = lib_malloc(*BufferSize);

    memset(
        *FinalEntry, 
        0, 
        *BufferSize);

    convert_node_to_final(
        BuildEntry, 
        *FinalEntry);

    

    return;
}

bool match(
    _In_ FinalTrieEntry* BaseAddr,
	_In_ FinalTrieEntry* Entry,
	_In_ const wchar_t* Str,
    _In_ MatchContext* Context) {

    size_t len = wcslen(Str);
    if (Context->I == len || Entry == NULL) {
        return false;
    }

    int next_index =
        Entry->Leaves[Str[Context->I]];
    
    FinalTrieEntry* next;

    if (next_index != INT_NULL_VALUE) {

        next = 
            (FinalTrieEntry*)(
                (char*)BaseAddr + 
                (next_index * sizeof(FinalTrieEntry)));

        if (next->OutputNode == true) {
            return true;
        }

        Context->I++;

    }
    else {
        next_index =
            Entry->FailureLink;
        next =
            (FinalTrieEntry*)(
                (char*)BaseAddr + 
                (next_index * sizeof(FinalTrieEntry)));

        if (Entry->Root) {

            Context->I++;
            next = Entry;
        }
    }

    bool res = match(
        BaseAddr,
        next,
        Str,
        Context);

	return res;
}