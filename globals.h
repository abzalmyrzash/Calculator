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

Variable* get_var_by_name(char *name)
{
	return (Variable*)HashMap_search(hashmap, name);
}

Variable* copy_var_with_name(Variable* var, char *name)
{
	Variable* oldVar = get_var_by_name(name); 
	if(oldVar != NULL) {
		HashMap_delete(hashmap, name);
		Variable_free(oldVar);
	}
	Variable* copy = Variable_copy(var);
	Variable_change_name(copy, name);
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

