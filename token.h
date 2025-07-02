#pragma once
#include "strfunctions.h"

typedef enum {
	TOKEN_TYPE_NAME,
	TOKEN_TYPE_NUMBER,
	TOKEN_TYPE_OPERATION,
	TOKEN_TYPE_FUNCTION,
	TOKEN_TYPE_BRACKET1,
	TOKEN_TYPE_BRACKET2,
	TOKEN_TYPE_SQRBR1,
	TOKEN_TYPE_SQRBR2,
	TOKEN_TYPE_EQUAL,
	TOKEN_TYPE_SPECIAL,
} TokenType;

char* tokenTypeStrArr[] = {
	"TOKEN_TYPE_NAME",
	"TOKEN_TYPE_NUMBER",
	"TOKEN_TYPE_OPERATION",
	"TOKEN_TYPE_FUNCTION",
	"TOKEN_TYPE_BRACKET1",
	"TOKEN_TYPE_BRACKET2",
	"TOKEN_TYPE_SQRBR1",
	"TOKEN_TYPE_SQRBR2",
	"TOKEN_TYPE_EQUAL",
	"TOKEN_TYPE_SPECIAL"
};

typedef struct {
	char* str;
	TokenType type;
} Token;

void Token_print(Token* t){
	printf("%s - %s\n", t->str, tokenTypeStrArr[t->type]);
}

void Token_free(Token* t) {
	free(t->str);
	free(t);
}

Token* Token_copy(Token* t) {
	Token* copy = malloc(sizeof(Token));
	strcpy(copy->str, t->str);
	copy->type = t->type;
	return copy;
}

void DynArrToken_print(void* data, size_t len){
	for (size_t i = 0; i < len; i++) {
		Token_print((Token*)data + i);
	}
}

void DynArrToken_free(void* ptr) {
	free( ((Token*)ptr)->str);
}

DynArrFunc DynArrTokenFunc = {
	DynArrToken_print,
	NULL,
	DynArrToken_free
};

#define NUM_OPERATIONS 23
#define NUM_SINGLE_CHAR_OPS 12
#define NUM_DOUBLE_CHAR_OPS 11

char* operations[NUM_OPERATIONS] = {
	",", "+", "-", "*", "/", "%", "^", "!", "'", ">", "<", "~"
	"++", "--", "**", "//", "%%", "==", "~=", ">=", "<=", "&&", "||",
};

char* single_char_ops = ",+-*/%^!'><~";

char* double_char_ops[NUM_DOUBLE_CHAR_OPS] = {
	"++", "--", "**", "//", "%%", "==", "~=", ">=", "<=", "&&", "||"
};

int str_is_op_len(char* str) {
	int len = 0;
	if(char_is_one_of(*str, single_char_ops)) len = 1;
	for(int i = 0; i < NUM_DOUBLE_CHAR_OPS; i++) {
		if (str_starts_with(str, double_char_ops[i])) {
			len = 2;
			break;
		}
	}
	return len;
}

#define SPECIAL_TOKENS_SIZE 13
char* special_tokens[SPECIAL_TOKENS_SIZE] = {
	"exit", "quit", "let", "calc", "solve", "matrix", "vector",
	"save", "del", "print", "help", "debug", "autoprint"
};

#define FUNCTION_TOKENS_SIZE 6
char* function_tokens[] = {
	"sqrt", "sin", "cos", "tan", "ln", "log"
};

DynArr* split_into_tokens(char* command)
{
	DynArr* tokens = DynArr_new(sizeof(Token), 8, DynArrTokenFunc);
	Token token;
	char* tokenStart = command;
	char* tokenString;
	int tokenLen;
	TokenType tokenType;

	while(*tokenStart != '\0') {
		if (*tokenStart == ' ') {
			tokenStart++;
			continue;
		}
		int name_len = str_is_name_len(tokenStart); 
		int num_len = str_is_num_len(tokenStart);
		int op_len = str_is_op_len(tokenStart);
		bool is_equal_sign = *tokenStart == '=';
		bool is_bracket1 = *tokenStart == '(';
		bool is_bracket2 = *tokenStart == ')';
		bool is_sqrbr1 = *tokenStart == '[';
		bool is_sqrbr2 = *tokenStart == ']';
		TokenType prevType = -1;
		if (tokens->len > 0) {
			prevType = ((Token*)DynArr_at(tokens, tokens->len - 1))->type;
		}

		if (name_len > 0) {
			tokenLen = name_len;
			tokenType = TOKEN_TYPE_NAME;
		}

		else if (op_len > 0) {
			tokenLen = op_len;
			tokenType = TOKEN_TYPE_OPERATION;
		}

		else if (num_len > 0) {
			tokenLen = num_len;
			tokenType = TOKEN_TYPE_NUMBER;
		}

		else if (is_equal_sign) {
			tokenLen = 1;
			tokenType = TOKEN_TYPE_EQUAL;
		}

		else if (is_bracket1) {
			tokenLen = 1;
			tokenType = TOKEN_TYPE_BRACKET1;
		}

		else if (is_bracket2) {
			tokenLen = 1;
			tokenType = TOKEN_TYPE_BRACKET2;
		}

		else if (is_sqrbr1) {
			tokenLen = 1;
			tokenType = TOKEN_TYPE_SQRBR1;
		}

		else if (is_sqrbr2) {
			tokenLen = 1;
			tokenType = TOKEN_TYPE_SQRBR2;
		}

		else {
			printf("ERROR: Invalid token!\n");
			DynArr_free(tokens);
			return NULL;
		}

		tokenString = str_get_null_terminated(tokenStart, tokenLen);

		if (tokenType == TOKEN_TYPE_NAME) {
			if (str_is_one_of(tokenString, special_tokens, SPECIAL_TOKENS_SIZE)) {
				tokenType = TOKEN_TYPE_SPECIAL;
			}
			else if (str_is_one_of(tokenString, function_tokens, FUNCTION_TOKENS_SIZE)) {
				tokenType = TOKEN_TYPE_FUNCTION;
			}
		}

		// if something like 2a is inputted, assume multiplication
		if (prevType != TOKEN_TYPE_SPECIAL &&
	(prevType == TOKEN_TYPE_NUMBER || prevType == TOKEN_TYPE_NAME)
	&& (tokenType == TOKEN_TYPE_NAME || tokenType == TOKEN_TYPE_BRACKET1 ||
	tokenType == TOKEN_TYPE_FUNCTION)) {
			char* multiplyStr = malloc(2);
			strcpy(multiplyStr, "*");
			Token multiplyToken = (Token){multiplyStr, TOKEN_TYPE_OPERATION};
			DynArr_append(tokens, &multiplyToken);
		}

		token = (Token){tokenString, tokenType};
		tokenStart += tokenLen;
		DynArr_append(tokens, &token);
	}
	return tokens;
}

bool validate_tokens(Token* tokens, int len) {
	if (len == 0) return false;
	return true;
}

DynArr* Token_indices_by_type(Token* tokens, int len, TokenType type) {
	DynArr* dynarr = DynArr_new (sizeof(int), 4, DynArrIntFunc);
	for (int i = 0; i < len; i++) {
		if(tokens[i].type == type) {
			DynArr_append(dynarr, &i);
		}
	}
	return dynarr;
}
