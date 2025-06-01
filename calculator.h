#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "matrixdynarr.h"
#include "dynarr.h"
#include "hashmap.h"
#include "strfunctions.h"
#include "token.h"

typedef enum {
	VAR_TYPE_NULL,
	VAR_TYPE_NUMBER,
	VAR_TYPE_MATRIX
} VariableType;

char* variableTypeStrArr[3] = {
	"NULL",
	"NUMBER",
	"MATRIX"
};

typedef struct {
	VariableType type;
	char* name;
	void* data;
} Variable;

void Variable_init(Variable* var, VariableType type, char* name, void* data) {
	var->type = type;
	var->name = name;
	var->data = data;
}

Variable* Variable_new(VariableType type, char* name, void* data) {
	Variable* var = malloc(sizeof(Variable));
	Variable_init(var, type, name, data);
	return var;
}

void Variable_print(Variable* var) {
	if (var == NULL || var->type == VAR_TYPE_NULL) {
		printf("NULL\n");
		return;
	}
	printf("%s %s = ", variableTypeStrArr[var->type], var->name);
	switch(var->type) {

	case VAR_TYPE_MATRIX:
		Matrix_print(var->data);
		break;
	case VAR_TYPE_NUMBER:
		printf("%g\n", var->data);
	default:
		printf("BUG: IMPOSSIBLE VARIABLE TYPE!\n");
		break;
	}
}

Variable* Variable_copy(Variable* var) {
	Variable* copy = malloc(sizeof(Variable));
	copy->type = var->type;
	if(var->name != NULL) {
		copy->name = malloc(strlen(var->name)+1);
		strcpy(copy->name, var->name);
	} else copy->name = NULL;
	switch(var->type) {
		case VAR_TYPE_NULL:
			copy->data = NULL;
			break;
		case VAR_TYPE_NUMBER:
			copy->data = malloc(sizeof(double));
			memcpy(copy->data, var->data, sizeof(double));
			break;
		case VAR_TYPE_MATRIX:
			copy->data = Matrix_copy(var->data);
			break;
		default:
			printf("BUG: IMPOSSIBLE VARIABLE TYPE!\n");
			break;
	}
	return copy;
}
	
void Variable_free_data(Variable* var) {
	if (var == NULL || var->data == NULL) return;
	switch(var->type) {

	case VAR_TYPE_MATRIX:
		Matrix_free(var->data);
		break;
	case VAR_TYPE_NUMBER:
		free(var->data);
		break;
	default:
		printf("BUG: IMPOSSIBLE VARIABLE TYPE!\n");
		break;
	}
}

void DynArrVar_print(void* data, int len) {
	for (int i = 0; i < len; i++) {
		Variable_print(data + i);
	}
}

void* DynArrVar_copy(void* ptr) {
	return Variable_copy(ptr);
}

void DynArrVar_free(void* ptr) {
	Variable* var = (Variable*)ptr;
	free(var->name);
	Variable_free_data(var);
}

DynArrFunc DynArrVarFunc = {DynArrVar_print, DynArrVar_copy, DynArrVar_free};

DynArr* variables;
HashMap* hashmap;
Variable* memory;

void init_globals() {
	variables = DynArr_new(sizeof(Variable), 8, DynArrVarFunc);
	hashmap = HashMap_new(8);
	memory = Variable_new(VAR_TYPE_NULL, "mem", NULL);
}

typedef struct {
	double a, b;
	char op;
} NumberOperation;

typedef struct {
	Matrix A, B;
	char op;
} MatrixOperation;

typedef struct {
	Matrix A;
	double c;
} MatrixScale;

void clear_stdin() {
	char c;
	while ((c = getchar()) != '\n' && c != EOF) { }
}

Variable* get_var_by_name(char *name)
{
	return (Variable*)HashMap_search(hashmap, name);
}

Variable* save_variable(char *name)
{
	char* name_permanent;
	Variable* oldVar = get_var_by_name(name); 
	if(oldVar != NULL) {
		name_permanent = oldVar->name;
		HashMap_delete(hashmap, name);
		DynArr_delete(variables, oldVar);
	} else {
		name_permanent = malloc(strlen(name)+1);
		strcpy(name_permanent, name);
	}
	Variable* var = DynArr_append(variables, memory);
	free(var->name);
	var->name = name_permanent;
	HashMap_insert(hashmap, name_permanent, var);
	return var;
}

void delete_variable(char *name)
{
	Variable* var = get_var_by_name(name);
	if (var == NULL) return;
	DynArr_delete(variables, var);
	HashMap_delete(hashmap, name);
	Variable_free_data(var);
	free(var->name);
	free(var);
}

void Variable_assign_number(Variable* var, double* number) {
	if(var == NULL) {
		var = (Variable*)malloc(sizeof(Variable));
	} else {
		Variable_free_data(var);
	}
	var->type = VAR_TYPE_NUMBER;
	var->data = number;
}

void Variable_assign_matrix(Variable* var, Matrix* matrix) {
	if(var == NULL) {
		var = (Variable*)malloc(sizeof(Variable));
	} else {
		// Variable_free_data(var);
	}
	var->type = VAR_TYPE_MATRIX;
	var->data = matrix;
}

#define SPECIAL_TOKENS_SIZE 11
char* special_tokens[SPECIAL_TOKENS_SIZE] = {
	"exit", "quit", "let", "calc", "solve", "mem", "matrix",
	"save", "del", "print", "help"
};

int process_tokens_let(Token* tokens, int len) {
	return 0;
}

int process_tokens_calc(Token* tokens, int len) {
	Matrix* prev;
	for (int i = 0; i < len; i++) {
		Token t = tokens[i];
		switch(t.type) {
			case TOKEN_TYPE_NAME:
				void* ptr = HashMap_search(hashmap, t.str);
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
				break;
			case TOKEN_TYPE_BRACKET1:
				break;
			case TOKEN_TYPE_BRACKET2:
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
	Variable_print(memory);
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
	save_variable(name);
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

int process_tokens_mem(Token* tokens, int len) {
	if(memory == NULL) {
		printf("Memory is NULL!\n");
		return 0;
	}
	Variable_print(memory);
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
		if (specialToken == "mem") {
			return process_tokens_mem(tokens+1, len-1);
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
