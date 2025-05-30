#pragma once
#include "stdlib.h"
#include "stdio.h"

typedef struct {
	size_t total_size;
	void* (*malloc)(size_t size);
	void* (*realloc)(void* ptr, size_t size);
	void (*free)(void* ptr);
} CustomAllocator;
