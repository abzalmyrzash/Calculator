#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "matrix.h"

typedef enum {
	VAR_TYPE_NULL,
	VAR_TYPE_NUMBER,
	VAR_TYPE_PERCENT,
	VAR_TYPE_MATRIX,
	VAR_TYPE_VECTOR,
	VAR_TYPE_ERROR
} VariableType;

char* variableTypeStrArr[] = {
	"NULL",
	"NUMBER",
	"PERCENT",
	"MATRIX",
	"VECTOR",
	"ERROR"
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

void Variable_print(Variable* var, bool withTypeName) {
	if (var == NULL || var->type == VAR_TYPE_NULL) {
		printf("NULL\n");
		return;
	}
	if(withTypeName) {
		if (var->name != NULL)
			printf("%s %s = ", variableTypeStrArr[var->type], var->name);
		else
			printf("%s = ", variableTypeStrArr[var->type]);
	}
	switch(var->type) {

	case VAR_TYPE_MATRIX:
		Matrix_print(var->data);
		break;
	case VAR_TYPE_NUMBER:
		printf("%g\n", *(double*)var->data);
		break;
	case VAR_TYPE_PERCENT:
		printf("%g%\n", *(double*)var->data);
		break;
	case VAR_TYPE_ERROR:
		printf("%s%\n", (char*)var->data);
	default:
		printf("BUG: IMPOSSIBLE VARIABLE TYPE!\n");
		break;
	}
}

void* Variable_copy_data(Variable* var) {
	void* copy_data;
	switch(var->type) {
		case VAR_TYPE_NULL:
			copy_data = NULL;
			break;
		case VAR_TYPE_NUMBER:
			copy_data = malloc(sizeof(double));
			memcpy(copy_data, var->data, sizeof(double));
			break;
		case VAR_TYPE_MATRIX:
			copy_data = Matrix_copy(var->data);
			break;
		default:
			printf("BUG: IMPOSSIBLE VARIABLE TYPE!\n");
			break;
	}
	return copy_data;
}

Variable* Variable_copy(Variable* var) {
	Variable* copy = malloc(sizeof(Variable));
	copy->type = var->type;
	if(var->name != NULL) {
		copy->name = malloc(strlen(var->name)+1);
		strcpy(copy->name, var->name);
	} else copy->name = NULL;
	copy->data = Variable_copy_data(var);
	return copy;
}

void Variable_free_data(Variable* var) {
	if (var == NULL || var->data == NULL) return;
	switch(var->type) {

	case VAR_TYPE_MATRIX:
		Matrix_free(var->data);
		break;
	case VAR_TYPE_NUMBER:
	case VAR_TYPE_PERCENT:
		free(var->data);
		break;
	default:
		printf("BUG IN Variable_free_data: IMPOSSIBLE VARIABLE TYPE!\n");
		break;
	}
}

void Variable_free_data_name(Variable* var) {
	if (var == NULL) return;
	Variable_free_data(var);
	free(var->name);
}

void Variable_free(Variable* var) {
	if (var == NULL) return;
	Variable_free_data_name(var);
	free(var);
}

void Variable_free_except_name(Variable* var) {
	if (var == NULL) return;
	Variable_free_data(var);
	free(var);
}

Variable* Variable_set_name(Variable* var, char* name) {
	free(var->name);
	char* name_permanent = malloc(strlen(name)+1);
	strcpy(name_permanent, name);
	var->name = name_permanent;
	return var;
}

Variable* Variable_assign_data(Variable* dest, Variable* source) {
	dest->type = source->type;
	Variable_free_data(dest->data);
	dest->data = Variable_copy_data(source);
	return dest;
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
		Variable_free_data(var);
	}
	var->type = VAR_TYPE_MATRIX;
	var->data = matrix;
}

void* Variable_copy_void_ptr(void* ptr) {
	return Variable_copy(ptr);
}

void Variable_free_void_ptr(void* ptr) {
	Variable_free(ptr);
}

void Variable_free_data_void_ptr(void* ptr) {
	Variable_free_data(ptr);
}

void Variable_free_data_name_void_ptr(void* ptr) {
	Variable_free_data_name(ptr);
}

void Variable_free_except_name_void_ptr(void* ptr) {
	Variable_free_except_name(ptr);
}
