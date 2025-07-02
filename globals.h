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
	hashmap = HashMap_new(8);

	char* mem_name = malloc(4);
	strcpy(mem_name, "mem");
	memory = Variable_new(VAR_TYPE_NULL, mem_name, NULL);

	HashMap_insert(hashmap, mem_name, memory);

	char* pi_name = malloc(3);
	strcpy(pi_name, "pi");
	double* pi_value = malloc(sizeof(double));
	*pi_value = M_PI;
	Variable* pi = Variable_new(VAR_TYPE_NUMBER, pi_name, pi_value);

	HashMap_insert(hashmap, pi_name, pi);

	char* deg_name = malloc(4);
	strcpy(deg_name, "deg");
	double* deg_value = malloc(sizeof(double));
	*deg_value = M_PI / 180;
	Variable* deg = Variable_new(VAR_TYPE_NUMBER, deg_name, deg_value);

	HashMap_insert(hashmap, deg_name, deg);

	char * e_name = malloc(2);
	strcpy(e_name, "e");
	double* e_value = malloc(sizeof(double));
	*e_value = M_E;
	Variable* e = Variable_new(VAR_TYPE_NUMBER, e_name, e_value);

	HashMap_insert(hashmap, e_name, e);
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

