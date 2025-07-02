#include "vector.h"
#include "dynarr.h"

void _DynArrVector_print(void* data, size_t len) {
	for(size_t i = 0; i < len; i++) {
		Vector_print((Vector*)data + len);
	}
}

void _DynArrVector_free(void* ptr) {
	free((*(Vector*)ptr)->data);
}

void* _DynArrVector_copy(void* ptr) {
	return Vector_copy(ptr);
}

DynArrFunc DynArrVectorFunc = (DynArrFunc) {
	_DynArrVector_print, _DynArrVector_copy, _DynArrVector_free
};
