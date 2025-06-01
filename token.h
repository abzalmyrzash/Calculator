#pragma once
#include "strfunctions.h"

typedef enum {
	TOKEN_TYPE_NAME,
	TOKEN_TYPE_NUMBER,
	TOKEN_TYPE_OPERATION,
	TOKEN_TYPE_BRACKET1,
	TOKEN_TYPE_BRACKET2,
	TOKEN_TYPE_EQUAL,
	TOKEN_TYPE_INVALID
} TokenType;

char* tokenTypeStrArr[7] = {
	"TOKEN_TYPE_NAME",
	"TOKEN_TYPE_NUMBER",
	"TOKEN_TYPE_OPERATION",
	"TOKEN_TYPE_BRACKET1",
	"TOKEN_TYPE_BRACKET2",
	"TOKEN_TYPE_EQUAL",
	"TOKEN_TYPE_INVALID"
};

typedef struct {
	char* str;
	TokenType type;
} Token;

void Token_print(Token* t){
	printf("%s - %s\n", t->str, tokenTypeStrArr[t->type]);
}

void DynArrToken_print(void* data, int len){
	for (int i = 0; i < len; i++) {
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

#define NUM_OPERATIONS 11
#define NUM_SINGLE_CHAR_OPS 7
#define NUM_DOUBLE_CHAR_OPS 4

char* operations[NUM_OPERATIONS] = {
	"+", "-", "*", "/", "%", ">", "<", "==", "!=", ">=", "<="
};

char* single_char_ops = "+-*/%><";

char* double_char_ops[NUM_DOUBLE_CHAR_OPS] = {
	"==", "!=", ">=", "<="
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

DynArr* split_into_tokens(char** words, int len)
{
	DynArr* tokens = DynArr_new(sizeof(Token), 8, DynArrTokenFunc);
	Token token;
	for (int i = 0; i < len; i++) {
		char* word = words[i];
		int wordlen = strlen(word);
		char* tokenStart = word;
		char* tokenString;
		int tokenLen;
		TokenType tokenType;

		while(*tokenStart != '\0') {
			int name_len = str_is_name_len(tokenStart); 
			int num_len = str_is_num_len(tokenStart);
			int op_len = str_is_op_len(tokenStart);
			bool is_equal_sign = *tokenStart == '=';
			bool is_bracket1 = *tokenStart == '(';
			bool is_bracket2 = *tokenStart == ')';

			if (name_len > 0) {
				tokenLen = name_len;
				tokenType = TOKEN_TYPE_NAME;
			}

			else if (num_len > 0) {
				tokenLen = num_len;
				tokenType = TOKEN_TYPE_NUMBER;
			}

			else if (op_len > 0) {
				tokenLen = op_len;
				tokenType = TOKEN_TYPE_OPERATION;
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

			else {
				tokenLen = 1;
				tokenType = TOKEN_TYPE_INVALID;
			}

			char* tokenString = str_get_null_terminated(tokenStart, tokenLen);
			token = (Token){tokenString, tokenType};
			tokenStart += tokenLen;
			DynArr_append(tokens, &token);
		}
	}
	return tokens;
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
