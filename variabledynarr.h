#pragma once
#include "variable.h"
#include "dynarr.h"

void DynArrVar_print(void* data, size_t len) {
	for (size_t i = 0; i < len; i++) {
		Variable_print(data + i, true);
	}
}

DynArrFunc DynArrVarFunc = {
	DynArrVar_print,
	Variable_copy_void_ptr, 
	Variable_free_data_name_void_ptr
};

