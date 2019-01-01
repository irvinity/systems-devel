/*
	Irving Alvarez

	Pass two of the assembler

	This file creates the listing file and the object file.
	It assembles the object code and operand address together to create a SIC instruction, and
	converts constant operands to its machine equivalent.
*/

#include <stdio.h>
#include <string.h>
#include "assemble_instructions.h"
#include "assemble_objectfile.h"

#ifndef TRUE
#define TRUE 1;
#endif
#ifndef FALSE
#define FALSE 0;
#endif

// Useful data types
typedef unsigned int ADDRESS;
typedef unsigned char BYTE;

// File pointers
char file[16] = "intermediate";
FILE* imtSource;
FILE* listingFile;
FILE* dataFile;

void openFiles();
void readImtFile(char* line, char* locctr, char* label, char* opcode, char* operand, char* opcodeValue, char* labelErr, char* opcodeErr, char* operandErr, char* objectCode);
void splitLine(char* line, char* locctr, char* label, char* opcode, char* operand, char* opvalue, char* labelErr, char* opcodeErr, char* operandErr);
void splitLinePt2(int itr, char* line, char* labelErr, char* opcodeErr, char* operandErr);
void clearObjectCode(char* objectCode);
void resetStrings(char* locctr, char* label, char* opcode, char* operand, char* opcodeValue);
void resetStringsPt2(char* labelErr, char* opcodeErr, char* operandErr);
int checkError(char* labelErr, char* opcodeErr, char* operandErr);
void setClock(int* clock);
int checkSourceLine(char* line, char* objectCode);
int getDecimalValue(char* hex);

void passTwo()
{
	// String arrays used to store source data from intermediate file
	BYTE locctr[32],label[32], opcode[32], operand[32], opcodeValue[32];
	BYTE labelErr[10],opcodeErr[10],operandErr[10];
	
	// Useful variables for listing file creation
	ADDRESS operandAddress = 0;
	BYTE constantValue[8];
	BYTE objectCode[8];

	// Useful variables for object file creation
	char line[1024];
	char startingAddress[32];
	char operandCopy[16];
	char prevLcctr[10];
	char programLength[8];
	char length[8];
	unsigned int objCodeLen = 0;
	int clock = 0;

	// Flag variables
	int validOperandSymbol;
	int indexBitState;
	int errorState = 0;

	// Open intermediate file & create listing
	openFiles();

	if(imtSource != NULL){

		fprintf(listingFile, ".%-9s %-10s %-10s %-10s %-15s %s\n\n", "LOCATION", "LABEL", "OPCODE", "OPERAND", "OBJECT CODE", "ERRORS");

		/* BEGIN PASS 2 */

		// Read first input line from inter. file
		readImtFile(line, locctr, label, opcode, operand, opcodeValue, labelErr, opcodeErr, operandErr, objectCode);
		strcpy(startingAddress, operand);

		// Get program length from dataFile
		fgets(programLength, 32, dataFile);

		// Create header record for object file
		writeHeaderRecord(label, startingAddress, programLength);

		// If opcode is START
		if(strcmp("START", opcode) == 0){

			// Write first line to listing & read next line
			fprintf(listingFile, "%-10s %-10s %-10s %-10s %-15s %-5s %-5s %s\n", locctr, label, opcode, operand, opcodeValue, labelErr, opcodeErr, operandErr);
			readImtFile(line, locctr, label, opcode, operand, opcodeValue, labelErr, opcodeErr, operandErr, objectCode);

			objCodeLen = 3;
		}
		// Repeat until end of source file
		while(strcmp("END", opcode) != 0){
		
				// If opcode exists
				if(opcodeValue[0] != '\0'){

					// If there is a symbol in operand field
					if(operand[0] != '\0'){

						strcpy(operandCopy, operand);

						// Check for index addressing
						indexBitState = checkIndexBit(operand);

						// Search symbol in SYMTAB
						validOperandSymbol = searchSymbol(operand);

						// If symbol found, set symbol value as operand address;
						// otherwise set symbol value to 0
						if(validOperandSymbol == 1)
							getSymbolAddress(operand, &operandAddress);
						else
							operandAddress = 0;

					}else{
						// Store 0 as operand address
						zeroOperandAddress(opcodeValue, objectCode);
					}
					
					// Assemble the object code instruction
					if(indexBitState == 1){
						setIndexAddressing(opcodeValue, operandAddress, objectCode);
						strcpy(operand, operandCopy);
					}else{
						assembleInstruction(opcodeValue, operandAddress, objectCode);
					}

				}
				// If opcode is BYTE or WORD
				else if(strcmp("BYTE", opcode) == 0){
					convertByteOprConstant(operand, constantValue);
					strcpy(objectCode, constantValue);
				}
				else if(strcmp("WORD",opcode) == 0){
					convertWordOprConstant(operand, constantValue);
					strcpy(objectCode, constantValue);
				}
				// Check for errors in current line
				errorState = checkError(labelErr, opcodeErr, operandErr);
				
				/* Write object file */

				// Write first object record
				if(clock == 0){
					beginTextRecord(locctr, objectCode);	
				}else if(clock > 0 && clock  < 10){
					// Write object code to current text record
					int validLine = checkSourceLine(line, objectCode);
					writeObjectCode(objectCode);

					if(validLine == 1)
						objCodeLen = processLength(locctr, prevLcctr, objCodeLen);

				}else if(clock == 10){
					// Start new text record & reset variables
					beginTextRecord(locctr, objectCode);
					clock = 0;
					objCodeLen = 0;
					memset(length, '\0', sizeof(length));
				}
				
				// Increment clock
				setClock(&clock);

				// Copy current locctr to prevlocctr
				if(line[0] != '.' || locctr[0] != '\0'){
					memset(prevLcctr, '\0', sizeof(prevLcctr));
					strcpy(prevLcctr, locctr);
				}

				// Write to listing file & 
				fprintf(listingFile, "%-10s %-10s %-10s %-10s %-15s %-5s %-5s %s\n", locctr, label, opcode, operand, objectCode, labelErr, opcodeErr, operandErr);
				// Read next line from intermediate file
				readImtFile(line, locctr, label, opcode, operand, opcodeValue, labelErr, opcodeErr, operandErr, objectCode);
				clearObjectCode(objectCode);
		}

		// Write end record to object program
		writeEndRecord(startingAddress);

		// Write last listing line
		fprintf(listingFile, "%-10s %-10s %-15s %-5s %-5s %s\n", label, opcode, operand, labelErr, opcodeErr, operandErr);

		// Print symtab to listing file
		printSymtabToFile(listingFile);

		// Delete object file if there were any errors in source
		if(errorState == 1)
			deleteObjectFile();

		printf("Pass Two: Success!\n");
		fclose(listingFile);
		fclose(objectFile);
		fclose(dataFile);
	}

}// End pass two

