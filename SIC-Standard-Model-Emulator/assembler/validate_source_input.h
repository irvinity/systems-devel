/* 
	Irving Alvarez
	
	This file handles the validation of symbols, operands, and the BYTE operand 
	format and content read from the source file during pass one of the assembler.

	Instructions -
		Valid Symbol: Must be 6 alphanumeric characters or less, and its first character must be a letter.

		Valid Operands: Must be in the form "operand" or "operand, X", without the quotations, and operand is either 
		a label(symbol) or an actual hexadecimal address, if the latter starts with a A..F, it must
		have a leading 0 (e.g. 0A14) to distinguish them from symbols.

		BYTE directive: Can have two formats. C'..' or X'..' , if first format then it must be at most
		30 chars, if second format it must be at most 32 digits(chars).
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define valid 1
#define invalid 0

// Useful variables
typedef unsigned char BYTE;

int maxSymbolSize = 6;
int maxChars = 30;
int maxHexDigits = 32;

int checkSymbol(char* symbol);
int checkOperand(char* operand);
int checkByteOperand(char* operand);
int checkStartOperand(char* operand);
int checkString(char* str);
int checkHexDigits(char* str);
int checkByteOperandFormat(char* operand);
int isEven(int num);
int isalnum(int n);
int isalpha(int n);
int isdigit(int n);

// Return 1 if symbol is valid; otherwise return 0
int checkSymbol(char* symbol)
{
	int i = 0;
	int c = symbol[i];
	int symSize = strlen(symbol);
	
	// If symbol has > 6 chars
	if(symSize > maxSymbolSize)
		return invalid;
	
	// If first char is not a letter
	if(!isalpha(c))
		return invalid;

	for(int j = i; j < symSize; j++){
		if(!(isalnum(c)))
			return invalid;
		c = symbol[j];
	}

	return valid;
}
// Return 1 if operand is valid; otherwise return 0
int checkStartOperand(char* operand)
{
	int i = 0;

	// If operand is empty
	if(operand[i] == '\0')
		return invalid;
	// If operand begins with 'A-Z'
	else if(isalpha(operand[0]))
		return invalid;
	// Iterate string operand
	else
		return checkHexDigits(operand);
	
}
// Checks the format of operand
int checkOperand(char* operand)
{
	// If operand is a HEX string
	if(isdigit(operand[0]))
		return checkHexDigits(operand);
	// If operand is not a HEX string
	else
		return checkString(operand);
	
}
// Checks BYTE operand format
// Returns the length of operand value in bytes if it's in valid format; otherwise return 0
int checkByteOperandFormat(char* operand)
{
	BYTE bytes = 0;
	int length = 0;
	int opSize = strlen(operand);

	// If operand is in the CHAR(C'..') format
	if(operand[0] == 'C')
	{
		// Second char & last are ' char
		if((operand[1] == 39 && (operand[opSize - 1] == 39) || operand[opSize - 1] == '\0')){
			length = opSize - 3;
			// Length must be <= 30
			if(length <= maxChars)
				return length;
			else
				length = 0;
		}
	}
	// If operand is in the HEX(X'..') format
	else if(operand[0] == 'X')
	{
		// If second char & last are ' char
		if(operand[1] == 39 && operand[opSize - 1] == 39){
			length = opSize - 3;
			int even = isEven(length);
			// Length must be even and <= 32
			if(even == valid && length <= maxHexDigits)
				return length / 2;
			else
				length = 0;
		}
	}
	return length;
}
// Return 1 if num is even; otherwise return 0
int isEven(int num)
{
	if(num % 2 == 0)
		return valid;
	else
		return invalid;
}
// Return 1 if str(operand) has valid format; otherwise return 0
int checkString(char* str)
{
	int i = 0; // Iterator
	char c = ','; // Only(one) non alphanumeric char permitted

	// 'str' must be 8 chars o less
	if(strlen(str) > 8)
		return invalid;

	// Repeat until the end of 'str'
	while(str[i] != '\0')
	{
		int ch = str[i];

		// If ch is not an alphanumeric char
		if(!isalnum(ch)){
			// If ch is ',', next char must be 'X'
			if(ch == c){
				if(str[i+1] != 'X')
					return invalid;
			}else{
				return invalid;
			}
		}
		++i;

	}
	return valid;
}
// Return 1 if str(Hex Digits) have valid format; otherwise return 0
int checkHexDigits(char* str)
{
	int i = 0; // Iterator

	// If HEX is single digit
	if((str[i] >= '0' && str[i] <= '9') && str[i+1] == '\0')
		return valid;
	
	// If the first char is '0'
	if(str[i] == '0'){
		++i;
		// Next char must be A-F
		if(str[i] >= 'A' && str[i] <= 'F'){
			// Next chars must be A-F or 0-9
			while(str[i] != '\0'){
				if((str[i] >= 'G' && str[i] <= 'Z') && !isdigit(str[i]))
					return invalid;
				++i;
			}
			return valid;

		}else{
			return invalid;
		}
	}
	// Else if the first char is 1-9
	else{
		// Next chars must be A-F or 0-9
		while(str[i] != '\0'){
			if((str[i] >= 'G' && str[i] <= 'Z') && !isdigit(str[i]))
				return invalid;
			++i;
		}
		return valid;
	}
}