#pragma once
#include <stdlib.h>
#include <stdio.h>
#include "matrix.h"

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
		printf("%g\n", *(double*)var->data);
		break;
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
	
void _Variable_free_data(Variable* var) {
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

void Variable_free(Variable* var) {
	_Variable_free_data(var);
	free(var->name);
	free(var);
}

Variable* Variable_change_name(Variable* var, char* name) {
	free(var->name);
	char* name_permanent = malloc(strlen(name)+1);
	strcpy(name_permanent, name);
	var->name = name_permanent;
	return var;
}

void Variable_assign_number(Variable* var, double* number) {
	if(var == NULL) {
		var = (Variable*)malloc(sizeof(Variable));
	} else {
		_Variable_free_data(var);
	}
	var->type = VAR_TYPE_NUMBER;
	var->data = number;
}

void Variable_assign_matrix(Variable* var, Matrix* matrix) {
	if(var == NULL) {
		var = (Variable*)malloc(sizeof(Variable));
	} else {
		_Variable_free_data(var);
	}
	var->type = VAR_TYPE_MATRIX;
	var->data = matrix;
}

