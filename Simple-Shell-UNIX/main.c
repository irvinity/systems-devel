/*
	Irving Alvarez

	A simple shell - Reads an input line given by the user then parses that input line into its command name(1st word) and
	arguments(e.g "ls -a"), if any. The command is really a file under a path. The command is looked up in all paths returned 
	by the environment variable, PATH. If the command is found the path is returned and executed by a child process; otherwise, 
	the shell prompts for new input.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Useful variables
#define LINE_LEN	80
#define	MAX_ARGS	64
#define	MAX_ARG_LEN	16
#define	MAX_PATHS	64
#define	MAX_PATH_LEN	96
#define WHITESPACE	" .,\t\n"

#define TRUE 1
#define FALSE	0

// A simple struct use to save the command line input
struct command_t{
	char *name;				// Command name
	int argc;				// Argument count
	char *argv[MAX_ARGS];	// Command arguments
};

char* lookupPath(char**, char**);
int parseCommand(char*, struct command_t*);
int parsePath(char**);
void printPrompt();
int readCommand(char*);
void removeNewLineChar(char*);
int exitShell(char*);
void wait();

int main(int argc, char* argv[])
{
	// Shell variables
	char* command_line;
	struct command_t command;
	int valid_input;

	// Environment variables
	char* dirs[MAX_PATHS];
	char* user_name;

	// Process's variables
	int child_PID;
	int paths_count;

	// Parse the path variable
	paths_count = parsePath(dirs);

	// Shell initialization
	command_line = (char*)malloc(LINE_LEN);
	user_name = (char*)getenv("USER");

	do{

		// Prompt the user
		printf("\n%s>> ", user_name);

		// Read in the input command line
		valid_input = readCommand(command_line);
		
		if(valid_input){

			// Parse the input command line
			parseCommand(command_line, &command);

			// Get the full pathname for the file
			command.name = lookupPath(command.argv, dirs);

			// Command not found
			if(command.name == NULL)
				continue;
			
			// Create a child process to execute the command
			if(child_PID = fork() == 0){
				execv(command.name, command.argv);
				exit(0);
			}else{
				// Wait for the child to terminate
				wait(child_PID);
			}
		}

	}while(!exitShell(command_line));
	
	// Free memory
	free(command_line);

	return 0;
}
// Read the line from user input and save it in command_line
// Return 1 if input is valid; otherwise return 0
int readCommand(char* command_line)
{
	fgets(command_line, LINE_LEN, stdin);
	//printf("%s\n", command_line); // Debug

	if(strlen(command_line) > LINE_LEN){
		printf("Error: Maximum command length is %u\n", LINE_LEN);
		return FALSE;
	}else if(command_line[0] ==  '\n'){
		return FALSE;
	}else{
		removeNewLineChar(command_line);
		return TRUE;
	}
}
// Breaks the command line into tokens(arguments)
int parseCommand(char* command_line, struct command_t* cmd)
{
	char* delims = WHITESPACE;
	char* token;	// Temp variable
	int argc = 0;	// Number of arguments

	// Begin parsing
	do{

		if(argc < 1)
			token = strtok(command_line, delims); // Get first token
		else
			token = strtok(NULL, delims);	// Get the other tokens
		
		// Allocate memory and save the token
		cmd->argv[argc] = (char*)malloc(MAX_ARG_LEN);
		cmd->argv[argc] = token;

		argc += 1;

		// Validate input
		if(argc > MAX_ARGS)
			return 0;

	}while(token != NULL);

	cmd->argv[argc] = (char*)malloc(MAX_ARG_LEN);
	cmd->argv[argc] = '\0';

	argc -= 1; // Remove null token

	cmd->name = (char*)malloc(MAX_ARG_LEN);
	strcpy(cmd->name, cmd->argv[0]); // Set command name

	return 1;
}
// Parse the PATH variable and save directories to *dirs[]
int parsePath(char* dirs[])
{	
	// Variables to store data 
	char* path_env_var;
	char* the_path;

	// Variables for tokenizing
	char* token;
	char* delim = ":";
	int paths_count = 0;

	for(int i=0; i<MAX_ARGS; i++)
		dirs[i] = NULL;

	path_env_var = (char*)getenv("PATH");
	the_path = (char*)malloc(strlen(path_env_var)+1);
	strcpy(the_path, path_env_var);

	//printf("\n%s\n\n", the_path); // Debug
	
	// Loop to parse the_path
	do{

		if(paths_count < 1)
			token = strtok(the_path, delim);
		else
			token = strtok(NULL, delim);

		// Allocate memory and save current path
		dirs[paths_count] = (char*)malloc(MAX_PATH_LEN);
		dirs[paths_count] = token;

		paths_count += 1;

	}while(token != NULL);

	paths_count -= 1;

	return paths_count;

}
// Search for the command file, return the file path or NULL
char* lookupPath(char** argv, char** dirs)
{
	char* result;				// Command file path
	char p_name[MAX_PATH_LEN];	// Copy of command path
	int command_f;				// Validation var

	// Initialization
	result = NULL;
	p_name[0] = '\0';
	command_f = -1;

	// If the command is the absolute path of the command...return as it is
	if(*argv[0] == '/'){
		result = (char*)malloc(strlen(argv[0])+1);
		strcpy(result, argv[0]);
		return result;
	}
	// If the command is "exit"
	else if(strcmp(argv[0], "exit") == 0){
		return argv[0];
	}

	// Else, search for the command file path
	for(int i=0; i<MAX_PATHS; i++){

		if(dirs[i] != '\0'){

			// Concatanate "/argv[0]" after p_name
			strcpy(p_name, dirs[i]); strcat(p_name, "/"); strcat(p_name, argv[0]);
			//printf("%s\n", p_name); // Debug

			// Search for the command file
			command_f = access(p_name, F_OK);

			// Return the command file if it was found
			if(command_f == 0){
				result = (char*)malloc(strlen(p_name)+1);
				strcpy(result, p_name);
				return result;
			}
		}
	}

	// Return NULL if the command was not found
	printf("%s: Command not found\n", argv[0]);
	return NULL;
}
// Removes the trailing new line character from str
void removeNewLineChar(char* str)
{
	int s_len = strlen(str)-1;

	if(str[s_len] == '\n')
		str[s_len] = '\0';
}
// Returns TRUE to exit shell
int exitShell(char* command_line)
{
	if(strcmp("exit", command_line) == 0)
		return TRUE;
	else
		return FALSE;
}
