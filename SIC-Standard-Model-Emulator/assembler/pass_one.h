/*
	Irving Alvarez

	Pass one of the assembler

	This file creates the Opcode Table and reads the assembly source
	code line by line to create the Symbol Table, it also creates the
	intermediate file that will be used for the second pass of the
	assembler, and the listing file for logging.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler_tables.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// Useful data types
typedef unsigned int ADDRESS;
typedef unsigned char BYTE;

// OPCODES 
struct opcodes
{
	char mnemonic[8];
	char opcode[4];

}opcodes[] = {
	{"ADD", "18"}, 
	{"AND", "58"}, 
	{"COMP", "28"}, 
	{"DIV", "24"}, 
	{"J", "3C"},  
	{"JEQ", "30"}, 
	{"JGT", "34"}, 
	{"JLT", "38"},
    {"JSUB", "48"}, 
	{"LDA", "00"}, 
	{"LDCH", "50"}, 
	{"LDL", "08"},
    {"LDX", "04"}, 
	{"MUL", "20"}, 
	{"OR", "44"}, 
	{"RD", "D8"},
    {"RSUB", "4C"}, 
	{"STA", "0C"}, 
	{"STCH", "54"}, 
	{"STL", "14"},
    {"STX", "10"}, 
	{"SUB", "1C"}, 
	{"TD", "E0"}, 
	{"TIX", "2C"},
    {"WD", "DC"}
};

// Error messages(codes)
struct message
{
	char labelErr[4];
	char opcodeErr[4];
	char operandErr[4];

}errors[500];

int SIZE = 25;  // # of Opcodes
ADDRESS initAdress; // Starting address
ADDRESS LOCCTR; // Location counter
unsigned int LENGTH; // Program length

// File pointers
FILE* asmSource;	
FILE* imtFile;		 
FILE* dataFile;

// Flag variables for pass 1
int validOpcode, validSymbol, validOperand;

void openFile(char* file);
void loadOpcodes();
void passOne();
void readLine(char* line, char* label, char* opcode, char* operand);
void breakLine(char* line, char* label, char* opcode, char* operand);
int checkSymbol(char* label);
int searchSymbol(char* label);
void insertSymbol(char* label, ADDRESS location);
int searchOpcode(char* mnemonic);
void insertOpcode(char* mnemonic, char* op);
int checkOperand(char* operand);
int checkByteOperand(char* operand);
int checkStartOperand(char* operand);
int checkByteOperandFormat(char* operand);
void getOpcodeValue(int key, char* name, char* value);
int symtabCount();
void printSymbols();
void printSymtabToFile(FILE* fp);
int getDecValue(char* hex);
void removeNewLine(char* str);

void passOne(char* file)
{
	char label[5], opcode[4], operand[32];
	char opcodeVal[4];
	char line[1024]; // Current line from source file
	int symbol; // Flag for duplicate symbols
	int LC = 0;
	int i = 0; // Iterator
	int decValue = 0;
	char firstLabel[16];

	// Create tables for assembler
	SYMTAB = calloc(503, sizeof(struct s_hash));
	OPTAB = calloc(27, sizeof(struct o_hash));

	// Open assembler source file
	openFile(file);

	// Load all opcodes to OPTAB
	loadOpcodes();
	//printOpcodes();

	// If valid source file, begin pass 1 of the assembler
	if(asmSource != NULL){
		
		// Read first line
		readLine(line, label, opcode, operand);

		if(strcmp("START", opcode) == 0){

			validOperand = checkStartOperand(operand);

			// Initialize initial program address & LOCCTR
			if(validOperand == FALSE){
				strcpy(errors[i].operandErr,"0OP");
				initAdress = 0;
				LOCCTR = initAdress;
			}else{
				decValue = getDecValue(operand);
				initAdress = decValue;
				LOCCTR = initAdress;
			}
		}else{
			LOCCTR = 0; // Initialize LOCCTR to 0
		}
		
		// Write line to intermediate file, read next line
		fprintf(imtFile, "%X:%s:%s:%s;%s:%s:%s:\n", LOCCTR, label, opcode, operand, errors[i].labelErr, errors[i].opcodeErr, errors[i].operandErr);
		readLine(line, label, opcode, operand);
		i += 1;

		// Repeat until the end of file
		while(strcmp("END", opcode) != 0 && !feof(asmSource))
		{
			LC = LOCCTR;

			// If there is a symbol in the LABEL field...
			if(strcmp("\0", label) != 0){
				validSymbol = checkSymbol(label);

				if(validSymbol == FALSE) // Flag: Invalid label
					strcpy(errors[i].labelErr, "0IL");
				
				// Search SYMTAB for LABEL
				symbol = searchSymbol(label);

				if(symbol) // Flag: Duplicate label
					strcpy(errors[i].labelErr, "0DL");
				else
					insertSymbol(label, LOCCTR);
			}

			// Search OPTAB for OPCODE
			validOpcode = searchOpcode(opcode);

			// Increment LOCCTR according to opcode
			if(validOpcode >= 0){
				getOpcodeValue(validOpcode, opcode, opcodeVal); // Save opcode value in 'opcodeVal'
				LOCCTR += 3;
			}else if(strcmp("WORD", opcode) == 0){
				LOCCTR += 3;
			}else if(strcmp("RESW", opcode) == 0){
				if(atoi(operand) != 0)
					LOCCTR += (3*atoi(operand));
			}else if(strcmp("RESB", opcode) == 0){
				LOCCTR += atoi(operand);
			}else if(strcmp("BYTE", opcode) == 0){
				// Find length of constant in bytes then add its length to LOCCTR
				int byteLength = checkByteOperandFormat(operand);
				if(byteLength == 0)
					strcpy(errors[i].operandErr, "0BO");
				
				LOCCTR += byteLength;
			}else{
				// Invalid opcode
				strcpy(opcodeVal, "\0");
				strcpy(errors[i].opcodeErr, "0O");
			}
			
			// Check operand
			if(strcmp("BYTE", opcode) != 0){
				validOperand = checkOperand(operand);
				if(validOperand == FALSE)
					strcpy(errors[i].operandErr, "0OP");
			}

			// Write to intermediate file
			if(line[0] != '.')
				fprintf(imtFile, "%X:%s:%s:%s:%s;%s:%s:%s:\n", LC, label, opcode, operand, opcodeVal, errors[i].labelErr, errors[i].opcodeErr, errors[i].operandErr);
			else
				fprintf(imtFile, "%s", line);
			

			i += 1;
			memset(opcodeVal, '\0', sizeof(opcodeVal));
			// Read next line
			readLine(line, label, opcode, operand);

		}

		// Write the last line to the intermediate file
		fprintf(imtFile, "%X:%s:%s:%s;%s:%s:%s:\n", LOCCTR, label, opcode, operand, errors[i].labelErr, errors[i].opcodeErr, errors[i].operandErr);
		LENGTH = (LOCCTR - initAdress); // Save program length
		// Write the SYMTAB to the intermediate file
		printSymtabToFile(imtFile);

		// Write remaining errors to file, if any
		int totalSymbols = symtabCount();
		if(totalSymbols > 500)
			fprintf(imtFile, "!0SS\n");
		
		/* More error checks here */
		
		// Write the program length to a temp file
		fprintf(dataFile, "%X\n", LENGTH);
	}

	printf("Pass One: Success!\n");
	fclose(imtFile);
	fclose(dataFile);

}// End pass one

