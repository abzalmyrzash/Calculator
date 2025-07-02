#pragma once
#include "expression_tree.h"

void read_command(char* buffer, size_t bufsize) {
	if (fgets(buffer, bufsize, stdin) == NULL) {
		printf("\n");
		exit(1);
	}
	strtok(buffer, "\n");
}

ExpressionTree *tree = NULL;

void clear_stdin() {
	char c;
	while ((c = getchar()) != '\n' && c != EOF) { }
}

int process_tokens_matrix(Token* tokens, int len) {
	if (len != 3) {
		printf("ERROR: Invalid number of arguments!\n");
		return 1;
	}
	for (int i = 0; i < len; i++) {
		Token t = tokens[i];
		if (i == 1) {
			if (!strsame(t.str, ",")) {
				printf("ERROR: Comma was expected, but not provided!\n");
				return 1;
			}
		}
		else if (!str_is_int(t.str)) {
			printf("ERROR: Non-integer argument provided!\n");
			return 1;
		}
	}
	char* endPtr;
	int N = strtol(tokens[0].str, &endPtr, 10);
	int M = strtol(tokens[2].str, &endPtr, 10);
	Matrix* matrix = Matrix_input(N, M);
	Variable_assign_matrix(memory, matrix);
	return 0;
}

int process_tokens_vector(Token* tokens, int len) {
	if (len != 1) {
		printf("ERROR: Invalid number of arguments!\n");
		return 1;
	}
	Token t = tokens[0];
	if (!str_is_int(t.str)) {
		printf("ERROR: Non-integer argument provided!\n");
		return 1;
	}
	char* endPtr;
	int N = strtol(t.str, &endPtr, 10);
	Vector* vector = Vector_input(N);
	Vector_print(vector);
	Variable_assign_vector(memory, vector);
	return 0;
}

int process_tokens_calc(Token* tokens, int len) {
	Matrix* prev;
	int cnt_bracket1 = 0;
	int cnt_bracket2 = 0;
	int cnt_sqrbr1 = 0;
	int cnt_sqrbr2 = 0;
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
			case TOKEN_TYPE_OPERATION:
			case TOKEN_TYPE_FUNCTION:
				break;
			case TOKEN_TYPE_BRACKET1:
				cnt_bracket1++;
				break;
			case TOKEN_TYPE_BRACKET2:
				cnt_bracket2++;
				break;
			case TOKEN_TYPE_SQRBR1:
				cnt_sqrbr1++;
				break;
			case TOKEN_TYPE_SQRBR2:
				cnt_sqrbr2++;
				break;
			case TOKEN_TYPE_EQUAL:
			case TOKEN_TYPE_SPECIAL:
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
	if (cnt_sqrbr1 != cnt_sqrbr2) {
		printf("ERROR: Non-matching square brackets!\n");
		return 1;
	}
	ExpressionTree* tree = ExpressionTree_new(tokens, len);
	if (ExpressionTree_split(tree) == 1) {
		ExpressionTree_free(tree);
		return 1;
	}
	if (debug) ExpressionTree_print(tree);
	Variable* res = ExpressionTree_evaluate(tree);
	if (res == NULL || res->type == VAR_TYPE_ERROR) {
		ExpressionTree_free(tree);
		Variable_free(res);
		return 1;
	}
	Variable_assign_data(memory, res);
	if (autoprint) Variable_print(res, false);
	Variable_free(res);
	ExpressionTree_free(tree);
	
	return 0;
}

