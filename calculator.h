#pragma once
#include "operation.h"
#include "hashmap.h"
#include "strfunctions.h"
#include "token.h"
#include "stack.h"

ExpressionTree *tree = NULL;

void clear_stdin() {
	char c;
	while ((c = getchar()) != '\n' && c != EOF) { }
}

Variable* get_var_by_name(char *name)
{
	return (Variable*)HashMap_search(hashmap, name);
}

Variable* copy_var_with_name(Variable* var, char *name)
{
	Variable* oldVar = get_var_by_name(name); 
	if(oldVar != NULL) {
		HashMap_delete(hashmap, name);
		Variable_free(oldVar);
	}
	Variable* copy = Variable_copy(var);
	Variable_change_name(copy, name);
	HashMap_insert(hashmap, copy->name, copy);
	return copy;
}

void delete_variable(char *name)
{
	Variable* var = get_var_by_name(name);
	if (var == NULL) {
		printf("ERROR: doesn't exist!\n");
		return;
	}
	HashMap_delete(hashmap, name);
	Variable_free(var);
}

#define SPECIAL_TOKENS_SIZE 10
char* special_tokens[SPECIAL_TOKENS_SIZE] = {
	"exit", "quit", "let", "calc", "solve", "matrix",
	"save", "del", "print", "help"
};

int process_tokens_let(Token* tokens, int len) {
	if(len != 2) {
		printf("ERROR: Invalid number of arguments!\n");
		return 1;
	}
	if(tokens[0].type != TOKEN_TYPE_NAME) {
		printf("ERROR: Invalid name!\n");
		return 1;
	}
	char* name = tokens[0].str;
	if(str_find_in_list(name, special_tokens, SPECIAL_TOKENS_SIZE) != -1) {
		printf("ERROR: Invalid name!\n");
		return 1;
	}
	
	return 0;
}

int process_tokens_calc(Token* tokens, int len) {
	Matrix* prev;
	int cnt_bracket1 = 0;
	int cnt_bracket2 = 0;
	Stack* bracketStack = Stack_new();
	for (int i = 0; i < len; i++) {
		Token t = tokens[i];
		switch(t.type) {
			case TOKEN_TYPE_NAME:
				void* ptr = get_var_by_name(t.str);
				if (ptr == NULL) {
					printf("ERROR: %s doesn't exist!\n", t.str);
					return 1;
				}
				break;
			case TOKEN_TYPE_NUMBER:
				break;
			case TOKEN_TYPE_OPERATION:
				break;
			case TOKEN_TYPE_EQUAL:
				printf("Equal sign not supported yet\n");
				return 1;
				break;
			case TOKEN_TYPE_BRACKET1:
				cnt_bracket1++;
				break;
			case TOKEN_TYPE_BRACKET2:
				cnt_bracket2++;
				break;
			case TOKEN_TYPE_INVALID:
				printf("Invalid token!\n");
				return 1;
				break;
			default:
				printf("BUG: IMPOSSIBLE TOKEN TYPE!\n");
				return 1;
				break;
		}
	}
	if(cnt_bracket1 != cnt_bracket2) {
		printf("ERROR: Non-matching brackets!\n");
		return 1;
	}
	if(tree != NULL) {
		ExpressionTree_free(tree);
	}
	tree = ExpressionTree_new(tokens, len);
	printf("Tree created\n");
	ExpressionTree_print(tree);
	ExpressionTree_split(tree);
	printf("Tree split\n");
	ExpressionTree_print(tree);
	Variable_print(ExpressionTree_evaluate(tree));
	return 0;
}

int process_tokens_matrix(Token* tokens, int len) {
	if (len != 2) {
		printf("ERROR: Invalid number of arguments!\n");
		return 1;
	}
	for (int i = 0; i < len; i++) {
		Token t = tokens[i];
		if (!str_is_int(t.str)) {
			printf("ERROR: Non-integer argument provided!\n");
			return 1;
		}
	}
	char* endPtr;
	int N = strtol(tokens[0].str, &endPtr, 10);
	int M = strtol(tokens[1].str, &endPtr, 10);
	Matrix* matrix = Matrix_input(N, M);
	Variable_assign_matrix(memory, matrix);
	return 0;
}

int process_tokens_save(Token* tokens, int len) {
	if (len != 1) {
		printf("ERROR: Invalid number of arguments!\n");
		return 1;
	}
	if(tokens[0].type != TOKEN_TYPE_NAME) {
		printf("ERROR: Invalid name!\n");
		return 1;
	}
	char* name = tokens[0].str;
	if(str_find_in_list(name, special_tokens, SPECIAL_TOKENS_SIZE) != -1) {
		printf("ERROR: Invalid name!\n");
		return 1;
	}
	copy_var_with_name(memory, name);
	return 0;
}

int process_tokens_del(Token* tokens, int len) {
	if (len != 1) {
		printf("ERROR: Invalid number of arguments!\n");
		return 1;
	}
	char* name = tokens[0].str;
	delete_variable(name);
}

int process_tokens_print(Token* tokens, int len) {
	if (len != 1) {
		printf("ERROR: Invalid number of arguments!\n");
		return 1;
	}
	char* name = tokens[0].str;
	Variable* var = get_var_by_name(name);
	if (var == NULL) {
		printf("ERROR: %s doesn't exist!\n", name);
		return 1;
	}
	Variable_print(var);
	return 0;
}

int process_tokens_help(Token* tokens, int len) {
	printf("List of special tokens:\n");
	for (int i = 0; i < SPECIAL_TOKENS_SIZE; i++) {
		printf("%s\n", special_tokens[i]);
	}
	printf("\n");
	return 0;
}

int process_tokens(Token* tokens, int len) {
	DynArr* invalid = Token_indices_by_type(tokens, len, TOKEN_TYPE_INVALID);
	if (invalid->len > 0) {
		printf("ERROR: Invalid tokens!\n");
		return 1;
	}
	Token firstToken = tokens[0];
	int special_index = str_find_in_list(
		firstToken.str, special_tokens, SPECIAL_TOKENS_SIZE);
	if (special_index != -1) {
		char* specialToken = special_tokens[special_index];
		if (specialToken == "exit" || specialToken == "quit") {
			exit(0);
		}
		if (specialToken == "let") {
			return process_tokens_let(tokens+1, len-1);
		}
		if (specialToken == "calc") {
			return process_tokens_calc(tokens+1, len-1);
		}
		if (specialToken == "matrix") {
			return process_tokens_matrix(tokens+1, len-1);
		}
		if (specialToken == "save") {
			return process_tokens_save(tokens+1, len-1);
		}
		if (specialToken == "del") {
			return process_tokens_del(tokens+1, len-1);
		}
		if (specialToken == "print") {
			return process_tokens_print(tokens+1, len-1);
		}
		if (specialToken == "help") {
			return process_tokens_help(tokens+1, len-1);
		}
	}
	else {
		return process_tokens_calc(tokens, len);
	}
	return 0;
}

int process_command(char* command) {
	DynArr* words = split_by_space(command);
	if (words->len == 0) {
		DynArr_free(words);
		return 1;
	}
	DynArr* tokens = split_into_tokens(words->data, words->len);
	DynArr_free(words);
	if (tokens->len == 0) {
		DynArr_free(tokens);
		return 1;
	}
	// DynArr_print(tokens);
	int res = process_tokens(tokens->data, tokens->len);
	DynArr_free(tokens);
	return res;
}

