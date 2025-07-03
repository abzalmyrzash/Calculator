#pragma once
#include <stdio.h>
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

void Token_free_void_ptr(void* ptr) {
	free( ((Token*)ptr)->str);
}

