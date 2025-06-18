#pragma once
#include "hashmap.h"
#include "variable.h"

HashMap* hashmap;
Variable* memory;
bool quit = false;

void init_globals() {
	hashmap = HashMap_new(8);
	char* mem_name = malloc(4);
	strcpy(mem_name, "mem");
	memory = Variable_new(VAR_TYPE_NULL, mem_name, NULL);
	HashMap_insert(hashmap, mem_name, memory);
}

void free_globals() {
	HashMap_free(hashmap, Variable_free_except_name_void_ptr);
}

Variable* get_var_by_name(char *name)
{
	return (Variable*)HashMap_search(hashmap, name);
}

Variable* copy_var_to_name(Variable* var, char *name)
{
	Variable* oldVar = get_var_by_name(name); 
	if(oldVar != NULL) {
		HashMap_delete(hashmap, name);
		Variable_free(oldVar);
	}
	Variable* copy = Variable_copy(var);
	Variable_set_name(copy, name);
	HashMap_insert(hashmap, copy->name, copy);
	return copy;
}

void delete_variable(char *name)
{
	Variable* var = get_var_by_name(name);
	if (var == NULL) {
		printf("ERROR: doesn't exist!\n");
		return;
	}
	HashMap_delete(hashmap, name);
	Variable_free(var);
}

