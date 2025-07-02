#pragma once
#include "strfunctions.h"

typedef enum {
	OPERATION_PRIORITY_COMMA,
	OPERATION_PRIORITY_OR,
	OPERATION_PRIORITY_AND,
	OPERATION_PRIORITY_EQORNOT,
	OPERATION_PRIORITY_COMPAR,
	OPERATION_PRIORITY_ADDSUB,
	OPERATION_PRIORITY_MULTDIV,
	OPERATION_PRIORITY_EXPONENT,
	OPERATION_PRIORITY_UNARY
} OperationPriority;

char* addSubList[] = {"+", "-"};
char* multDivList[] = {"*", "/", "//", "%%"};
char* expList[] = {"^"};
char* unaryList[] = {"%", "!", "'", "~", "++", "--"};
char* commaList[] = {","};
char* orList[] = {"||"};
char* andList[] = {"&&"};
char* eqOrNotList[] = {"==", "~="};
char* comparList[] = {">", "<", ">=", "<="};

OperationPriority OperationPriority_getPriority(char* op) {
	if (str_is_one_of(op, addSubList, 2)) {
		return OPERATION_PRIORITY_ADDSUB;
	}
	if (str_is_one_of(op, multDivList, 4)) {
		return OPERATION_PRIORITY_MULTDIV;
	}
	if (str_is_one_of(op, expList, 1)) {
		return OPERATION_PRIORITY_EXPONENT;
	}
	if (str_is_one_of(op, unaryList, 4)) {
		return OPERATION_PRIORITY_UNARY;
	}
	if (str_is_one_of(op, commaList, 1)) {
		return OPERATION_PRIORITY_COMMA;
	}
	if (str_is_one_of(op, comparList, 4)) {
		return OPERATION_PRIORITY_COMPAR;
	}
	if (str_is_one_of(op, eqOrNotList, 2)) {
		return OPERATION_PRIORITY_EQORNOT;
	}
	if (str_is_one_of(op, andList, 1)) {
		return OPERATION_PRIORITY_AND;
	}
	if (str_is_one_of(op, orList, 1)) {
		return OPERATION_PRIORITY_OR;
	}
	return -1;
}