int process_tokens_let(Token* tokens, int len) {
	if(len < 3) {
		printf("ERROR: Invalid number of arguments!\n");
		return 1;
	}
	if(tokens[0].type != TOKEN_TYPE_NAME) {
		printf("ERROR: Variable name expected, but not provided!\n");
		return 1;
	}
	if(tokens[1].type != TOKEN_TYPE_EQUAL) {
		printf("ERROR: Equal sign expected, but not provided!\n");
		return 1;
	}
	char* name = tokens[0].str;
	if (str_is_one_of(name, const_var_names, CONST_VAR_NAMES_SIZE)) {
		printf("ERROR: Can't change constant variable %s!\n", name);
		return 1;
	}
	if(strsame(tokens[2].str, "matrix")) {
		process_tokens_matrix(tokens+3, len-3);
		copy_var_to_name(memory, name);
	}
	else if(strsame(tokens[2].str, "vector")) {
		process_tokens_vector(tokens+3, len-3);
		copy_var_to_name(memory, name);
	}
	else {
		if(strsame(tokens[2].str, "calc")) {
			if(process_tokens_calc(tokens+3, len-3)) return 1;
		}
		if (process_tokens_calc(tokens+2, len-2)) return 1;
		copy_var_to_name(memory, name);
	}
	
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
	if (str_is_one_of(name, const_var_names, CONST_VAR_NAMES_SIZE)) {
		printf("ERROR: Can't change constant variable %s!\n", name);
		return 1;
	}
	copy_var_to_name(memory, name);
	return 0;
}

int process_tokens_del(Token* tokens, int len) {
	if (len != 1) {
		printf("ERROR: Invalid number of arguments!\n");
		return 1;
	}
	if(tokens[0].type != TOKEN_TYPE_NAME) {
		printf("ERROR: Invalid name!\n");
		return 1;
	}
	char* name = tokens[0].str;
	if (str_is_one_of(name, special_var_names, SPECIAL_VAR_NAMES_SIZE)) {
		printf("ERROR: Can't delete special variable %s!\n", name);
		return 1;
	}
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
	Variable_print(var, true);
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

int process_tokens_debug(Token* tokens, int len) {
	if (len != 1) {
		printf("ERROR: debug accepts 1 argument!\n");
		return 1;
	}
	if (strsame(tokens->str, "on")) {
		debug = true;
		return 0;
	}
	if (strsame(tokens->str, "off")) {
		debug = false;
		return 0;
	}
	else {
		printf("ERROR: argument must be 'on' or 'off'!\n");
		return 1;
	}
}

int process_tokens_autoprint(Token* tokens, int len) {
	if (len != 1) {
		printf("ERROR: autoprint accepts 1 argument!\n");
		return 1;
	}
	if (strsame(tokens->str, "on")) {
		autoprint = true;
		return 0;
	}
	if (strsame(tokens->str, "off")) {
		autoprint = false;
		return 0;
	}
	else {
		printf("ERROR: argument must be 'on' or 'off'!\n");
		return 1;
	}
}

int process_tokens(Token* tokens, int len) {
	Token firstToken = tokens[0];
	int special_index = str_find_in_list(
		firstToken.str, special_tokens, SPECIAL_TOKENS_SIZE);
	if (special_index != -1) {
		char* specialToken = special_tokens[special_index];
		if (specialToken == "exit" || specialToken == "quit") {
			quit = true;
			return 0;
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
		if (specialToken == "vector") {
			return process_tokens_vector(tokens+1, len-1);
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
		if (specialToken == "debug") {
			return process_tokens_debug(tokens+1, len-1);
		}
		if (specialToken == "autoprint") {
			return process_tokens_autoprint(tokens+1, len-1);
		}
	}
	else {
		bool containsEqSign = false;
		for (int i = 0; i < len; i++) {
			if (strsame(tokens[i].str, "=")) {
				containsEqSign = true;
				break;
			}
		}
		if (containsEqSign) return process_tokens_let(tokens, len);
		return process_tokens_calc(tokens, len);
	}
	return 0;
}

int process_command(char* command) {
	DynArr* tokens = split_into_tokens(command);
	if (tokens == NULL) return 1;
	if (!validate_tokens(tokens->data, tokens->len)) {
		DynArr_free(tokens);
		return 1;
	}
	// DynArr_print(tokens);
	int res = process_tokens(tokens->data, tokens->len);
	DynArr_free(tokens);
	return res;
}

