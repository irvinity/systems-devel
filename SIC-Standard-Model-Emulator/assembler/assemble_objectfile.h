/*
	Irving Alvarez

	This file contains the functions that work collectively
	to generate the object source program from the assembly
	source file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void createObjFile();
void writeHeaderRecord(char* programName, char* startingAddress, char* programLength);
void writeObjectCode(char* objectCode);
void beginTextRecord(char* startingAddress, char* firstObjCode);
void writeEndRecord(char* startingAddress);
void processName(char* programName);
void processZeros(char* field);
int processLength(char* currentLcctr, char* previousLcctr, unsigned int length);
void deleteObjectFile();
void chomp(char* str);
int getDecimalValue(char* hex);

FILE* objectFile; // Object file pointer

// Creates the object file
void createObjFile()
{
	objectFile = fopen("generated_source/object", "w");
}
// Writes the header record for the object file
// Writes the starting address, program name, and program length
void writeHeaderRecord(char* programName, char* startingAddress, char* programLength)
{	
	// Remove newline character
	chomp(programName);
	chomp(startingAddress);
	chomp(programLength);

	// String length's
	int nameLen = strlen(programName);
	int addressLen = strlen(startingAddress);
	int progLen = strlen(programLength);

	// Write header character
	fprintf(objectFile, "H");

	// Write program name
	if(nameLen >= 6)
		fprintf(objectFile, "%s", programName);
	else
		processName(programName);

	// Write starting address
	if(addressLen >= 6){
		fprintf(objectFile, "%s", startingAddress);
	}else{
		processZeros(startingAddress);
		fprintf(objectFile, "%s", startingAddress);
	}

	// Write program length
	if(progLen >= 6){
		fprintf(objectFile, "%s", programLength);
	}else{
		processZeros(programLength);
		fprintf(objectFile, "%s", programLength);
	}

	fprintf(objectFile, "\n");

}
//  Prints spaces next to program name in object file
void processName(char* programName)
{
	int MAXLEN = 6;
	int spaces = 0;
	int nameLen = strlen(programName);

	// Determine space to write
	spaces = MAXLEN - nameLen;

	// Write program name
	fprintf(objectFile, "%s", programName);

	// Write required spaces
	for(int i = 0; i < spaces; i++)
		fprintf(objectFile, "\b");

}
// Places the exact amount of zeros before the specified field
void processZeros(char* field)
{
	int MAXLEN = 6; // Size of SIC instruction
	int zeros = 0;	// Zeros to be placed
	int fieldLen = strlen(field);
	int i = 0, j = 0;
	char temp[8];

	// Calculate zeros
	zeros = MAXLEN-fieldLen;

	// Add computed zeros to temp
	for(i = 0; i < zeros; i++)
		temp[i] = '0';

	// Copy the content of field to temp; after zeros
	for(j = i; j < MAXLEN; j++)
		temp[j] = field[j-i];

	// Copy result to field from temp
	temp[j] = '\0';
	strcpy(field,temp);
}
// Remove new line character from string
void chomp(char* str)
{
	int lastChar = strlen(str)-1;

	if(str[lastChar] == '\n')
		str[lastChar] = '\0';
	
}
// Writes new object code to current text record
void writeObjectCode(char* objectCode)
{
	fprintf(objectFile, "%s", objectCode);
}
// Starts a new text record in the object file
void beginTextRecord(char* startingAddress, char* firstObjCode)
{
	int addressLen = strlen(startingAddress);
	char locctrCopy[32];

	// Save original value
	strcpy(locctrCopy, startingAddress);

	fprintf(objectFile, "\nT");

	// Write starting address
	if(addressLen >= 6)
	{
		fprintf(objectFile, "%s", startingAddress);
	}else{

		processZeros(startingAddress);
		fprintf(objectFile, "%s", startingAddress);
		strcpy(startingAddress, locctrCopy);
	}
	// Write objectcode length (placeholders)
	fprintf(objectFile, "\b\b");

	// Write first object code
	fprintf(objectFile, "%s", firstObjCode);

}
// Computes the text record length
int processLength(char* currentLcctr, char* previousLcctr, unsigned int length)
{
	int currLcctr;
	int prevLcctr;
	int diff = 0;

	currLcctr = getDecimalValue(currentLcctr);
	prevLcctr = getDecimalValue(previousLcctr);

	diff = currLcctr-prevLcctr;
	length += diff;

	return length;
}
// Write last record in object file
void writeEndRecord(char* startingAddress)
{
	fprintf(objectFile, "\nE");
	fprintf(objectFile, "%s\n", startingAddress);
}
// Deletes the object file if source error flag is set
void deleteObjectFile()
{
	FILE* fp = fopen("generated_source/object","w");
}