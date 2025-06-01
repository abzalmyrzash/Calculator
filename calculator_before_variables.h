#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "dynarr.h"
#include "hashmap.h"
#include "strfunctions.h"
#include "token.h"

DynArr matrices;
DynArr numbers;
HashMap hashmap;
Matrix* matrix_memory;
double number_memory;

void init_globals() {
	DynArr_init(&matrices, sizeof(Matrix), 8, DynArrFuncNULL);
	HashMap_init(&hashmap, 8);
	matrix_memory = NULL;
	number_memory = 0;
}

void clear_stdin() {
	char c;
	while ((c = getchar()) != '\n' && c != EOF) { }
}

Matrix* save_matrix(Matrix* matrix, char *name)
{
	Matrix* oldMatrix = HashMap_search(&hashmap, name);
	if(oldMatrix != NULL) {
		HashMap_delete(&hashmap, name);
		DynArr_delete(&matrices, oldMatrix);
	}
	Matrix* matrix_ = DynArr_append(&matrices, matrix);
	char *name_permanent = malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(name_permanent, name);
	HashMap_insert(&hashmap, name_permanent, matrix_);
	return matrix_;
}

Matrix* get_matrix_by_name(char *name)
{
	return (Matrix*)HashMap_search(&hashmap, name);
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

#define SPECIAL_TOKENS_SIZE 11
char* special_tokens[SPECIAL_TOKENS_SIZE] = {
	"exit", "quit", "let", "calc", "solve", "mem", "memx", "matrix", "save", "print", "help"
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
				void* ptr = HashMap_search(&hashmap, t.str);
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
	matrix_memory = Matrix_input(N, M);
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
	save_matrix(matrix_memory, name);
	return 0;
}

int process_tokens_print(Token* tokens, int len) {
	if (len != 1) {
		printf("ERROR: Invalid number of arguments!\n");
		return 1;
	}
	char* name = tokens[0].str;
	Matrix* matrix = HashMap_search(&hashmap, name);
	if (matrix == NULL) {
		printf("ERROR: %s doesn't exist!\n", name);
		return 1;
	}
	Matrix_print(matrix);
	return 0;
}

int process_tokens_mem(Token* tokens, int len) {
	printf("%g\n", number_memory);
	return 0;
}

int process_tokens_memx(Token* tokens, int len) {
	if(matrix_memory == NULL) {
		printf("Matrix memory is NULL!\n");
		return 0;
	}
	Matrix_print(matrix_memory);
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
		if (specialToken == "mem") {
			return process_tokens_mem(tokens+1, len-1);
		}
		if (specialToken == "memx") {
			return process_tokens_memx(tokens+1, len-1);
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
