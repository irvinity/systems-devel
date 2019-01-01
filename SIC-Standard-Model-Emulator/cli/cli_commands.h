/*
	Irving Alvarez

	The CLI commands for the SIC.
	Valid commands will have an independent defined function, others
	will prompt the user no such command(s) exist.

*/

#include <stdio.h>
#include <string.h>

#define N 8 // Number of commands

struct Commands{

	char commandName[10];
	char commandNickname[5];
	char parameterOne[10];
	char parameterTwo[10];
	char description[128];

}comm[8];

void passOne();
void passTwo();
void printOpcodes();

// Load the specifications of each command
void loadCommands()
{
	// Command 1 - load
	strcpy(comm[0].commandName, "load");
	strcpy(comm[0].commandNickname, "");
	strcpy(comm[0].parameterOne, "[filename]");
	strcpy(comm[0].parameterTwo, "");
	strcpy(comm[0].description, "Loads the specified file.");

	// Command 2 - execute
	strcpy(comm[1].commandName, "execute");
	strcpy(comm[1].commandNickname, "x");
	strcpy(comm[1].description, "Will execute the program that was previously loaded in memory.");

	// Command 3 - debug
	strcpy(comm[2].commandName, "debug");
	strcpy(comm[2].commandNickname, "db");
	strcpy(comm[2].description, "Will allow you to execute in debug mode.'");

	// Command 4 - dump
	strcpy(comm[3].commandName, "dump");
	strcpy(comm[3].commandNickname, "dmp");
	strcpy(comm[3].parameterOne, "[start]");
	strcpy(comm[3].parameterTwo, "[end]");
	strcpy(comm[3].description, "Will call the dump function. Requires two parameters 'start' and 'end' as hexadecimal values.");

	// Command 5 - help
	strcpy(comm[4].commandName, "help");
	strcpy(comm[4].commandNickname, "h");
	strcpy(comm[4].description, "Will printout a list of available commands.");

	// Command 6 - assemble
	strcpy(comm[5].commandName, "assemble");
	strcpy(comm[5].commandNickname, "asm");
	strcpy(comm[5].parameterOne, "[filename]");
	strcpy(comm[5].description, "Will assemble a SIC assembly language program into a load module and store it in a file.");

	// Command 7 - directory
	strcpy(comm[6].commandName, "directory");
	strcpy(comm[6].commandNickname, "dir");
	strcpy(comm[6].description, "Will list the files stored in the current directory.");

	// Command 8 - exit
	strcpy(comm[7].commandName, "exit");
	strcpy(comm[7].commandNickname, "");
	strcpy(comm[7].description, "Will exit from the simulator.");
}
// Message for commands not yet implemented
void stub(char* cmd)
{
	printf("'%s' in progress...try 'help'\n", cmd);
}
void assemble(char* file)
{
	passOne(file);
	passTwo();
}
// Lists the command name and description
void help()
{
	int i;
	printf("\b\b\b%-15s %-15s %-15s %-15s %-15s\n", "COMMAND", "NICKNAME", "PARAMETER 1", "PARAMETER 2", "DESCRIPTION");
	printf("----------------------------------------------------------------------------------------------------------------------------------------\n\n");

	for(i = 0; i < N; i++)
		printf("%-15s %-15s %-15s %-15s %s\n\n", comm[i].commandName, comm[i].commandNickname, comm[i].parameterOne, comm[i].parameterTwo, comm[i].description);
	
}
// Lists the current working directory
void directory()
{
	system("ls");
}
// Check if command is a valid one, along with any parameters
void checkCommand(char* cmd, char* par1, char* par2)
{
	// Load 
	if(strcmp("load", cmd) == 0){
		// Check for missing or invalid parameters
		if(par1[0] == '\0'){
			printf("'load' missing required parameter.\nSee 'help'.\n");
		}else{
			if(par2[0] != '\0')
				printf("'load' requires one parameter only.\nSee 'help'.\n");
			else
				stub(cmd);
		}
	}
	// Execute
	else if(strcmp("execute", cmd) == 0 || strcmp("x", cmd) == 0){
		// Check for missing or invalid parameters
		if(par1[0] != '\0')
			printf("'execute' requires no parameters.\nSee 'help'.\n");
		else
			stub("execute");
	}
	// Debug
	else if(strcmp("debug", cmd) == 0 || strcmp("db", cmd) == 0){
		// Check for missing or invalid parameters
		if(par1[0] != '\0')
			printf("'debug' requires no parameters.\nSee 'help'.\n");
		else
			stub("debug");
		
	}
	// Dump
	else if(strcmp("dump", cmd) == 0 || strcmp("dmp", cmd) == 0){
		// Check for missing or invalid parameters
		if(par1[0] == '\0' || par2[0] == '\0')
			printf("'dump' missing required parameters.\nSee 'help'.\n");
		else
			stub("dump");
		
	}
	// Help
	else if(strcmp("help", cmd) == 0 || strcmp("h", cmd) == 0){
		// Check for missing or invalid parameters
		if(par1[0] != '\0')
			printf("'help' requires no parameters.\nSee 'help'.\n");
		else
			help();
	}
	// Assemble
	else if(strcmp("assemble", cmd) == 0 || strcmp("asm", cmd) == 0){
		// Check for missing or invalid parameters
		if(par1[0] == '\0'){
			printf("'assemble' missing required parameter.\nSee 'help'.\n");
		}
		else{
			if(par2[0] != '\0')
				printf("'assemble' requires one parameter only.\nSee 'help'.\n");
			else
				assemble(par1);
			
		}
	}
	// Directory
	else if(strcmp("directory", cmd) == 0 || strcmp("dir", cmd) == 0){
		// Check for missing or invalid parameters
		if(par1[0] != '\0')
			printf("'directory' requires no parameters.\nSee 'help'.\n");
		else
			directory();
		
	}
	else if(strcmp("exit", cmd) == 0){
		// Check for missing or invalid parameters
		if(par1[0] != '\0')
			printf("'exit' requires no parameters.\nSee 'help'.\n");
		
	}
	// Any other command is automatically invalid 
	else{
		printf("%s: Invalid command, try 'help'\n", cmd);
	}
}
