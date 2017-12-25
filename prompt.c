#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

/*if compiling on Windows compile these functions */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/*Fake readline function*/

char* readline(char* prompt){
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char* cpy = malloc(strlen(buffer) + 1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy)-1] = '\0';
	return cpy
}

/*fake add_history function*/
void add_history(char* unused){}

#else
#include <readline/readline.h>
#include <readline/history.h>
#endif


int main(int argc, char** arvg){
	/*version and exit information*/
	puts("Lispy Version 0.0.0.0.1");
	puts("Press Cntrl+c to Exit");
	
	while(1){
		//output prompt and get input
		char* input = readline("lispy> ");
		
		//add input to history
		add_history(input);	

		//echo input back to user
		printf("You put: %s\n", input);

		//free retrieved input
		free(input);
	}

	return 0;
}
