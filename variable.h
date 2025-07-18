#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "matrix.h"
#include "vector.h"
#include "dynarr.h"
#include "utils.h"

typedef enum {
	VAR_TYPE_NULL,
	VAR_TYPE_NUMBER,
	VAR_TYPE_PERCENT,
	VAR_TYPE_MATRIX,
	VAR_TYPE_VECTOR,
	VAR_TYPE_LIST,
	VAR_TYPE_ERROR
} VariableType;

char* variableTypeStrArr[] = {
	"NULL",
	"NUMBER",
	"PERCENT",
	"MATRIX",
	"VECTOR",
	"LIST",
	"ERROR"
};

typedef struct {
	VariableType type;
	char* name;
	void* data;
} Variable;

void Variable_set_name(Variable* var, char* name) {
	if (var->name != NULL) free(var->name);
	if (name == NULL) {
		var->name = NULL;
		return;
	}
	char* name_permanent = malloc(strlen(name)+1);
	strcpy(name_permanent, name);
	var->name = name_permanent;
	return;
}

void Variable_init(Variable* var, VariableType type, char* name, void* data) {
	var->type = type;
	var->data = data;
	var->name = NULL;
	Variable_set_name(var, name);
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
	case VAR_TYPE_VECTOR:
		Vector_print(var->data);
		break;
	case VAR_TYPE_NUMBER:
		//printf("%.17g\n", *(double*)var->data);
		print_number(*(double*)var->data);
		printf("\n");
		break;
	case VAR_TYPE_PERCENT:
		//printf("%.17g%\n", *(double*)var->data);
		print_number(*(double*)var->data);
		printf("\n");
		break;
	case VAR_TYPE_LIST:
		DynArr_print(var->data);
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
		case VAR_TYPE_VECTOR:
			copy_data = Vector_copy(var->data);
			break;
		case VAR_TYPE_LIST:
			copy_data = DynArr_copy(var->data);
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
	case VAR_TYPE_VECTOR:
		Vector_free(var->data);
		break;
	case VAR_TYPE_NUMBER:
	case VAR_TYPE_PERCENT:
		free(var->data);
		break;
	case VAR_TYPE_LIST:
		DynArr_free(var->data);
		break;
	default:
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

//////////////////////////////////////////

void DynArrVar_print(void* data, size_t len) {
	printf("%d\n", len);
	for (size_t i = 0; i < len; i++) {
		Variable_print((Variable*)data + i, true);
	}
}

DynArrFunc DynArrVarFunc = {
	DynArrVar_print,
	Variable_copy_void_ptr, 
	Variable_free_data_name_void_ptr
};

/////////////////////////////////////////

bool Variable_is_list_of_numbers(Variable* var) {
	if (var->type != VAR_TYPE_LIST) return false;
	DynArr* list = var->data;
	if (list->elem_size != sizeof(Variable)) return false;
	Variable* arr = list->data;
	for (int i = 0; i < list->len; i++) {
		if (arr[i].type != VAR_TYPE_NUMBER) {
			return false;
		}
	}
	return true;
}
