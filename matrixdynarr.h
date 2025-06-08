#pragma once
#include "matrix.h"
#include "dynarr.h"

void _DynArrMatrix_print(void* data, int len) {
	for(int i = 0; i < len; i++) {
		Matrix_print((Matrix*)data + len);
	}
}

void _DynArrMatrix_free(void* ptr) {
	free((*(Matrix*)ptr)->data);
}

void* _DynArrMatrix_copy(void* ptr) {
	return Matrix_copy(ptr);
}

DynArrFunc DynArrMatrixFunc = (DynArrFunc) {
	_DynArrMatrix_print, _DynArrMatrix_copy, _DynArrMatrix_free
};
