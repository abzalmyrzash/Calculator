#pragma once
#include "variable.h"
#include "dynarr.h"

void DynArrVar_print(void* data, int len) {
	for (int i = 0; i < len; i++) {
		Variable_print(data + i, true);
	}
}

DynArrFunc DynArrVarFunc = {
	DynArrVar_print,
	Variable_copy_void_ptr, 
	Variable_free_void_ptr
};

