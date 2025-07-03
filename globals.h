#pragma once
#include "hashmap.h"
#include "variable.h"

HashMap* hashmap;
Variable* memory;
bool quit = false;
bool debug = false;
bool autoprint = true;
#define SPECIAL_VAR_NAMES_SIZE 4
#define CONST_VAR_NAMES_SIZE 3
char* special_var_names[] = {"mem", "pi", "deg", "e"};
char* const_var_names[] = {"pi", "deg", "e"};

void init_globals() {
	hashmap = HashMap_new(16);

	memory = Variable_new(VAR_TYPE_NULL, "mem", NULL);
	HashMap_insert(hashmap, memory->name, memory);

	double* pi_value = malloc(sizeof(double));
	*pi_value = M_PI;
	Variable* pi = Variable_new(VAR_TYPE_NUMBER, "pi", pi_value);
	HashMap_insert(hashmap, pi->name, pi);

	double* deg_value = malloc(sizeof(double));
	*deg_value = M_PI / 180;
	Variable* deg = Variable_new(VAR_TYPE_NUMBER, "deg", deg_value);
	HashMap_insert(hashmap, deg->name, deg);

	double* e_value = malloc(sizeof(double));
	*e_value = M_E;
	Variable* e = Variable_new(VAR_TYPE_NUMBER, "e", e_value);
	HashMap_insert(hashmap, e->name, e);
}

void free_globals() {
	HashMap_free(hashmap, Variable_free_except_name_void_ptr);
}

Variable* get_var_by_name(char *name)
{
	return (Variable*)HashMap_search(hashmap, name);
}

// copy contents of var to another variable that has specified name
Variable* copy_var_to_name(Variable* var, char *name)
{
	void* data_copy = Variable_copy_data(var);
	Variable* dest = get_var_by_name(name); 
	if (dest == NULL) { // if var with such name doesn't exit yet, create
		dest = Variable_new(var->type, name, data_copy);
		HashMap_insert(hashmap, dest->name, dest);
	}
	else { // if exists, free the data, and assign to the copy
		Variable_free_data(dest);
		dest->type = var->type;
		dest->data = data_copy;
	}
	return dest;
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