// Opens the source file, the intermediate file and the data file
void openFile(char* file)
{
	// Open file for reading
	asmSource = fopen(file, "r");

	// If file not found
	if(asmSource == NULL){
		printf("Error: Cannot open file '%s'\n", file);
		return;
	}

	// Open file for writing
	imtFile = fopen("generated_source/intermediate", "w");
	dataFile = fopen("generated_source/program_length", "w");

}

// Reads a line from the source file and breaks it into tokens (label, opcode, operand)
void readLine(char* line,char* label, char* opcode, char* operand)
{
	char buf[1024];
	int c;

	if(fgets(buf, 1024, asmSource) != NULL){
		
		// Ignore comment line
		if(buf[0] == '.'){
			strcpy(line,buf);
			memset(label, '\0', sizeof(label));
			memset(opcode, '\0', sizeof(opcode));
			memset(operand, '\0', sizeof(operand));

		}
		// Break line into tokens
		else{
			strcpy(line, buf);
			breakLine(buf, label, opcode, operand);
		}
	}
}
// Breaks line into tokens (label, opcode, operand)
void breakLine(char* line, char* label, char* opcode, char* operand)
{
	int tokenCtr = 1; // Number of current tokens
	char* pch;
	pch = strtok(line, " 	");

	// No label
	if(line[0] == '\b' || line[0] == '\t'){

		strcpy(label,"\0");

		while(pch != NULL && tokenCtr <= 2)
		{
			if(tokenCtr == 1)
				strcpy(opcode, pch);
			if(tokenCtr == 2)
				strcpy(operand, pch);
			pch = strtok(NULL, " 	");
			++tokenCtr;
		}

	}
	// Label
	else{
		while(pch != NULL && tokenCtr <= 3)
		{
			if(tokenCtr == 1)
				strcpy(label, pch);
			if(tokenCtr == 2)
				strcpy(opcode, pch);
			if(tokenCtr == 3)
				strcpy(operand, pch);
			
			pch = strtok(NULL, " 	");
			++tokenCtr;
		}
	}
	
	removeNewLine(label);
	removeNewLine(opcode);
	removeNewLine(operand);

}
void removeNewLine(char* str)
{
	if(str[strlen(str) - 1] == '\n')
		str[strlen(str) - 1] = '\0';
}
// Inserts all opcodes to the OPTAB hash table
void loadOpcodes()
{
	for(int i = 0; i < SIZE; i++)
		insertOpcode(opcodes[i].mnemonic, opcodes[i].opcode);
}
// Converts a hexadecimal string to its decimal equivalent
int getDecValue(char* hex)
{

	int base = 1;
	int decimalVal = 0;

	int len = strlen(hex);

	for(int i = len-1; i >= 0; i--){

		// If char is a digit
		if(hex[i] >= '0' && hex[i] <= '9'){
			decimalVal += (hex[i] - 48) * base;
			base = base * 16;
		}
		// Is char is between A-F
		else if(hex[i] >= 'A' && hex[i] <= 'F'){
			decimalVal += (hex[i] - 55) * base;
			base = base * 16;
		}
	}

	return decimalVal;
}

