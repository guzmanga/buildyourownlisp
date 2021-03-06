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


//performs corresponding C operation for an operator
long eval_op(long x, char* op, long y){
	if(strcmp(op, "+") == 0){
		return x + y;
	}
	if(strcmp(op, "-") == 0){
		return x - y;
	}
	if(strcmp(op, "*") == 0){
		return x * y;
	}
	if(strcmp(op, "/") == 0){
		return x / y;
	}
	if(strcmp(op, "%") == 0){
		return x % y;
	}

	return 0;
}

//evaluates an expression 
long eval(mpc_ast_t* t){

	//base case: if tagged as number return it directly 
	if(strstr(t->tag, "number")){
		return atoi(t->contents);
	}

	//operator will always be second child in AST 
	char* op = t->children[1]->contents;

	//store the third child in x
	long x = eval(t->children[2]);

	//iterate through the remaining children and combining
	int i = 3;
	while(strstr(t->children[i]->tag, "expr")){
		x = eval_op(x, op, eval(t->children[i]));
		i++;
	}

	return x; 
}


int main(int argc, char** arvg){

	/*creating and naming rules for polish notation expressions
	  using regex to get chars*/

	mpc_parser_t* Number = mpc_new("number");
	mpc_parser_t* Operator = mpc_new("operator");
	mpc_parser_t* Expr = mpc_new("expr");
	mpc_parser_t* Lispy = mpc_new("lispy");

	/*define rules 
	Number: between 0 and 9, negative optional
	Operator: one or more of +,-,*,/
	Expression: either a Number or (, an Operator, one or more Expression and an ')'
	Lispy: start of input, an operator, one or more expression and end of input 
	*/

	mpca_lang(MPCA_LANG_DEFAULT,
		"					\
		number: /-?[0-9]+/;		\
		operator: '+'|'-'|'*'|'/'|'%'|/(add)|(sub)|(mul)|(div)/;	\
		expr : <number> | '(' <operator><expr>+')';\
		lispy: /^/<operator><expr>+/$/;	\
		",
		Number, Operator, Expr, Lispy);

	/*version and exit information*/
	puts("Lispy Version 0.0.0.0.2");
	puts("Press Cntrl+c to Exit");
	
	while(1){
		//output prompt and get input
		char* input = readline("mylisp> ");
		
		//add input to history
		add_history(input);	

		//attempt to parse user input
		mpc_result_t r;
		if(mpc_parse("<stdin>", input, Lispy, &r)){

			long result = eval(r.output);
			printf("%li\n", result);
			mpc_ast_delete(r.output);
		}
		else{
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}

		free(input);

	}

	mpc_cleanup(4, Number, Operator, Expr, Lispy);

	return 0;
}