// Open intermediate file
void openFiles()
{
	// Open files for reading
	imtSource = fopen("generated_source/intermediate", "r");
	dataFile = fopen("generated_source/program_length", "r");

	// If file not found
	if(imtSource == NULL)
		printf("Error: Cannot open file '%s'\n", file);
	
	// Create file for writing
	listingFile = fopen("generated_source/listing", "w");
	createObjFile();

}
// Read the current/new line from the intermediate file
void readImtFile(char* line, char* locctr, char* label, char* opcode, char* operand, char* opcodeValue, char* labelErr, char* opcodeErr, char* operandErr, char* objectCode)
{
	char buf[1024];

	if(fgets(buf,1024,imtSource) != NULL){

		buf[strlen(buf) - 1] = '\0';

		// If line is not a comment, split line
		if(buf[0] != '.'){	 
			splitLine(buf, locctr, label, opcode, operand, opcodeValue, labelErr, opcodeErr, operandErr);
		}else{
			fprintf(listingFile, "%s", buf);
			strcpy(line,buf);
			resetStrings(locctr, label, opcode, operand, opcodeValue);
			resetStringsPt2(labelErr, opcodeErr, operandErr);
			clearObjectCode(objectCode);
		}
	}
}
void clearObjectCode(char* objectCode)
{
	memset(objectCode, '\0', sizeof(objectCode));
}
void resetStrings(char* locctr, char* label, char* opcode, char* operand, char* opcodeValue)
{
	memset(locctr, '\0', sizeof(locctr));
	memset(label, '\0', sizeof(label));
	memset(opcode, '\0', sizeof(opcode));
	memset(operand, '\0', sizeof(operand));
	memset(opcodeValue, '\0', sizeof(opcodeValue));

}
void resetStringsPt2(char* labelErr, char* opcodeErr, char* operandErr)
{
	memset(labelErr, '\0', sizeof(labelErr));
	memset(opcodeErr, '\0', sizeof(opcodeErr));
	memset(operandErr, '\0', sizeof(operandErr));
}
// Starts splitting the line read from intermediate file
// it cuts the location counter, label, opcode, operand, and opcode value
void splitLine(char* line, char* locctr, char* label, char* opcode, char* operand, char* opvalue, char* labelErr, char* opcodeErr, char* operandErr)
{	
	int itr = 0;
	int jtr = 0;
	char endingChar = ';';
	char processed = '*';
	int lastChar_lc = 0, lastChar_lb = 0, lastChar_oc = 0, lastChar_op = 0, lastChar_ov = 0;

	resetStrings(locctr, label, opcode, operand, opvalue);
	resetStringsPt2(labelErr, opcodeErr, operandErr);

	// Read line until ';' is seen
	while(line[itr] != endingChar)
	{
		// Locctr
		if(locctr[strlen(locctr) - 1] != processed){
			if(line[itr] == ':'){
				locctr[jtr] = processed;
				jtr = 0;
			}else{
				locctr[jtr] = line[itr];
				++jtr;
				lastChar_lc = jtr;
			}
		}
		// Label
		else if(label[strlen(label) - 1] != processed){
			if(line[itr] == ':'){
				label[jtr] = processed;
				jtr = 0;
			}else{
				label[jtr] = line[itr];
				++jtr;
				lastChar_lb = jtr;
			}
		}
		// Opcode
		else if(opcode[strlen(opcode) - 1] != processed){
			if(line[itr] == ':'){
				opcode[jtr] = processed;
				jtr = 0;
			}else{
				opcode[jtr] = line[itr];
				++jtr;
				lastChar_oc = jtr;
			}
		}
		// Operand
		else if(operand[strlen(operand) - 1] != processed){
			if(line[itr] == ':'){
				operand[jtr] = processed;
				jtr = 0;
			}else{
				operand[jtr] = line[itr];
				++jtr;
				lastChar_op = jtr;
			}
		}
		// Opval
		else if(opvalue[strlen(opvalue) - 1] != processed){
			if(line[itr] == ';'){
				opvalue[jtr] = processed;
				jtr = 0;
			}else{
				opvalue[jtr] = line[itr];
				++jtr;
				lastChar_ov = jtr;
			}
		}
		++itr;
	}

	locctr[lastChar_lc] = '\0';
	label[lastChar_lb] = '\0';
	opcode[lastChar_oc] = '\0';
	operand[lastChar_op] = '\0';
	opvalue[lastChar_ov] = '\0';
	

	splitLinePt2(++itr, line, labelErr, opcodeErr, operandErr);
}
// Splits the rest of the line read from intermdiate file
// it cuts the errors from the line, if any
void splitLinePt2(int itr, char* line, char* labelErr, char* opcodeErr, char* operandErr)
{
	char processed = '*';
	int jtr = 0;
	int i = itr;
	int lastChar_lb = 0, lastChar_oc = 0, lastChar_op = 0;

	while(line[i] != '\0'){

		if(labelErr[strlen(labelErr) - 1] != processed){
			if(line[i] == ':'){
				labelErr[jtr] = processed;
				jtr = 0;
			}else{
				labelErr[jtr] = line[i];
				++jtr;
				lastChar_lb = jtr; 
			}
		}
		else if(opcodeErr[strlen(opcodeErr) - 1] != processed){
			if(line[i] == ':'){
				opcodeErr[jtr] = processed;
				jtr = 0;
			}else{
				opcodeErr[jtr] = line[i];
				++jtr;
				lastChar_oc = jtr;
			}
		}
		else if(operandErr[strlen(operandErr) - 1] != processed){
			if(line[i] == ':'){
				operandErr[jtr] = processed;
				jtr = 0;
			}else{
				operandErr[jtr] = line[i];
				++jtr;
				lastChar_op = jtr;
			}
		}
		++i;
	}

	labelErr[lastChar_lb] = '\0';
	opcodeErr[lastChar_oc] = '\0';
	operandErr[lastChar_op] = '\0';
}
// Returns 1(TRUE) if any error is found; 0(FALSE) otherwise
int checkError(char* labelErr, char* opcodeErr, char* operandErr)
{
	if(labelErr[0] != '\0')	
		return 1;
	else if(opcodeErr[0] != '\0')
		return 1;
	else if(operandErr[0] != '\0')
		return 1;
	else
		return 0;
}
// Converts a hexadecimal string to its decimal equivalent
int getDecimalValue(char* hex)
{
	int base = 1;
	int decimalVal = 0;

	int len = strlen(hex);

	for(int i=len-1; i >= 0; i--){

		// If hex[i] is a digit
		if(hex[i] >= '0' && hex[i] <= '9'){
			decimalVal += (hex[i] - 48) * base;
			base = base * 16;
		}
		// If hex[i] is between A-F
		else if(hex[i] >= 'A' && hex[i] <= 'F'){
			decimalVal += (hex[i] - 55) * base;
			base = base * 16;
		}
	}

	return decimalVal;
}
// Increment the clock by one
void setClock(int* clock)
{
	*clock = *clock+1;
}
// Returns FALSE(0) if line is a comment line, or object code is empty
int checkSourceLine(char* line, char* objectCode)
{
	if(line[0] == '.')
		return FALSE;
	
	if(objectCode[0] == '\0'){
		return FALSE;
	}else{
		return TRUE;
	}
}