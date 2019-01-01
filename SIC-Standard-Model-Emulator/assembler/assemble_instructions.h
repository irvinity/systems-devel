/*
	Irving Alvarez

	This file contains the functions that construct the SIC
	instructions in the proper format according to its rules.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void convertByteOprConstant(char* operand, char* constantValue);
void convertWordOprConstant(char* operand, char* constantValue);
void assembleInstruction(char* opcodeValue, unsigned int operandAddress, char* objectCode);
int checkIndexBit(char* operand);
void setIndexAddressing(char* opcodeValue, unsigned int operandAddress, char* objectCode);
void zeroOperandAddress(char* opcodeValue, char* objectCode);

// Converts the BYTE operand constant to its hexadecimal equivalent
// and copies the result to the 'constantValue' array
void convertByteOprConstant(char* operand, char* constantValue)
{
	int i = 2, j = 0; // Iterators
	BYTE charString[30], hexValue[30];
	memset(constantValue, '\0', sizeof(constantValue));

	// If operand format is X'..'
	if(operand[0] == 'X'){
		
		// Copy hex value
		for(i = 2; i < strlen(operand)-1; i++){
			constantValue[j] = operand[i];
			++j;
		}
	}
	// If operand format is C'..'
	else if(operand[0] == 'C'){
		// Copy operand value to charString; skip C'
		for(i = 2; i < strlen(operand)-1; i++){
			charString[j] = operand[i];
			++j;
		}
		charString[j] = '\0';

		// Save hex equivalent to hexvalue from charString, and add it to constantValue
		for(i = 0; i < strlen(charString); i++){
			sprintf(hexValue, "%X", charString[i]);
			strcat(constantValue, hexValue);
		}
	}
}
// Converts the WORD operand constant to its hexadecimal equivalent
// and adds any leading zeros, if required, then copies the final result to
// the constantValue array
void convertWordOprConstant(char* operand, char* constantValue)
{
	unsigned int decimalNum = atoi(operand);
	char hexStringValue[32], tempHexStringValue[32];
	char* ptr;
	int operandLen, zeros;
	int totalLen;
	int i = 0, j = 0, k = 0; // Iterators
	memset(constantValue, '\0', sizeof(constantValue));

	// Save HEX constant value to hexStringValue
	sprintf(hexStringValue, "%X", decimalNum);

	operandLen = strlen(hexStringValue);

	if(operandLen == 6){
		strcpy(constantValue, hexStringValue);
	}else{

		zeros = 6 - operandLen;
		totalLen = zeros + operandLen; // Length of constant value

		// Add leading zeros to constantValue (new temp array)
		for(i = 0; i < zeros; i++)
			tempHexStringValue[i] = '0';

		// Transfer actual value to previously temp array
		for(j = zeros; j < totalLen; j++){
			tempHexStringValue[j] = hexStringValue[k];
			++k;
		}

		// Copy value of temp array to final destination(constantValue)
		strcpy(constantValue, tempHexStringValue);
	}

}
// Assembles the values of opcodeValue and operandAddress into
// the objectCode array to form the instruction
void assembleInstruction(char* opcodeValue, unsigned int operandAddress, char* objectCode)
{
	char tempObjCode[10];
	char operandAddressString[10];

	int opcodeSize = strlen(opcodeValue);
	int operandAddressSize, objectCodeSize; // String length's
	int i = 0, j = 0; // Iterators

	// If opcode is RSUB
	if(strcmp("4C",opcodeValue) == 0){
		zeroOperandAddress(opcodeValue, objectCode);
	}else{

		// Copy opcodeValue to temp
		for(i = 0; i < opcodeSize; i++)
			tempObjCode[i] = opcodeValue[i];

		// Convert int(operandAddress) to  a string
		sprintf(operandAddressString, "%X", operandAddress);
		operandAddressSize = strlen(operandAddressString);
		
		// Determine object code length
		objectCodeSize = opcodeSize + operandAddressSize;

		// Copy operandAddressString to temp
		for(j = i; j < objectCodeSize; j++)
			tempObjCode[j] = operandAddressString[j-i];

		tempObjCode[objectCodeSize] = '\0'; // Set terminating char

		// Copy to result to objectCode
		strcpy(objectCode, tempObjCode);
	}
}
// Checks if the 'X' bit is set in the operand field
int checkIndexBit(char * operand)
{
	int operandLen = strlen(operand)-1;
	char chompOperand[32];
	int i;

	// If index bit is set
	if(operand[operandLen-1] == ',' && operand[operandLen] == 'X'){
		// Copy all but ',X' from operand
		for(i = 0; i < operandLen-1; i++)
			chompOperand[i] = operand[i];

		// Copy chomped operand
		strcpy(operand, chompOperand);

		return 1;

	}else{
		return 0;
	}

}
// Assembles the sic instruction using index addressing
// the object code is stored in 'objectCode'
void setIndexAddressing(char* opcodeValue, unsigned int operandAddress, char* objectCode)
{
	unsigned int X_CONTENT = 32768;
	unsigned int newOprAdd = 0;
	char hexOprAddress[10];
	int opcodeLen, addressLen, objCodeLen;
	int i = 0, j = 0; // Iterators

	// Determine new operand address(in base 10)
	newOprAdd = operandAddress + X_CONTENT;

	// Store new operand address to string (in base 16)
	sprintf(hexOprAddress, "%X", newOprAdd);

	// Set the length's from strings
	addressLen = strlen(hexOprAddress);
	opcodeLen = strlen(opcodeValue);
	objCodeLen = addressLen + opcodeLen;

	// Copy opcode to objectCode
	for(i = 0; i < opcodeLen; i++)
		objectCode[i] = opcodeValue[i];

	// Copy new operand address to objecCode
	for(j = i; j < objCodeLen; j++)
		objectCode[j] = hexOprAddress[j-i];

}
// Places '0's as the operand address for the objectCode
void zeroOperandAddress(char* opcodeValue, char* objectCode)
{
	int zeros = 0;
	int instructionLen = 6;
	int opcodeValLen = 0;
	int i = 0, j = 0;
	memset(objectCode,'\0',sizeof(objectCode));

	opcodeValLen = strlen(opcodeValue);
	zeros = instructionLen - opcodeValLen;

	// Copy opcodeValue to objectCode
	for(i = 0; i < opcodeValLen; i++)
		objectCode[i] = opcodeValue[i];

	// Place zeros to the remaining bits of instruction
	for(j = i; j < instructionLen; j++)
		objectCode[j] = '0';

	// Set terminating char 
	objectCode[j] = '\0';
}