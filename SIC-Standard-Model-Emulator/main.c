/*
	Irving Alvarez
	
	SIC Standard Model Emulator

	The CLI processes the input from user, the program consist of hardcoded commands that the user
	can run to emulate the SIC machine modules. The assembler files construct the object program 
	from the assembly source code (given there is no errors).
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli/cli_commands.h"
#include "assembler/pass_one.h" // Pass one of assembler
#include "assembler/pass_two.h" // Pass two of assembler
#include "assembler/validate_source_input.h"

int breakUp(char* str, char* c, char* p1, char* p2);
int exit_cli(char* cmd);

int main(int argc, char *argv[])
{

	// CLI data variables
	char line[80], comm[10], p1[10], p2[10];
	int len, n;

	// Welcome and prompt user
	printf("\nHello. Welcome to the SIC command line interface.\nEnter a command or 'help' to get started.\n");

	// Create stuct array
	loadCommands();

	// Enter cli
	do
	{	

		printf("Command> ");

		// Read in a line 
		fgets(line, 80, stdin);
		printf("\n");

		// Get rid of trailing newline character 
		len = strlen(line) - 1;
		if(line[len] == '\n')
			line[len] = '\0'; // Null
		
		// Split the line into word(s)
		n = breakUp(line, comm, p1, p2);
		
		if(n > 3)
			printf("Error: Too many arguments.\nSee 'help'\n");
		else
			checkCommand(comm, p1, p2);
		
		printf("\nTotal characters: %d\n", len);
		printf("Commands in this line: %d\n", n);	
		printf("----------------------------------------------------------\n");
		
	}while(exit_cli(line) != 1);

	return 0;
}

// This function takes a line, 'str' and breaks it into words.
// The command is in 'c', the paramaters in 'p1' and 'p2',
// and the number of words in 'n'.
int breakUp(char* str,  char* cmd, char* par1, char* par2)
{
	// Useful variables
	int line_itr = 0;
	int word_itr = 0;
	int wc = 0;

	char wr_flag = '*';
	char SPACE = ' ';
	char END = '\0';

	// Reset arrays
	memset(cmd, '\0', sizeof(cmd));
	memset(par1, '\0', sizeof(par1));
	memset(par2, '\0', sizeof(par2));

	// Process line and break into words
	while(str[line_itr] != END)
	{
		if(cmd[strlen(cmd)-1] != wr_flag){ 

			// Copy current character
			if(str[line_itr] != SPACE){
				if(str[line_itr+1] == '\0')
					++wc;
				
				cmd[word_itr] = str[line_itr];
				word_itr++;
			}
			// Space detected
			else{
				if(str[line_itr+1] == '\0'){
					++wc;
					break;
				}
				cmd[word_itr] = wr_flag;
				++wc;
				word_itr = 0;
			}
		}
		else if(par1[strlen(par1)-1] != wr_flag){

			// Copy current character
			if(str[line_itr] != SPACE){
				if(str[line_itr+1] == '\0')
					++wc;
				
				par1[word_itr] = str[line_itr];
				word_itr++;
			}
			// Space detected
			else{
				if(str[line_itr+1] == '\0'){
					++wc;
					break;
				}
				par1[word_itr] = wr_flag;
				++wc;
				word_itr = 0;
			}
		}
		else if(par2[strlen(par2)-1] != wr_flag){

			// Copy current character
			if(str[line_itr] != SPACE){
				if(str[line_itr+1] == '\0')
					++wc;
				
				par2[word_itr] = str[line_itr];
				word_itr++;
			}
			// Space detected
			else{
				if(str[line_itr+1] == '\0'){ 
					++wc;
					break;
				}
				par2[word_itr] = wr_flag;
				++wc;
				word_itr = 0;
			}
		}

		// More than three words read (Invalid)
		else if(wc >= 3 && (str[line_itr-1] == SPACE))
			++wc;
		

		line_itr += 1;

	}

	if(cmd[strlen(cmd)-1] == '*')
		cmd[strlen(cmd)-1] = '\0';
	if(par1[strlen(par1)-1] == '*')
		par1[strlen(par1)-1] = '\0';
	if(par2[strlen(par2)-1] == '*')
		par2[strlen(par2)-1] = '\0';
	
	return wc;
}

// Return 1 if cmd matches exit command; 0 otherwise.
int exit_cli(char* cmd)
{
	int exit1;
	int exit2;

	exit1 = strcmp(cmd, "exit");
	exit2 = strcmp(cmd, "EXIT");

	if(exit1 == 0 || exit2 == 0)
		return 1;
	else
		return 0;
}