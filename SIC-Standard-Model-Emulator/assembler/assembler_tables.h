/* 
	Irving Alvarez

	The structs and functions that define two hash tables for use during Pass 1 & Pass 2 of the assembler.
	These tables are the SYMTAB(symbol table) and the OPTAB(opcode table).
	Implemented using chaining with linked lists. OPTAB is static and SYMTAB is dynamic.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

// Useful data types
typedef unsigned int ADDRESS;

// Node struct used for the symtab singly linked list 
typedef struct symbolNode
{
	char symbol[32];
	ADDRESS address;
	struct symbolNode* next;

}snode_t;

// Node struct used for the optab singly linked list
typedef struct opcodeNode
{
	char mnemonic[10];
	char opcode[4];
	struct opcodeNode* next;

}onode_t;

// Hash structs used for hash tables
typedef struct s_hash
{
	snode_t* head;
	snode_t* tail;

}SYMBOLS;

typedef struct o_hash
{
	onode_t* head;
	onode_t* tail;

}OPCODES;

SYMBOLS* SYMTAB = NULL; // Hash tables
OPCODES* OPTAB = NULL;
int symtabElements = 0; // Current number of items in symbol table
int symtabCapacity = 503;
int optabCapcity = 27;

unsigned int hash(char* str);
snode_t* createSymbolNode(char* label, ADDRESS location);
onode_t* createOpcodeNode(char* name, char* op);
void insertSymbol(char* label, ADDRESS loc);
void insertOpcode(char* name, char* op);
int searchSymbol(char* label);
int searchOpcode(char* name);
void getOpcodeValue(int key, char* name, char* value);
void printSymbols();
void printOpcodes();
int empty(int numItems);

// Hash function
unsigned int hash(char* str)
{
	unsigned int value = 0;
	for(int i = 0; i < strlen(str); i++)
		value += ((str[i]*(i+1)) * 31);
	
	return value;
}
// Creates & returns a new node with passed data
snode_t* createSymbolNode(char* label, ADDRESS location)
{
	snode_t* newNode = malloc(sizeof(snode_t));

	strcpy(newNode->symbol, label);
	newNode->address = location;
	newNode->next = NULL;

	return newNode;
}
// Creates & returns a new node with passed data
onode_t* createOpcodeNode(char* name, char* op)
{
	onode_t* newNode = malloc(sizeof(onode_t));

	strcpy(newNode->mnemonic,name);
	strcpy(newNode->opcode,op);
	newNode->next = NULL;

	return newNode;
}
// Insert a new node(symbol) to the list
void insertSymbol(char* label, ADDRESS loc)
{
	unsigned int key = hash(label);
	key = key % symtabCapacity;

	snode_t* newNode = createSymbolNode(label, loc);

	// Insert first node in list
	if(!SYMTAB[key].head){
		SYMTAB[key].head = newNode;
		SYMTAB[key].tail = SYMTAB[key].head;
	}
	// Add node to the back
	else{
		SYMTAB[key].tail->next = newNode;
		SYMTAB[key].tail = newNode;

	}
	++symtabElements;
}
// Insert a new node(opcode) to the list
void insertOpcode(char* name, char* op)
{
	unsigned int key = hash(name);
	key = key % optabCapcity;

	onode_t* newNode = createOpcodeNode(name, op);

	// Insert first node in list
	if(!OPTAB[key].head){
		OPTAB[key].head = newNode;
		OPTAB[key].tail = OPTAB[key].head;
	}
	// Add node to the back
	else{
		OPTAB[key].tail->next = newNode;
		OPTAB[key].tail = newNode;

	}
}
// Sarches for the node(symbol) with the given label in the list. Returns 1 if found; 0 otherwise
int searchSymbol(char* label)
{
	unsigned int key = hash(label);
	key = key % symtabCapacity;

	snode_t* current = SYMTAB[key].head;

	while(current != NULL){

		if(strcmp(current->symbol,label) == 0)
			return TRUE;
		
		current = current->next;
	}
	return FALSE;
}
unsigned int getSymbolAddress(char* label, unsigned int* operandAddress)
{
	unsigned int key = hash(label);
	key = key % symtabCapacity;

	snode_t* current = SYMTAB[key].head;

	while(current != NULL){

		if(strcmp(current->symbol,label) == 0){
			*operandAddress = current->address;
			break;
		}
		current = current->next;
	}
}
// Searches for the node(mnemonic) with the given name in the list. returns 1 if found; 0 otherwise
int searchOpcode(char* name)
{
	unsigned int key = hash(name);
	key = key % optabCapcity;

	onode_t* explorer = OPTAB[key].head;

	while(explorer != NULL){

		if(strcmp(explorer->mnemonic,name) == 0)
			return key;
		
		explorer = explorer->next;
	}
	return -1;
}
// Saves the opcode in value
void getOpcodeValue(int key, char* name, char* value)
{
	onode_t* current = OPTAB[key].head;

	while(current != NULL){

		if(strcmp(current->mnemonic,name) == 0){
			strcpy(value,current->opcode);
			break;
		}
		current = current->next;
	}
}
// Return size of symtab
int getSymtabSize()
{
	return symtabCapacity;
}
// Displays all nodes(symbols) in the list, starting from head
void printSymbols()
{
	snode_t* current;

	printf("\nSYMBOL 	  ADDRESS\n");
	printf("--------------------\n");

	for(int i = 0; i < getSymtabSize(); i++){
		current = SYMTAB[i].head;
		while(current != NULL){
			printf("%s 	| %X\n", current->symbol, current->address);
			current = current->next;
		}
	}
	printf("\n");

}
// Displays all nodes(opcodes) in the list, starting from head
void printOpcodes()
{
	onode_t* current;

	printf("\nMNEMONIC | OPCODE\n");
	printf("--------------------\n");

	for(int i = 0; i < optabCapcity; i++){
		current = OPTAB[i].head;
		while(current != NULL){
			printf("%s 	| %s\n", current->mnemonic,current->opcode);
			current = current->next;
		}
	}

}
// Prints SMYTAB to given file
void printSymtabToFile(FILE* fp)
{
	snode_t* current;

	fprintf(fp, ".\n.	SYMBOL 	  	ADDRESS\n");
	fprintf(fp, ". ---------------------------------\n");

	for(int i = 0; i < getSymtabSize(); i++){
		current = SYMTAB[i].head;
		while(current != NULL){
			fprintf(fp, ".|	%-10s|	%-10X|\n", current->symbol, current->address);
			current = current->next;
		}
	}
	fprintf(fp, ". ---------------------------------\n");

}
// Returns 0 if table is empty
int empty(int numItems)
{
	return numItems;
}
// Returns number of elements in table
int symtabCount()
{
	return symtabElements;
}