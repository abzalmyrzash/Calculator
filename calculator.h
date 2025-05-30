#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "dynarr.h"
#include "hashmap.h"
#include "command.h"
#include "strfunctions.h"

DynArr matrices;
HashMap hashmap;
Matrix* lastResult;
bool globals_are_init = false;

void init_globals() {
	DynArr_init(&matrices, sizeof(Matrix), 8, DynArrFuncNULL);
	HashMap_init(&hashmap, 8);
	lastResult = NULL;
	// printf("Globals initialized.\n");
	globals_are_init = true;
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

Matrix* input_matrix(int N, int M)
{
	Matrix* matrix = Matrix_new(N, M);
	printf("Enter your %dx%d matrix:\n", N, M);
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			scanf("%lf", Matrix_at(matrix, i, j));
		}
	}
	clear_stdin();
	return matrix;
}

Matrix* get_matrix_by_name(char *name)
{
	return (Matrix*)HashMap_search(&hashmap, name);
}

int process_command(char** words, int len) {
	char* cmd = words[0];
	bool is_add = strsame(cmd, "add");
	bool is_subtract = str_starts_with(cmd, "subtr");
	bool is_multiply = str_starts_with(cmd, "mult");
	bool is_operation = is_add || is_subtract || is_multiply;
	bool is_input = strsame(cmd, "input");
	bool is_print = strsame(cmd, "print");
	bool is_let = strsame(cmd, "let");
	bool is_save = strsame(cmd, "save");
	char* exit_cmds[2] = {"exit", "quit"};
	bool is_exit = str_is_one_of(cmd, exit_cmds, 2);
	
	if (is_input) {
		if (len != 4) {
			printf("ERROR: Invalid number of arguments!\n");
			return 1;
		}
		char *name = words[1];
		char *endptr;
		int N = strtol(words[2], &endptr, 10);
		if(*endptr != '\0') {
			printf("ERROR: Non-integer value provided!\n");
			return 1;
		}
		int M = strtol(words[3], &endptr, 10);
		if(*endptr != '\0') {
			printf("ERROR: Non-integer value provided!\n");
			return 1;
		}
		Matrix* matrix = input_matrix(N, M);
		save_matrix(matrix, name);
		free(matrix);
	}

	else if (is_print) {
		if (len != 2) {
			printf("ERROR: Invalid number of arguments!\n");
			return 1;
		}
		char *name = words[1];
		Matrix *matrix = get_matrix_by_name(name);
		if(matrix == NULL) {
			printf("ERROR: Matrix with name '%s' not found!\n", name);
			return 1;
		}
		Matrix_print(matrix);
	}

	else if(is_operation) {
		if (len != 3) {
			printf("ERROR: Invalid number of arguments!\n");
			return 1;
		}
		char *name1 = words[1];
		char *name2 = words[2];
		Matrix* A = get_matrix_by_name(name1);
		if(A == NULL) {
			printf("ERROR: Matrix with name '%s' not found!\n", name1);
			return 1;
		}
		Matrix_print(A);
		Matrix* B = get_matrix_by_name(name2);
		if(B == NULL) {
			printf("ERROR: Matrix with name '%s' not found!\n", name2);
			return 1;
		}
		Matrix_print(B);
		Matrix* result = NULL;
		if(is_multiply) result = Matrix_multiply(A, B);
		if(is_add) result = Matrix_add(A, B);
		if(is_subtract) result = Matrix_subtract(A, B);
		if(result == NULL) {
			return 1;
		}
		Matrix_free(lastResult);
		lastResult = result;
		Matrix_print(lastResult);
	}

	else if(is_save) {
		if (len != 2) {
			printf("ERROR: Invalid number of arguments!\n");
			return 1;
		}
		if (lastResult == NULL) {
			printf("ERROR: no result that can be saved!\n");
			return 1;
		}
		char *name = words[1];
		save_matrix(lastResult, name);
	}

	else if (is_exit) {
		exit(0);
	}

	else {
		printf("ERROR: Invalid command!\n");
		return 1;
	}
	return 0;
}

