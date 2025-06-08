#pragma once
#include "variable.h"
#include "dynarr.h"

void DynArrVar_print(void* data, int len) {
	for (int i = 0; i < len; i++) {
		Variable_print(data + i, true);
	}
}

void* DynArrVar_copy(void* ptr) {
	return Variable_copy(ptr);
}

void DynArrVar_free(void* ptr) {
	Variable* var = (Variable*)ptr;
	free(var->name);
	Variable_free_data(var);
}

DynArrFunc DynArrVarFunc = {DynArrVar_print, DynArrVar_copy, DynArrVar_free};

