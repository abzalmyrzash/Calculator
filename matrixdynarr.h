#pragma once
#include "matrix.h"
#include "dynarr.h"

void DynArrMatrix_free(void* ptr) {
	free((*Matrix)ptr);
}

void DynArrMatrix_print(void* data, int len) {
	for(int i = 0; i < len; i++) {
		Matrix_print((*Matrix)data + len);
	}
}

DynArrFunc DynArrMatrixFunc = (DynArrFunc) {
	DynArrMatrix_print,
	DynArrMatrix_free
};
