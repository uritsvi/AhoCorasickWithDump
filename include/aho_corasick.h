#ifndef __AHO__CORASICK__
#define __AHO__CORASICK__

#include <sal.h>
#include <stdint.h>
#include <stdbool.h>

#include "..\include\list.h"

#define MAX_LEAVES 1 << 16

typedef int Leaves[MAX_LEAVES];


typedef struct{
	ListEntry Entry;
	size_t len;
	wchar_t path[];
} PathInfo;

typedef struct {
	struct BuildTrieEntry* Leaves[MAX_LEAVES];

	bool Root;

	wchar_t C;
	ListEntry Prefixes;
	struct BuildTrieEntry* FailureLink;

	int Index;
	int TravsFromRoot;

	bool OutputNode;
	int WordIndex;


}BuildTrieEntry;

typedef struct {
	Leaves Leaves;
	
	int FailureLink;
	
	wchar_t C;
	
	bool OutputNode;
	bool Root;

}FinalTrieEntry;


typedef struct {
	int I;
	int C;

}MatchContext;


void add_leaves(
	_In_ const wchar_t* Words,
	_In_ int NumWords,
	_Inout_ BuildTrieEntry* TrieEntry,
	_Out_ int* NumOfTries);

void make_final_trie(
	_In_ BuildTrieEntry* BuildEntry,
	_In_ int NumOfNodes,
	_Out_ FinalTrieEntry** FinalEntry,
	_Out_ int* BufferSize);

bool match(
	_In_ FinalTrieEntry* BaseAddr,
	_In_ FinalTrieEntry* Entry,
	_In_ const wchar_t* Str,
	_In_ MatchContext* Context);

#endif