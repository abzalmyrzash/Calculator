#pragma once
#include "hashmap.h"
#include "variable.h"

HashMap* hashmap;
Variable* memory;

void init_globals() {
	hashmap = HashMap_new(8);
	memory = Variable_new(VAR_TYPE_NULL, "mem", NULL);
	HashMap_insert(hashmap, "mem", memory);
}

