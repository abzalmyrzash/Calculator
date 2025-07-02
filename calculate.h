#pragma once
#include <math.h>
#include "mymath.h"
#include "globals.h"
#include "matrixvector.h"

Variable* get_by_indices(Variable* var, DynArr* indices) {
	Variable* element;
	if (indices->len == 1 && var->type == VAR_TYPE_VECTOR) {
		Vector* vector = var->data;
		int index = *(double*)((Variable*)indices->data)->data - 1;
		double* val = malloc(sizeof(double));
		*val = vector->val[index];
		element = Variable_new(VAR_TYPE_NUMBER, NULL, val);
	}
	else if (indices->len == 2 && var->type == VAR_TYPE_MATRIX) {
		Matrix* matrix = var->data;
		int i = *(double*)((Variable*)indices->data)->data - 1;
		int j = *(double*)((Variable*)indices->data + 1)->data - 1;
		double* val = malloc(sizeof(double));
		*val = *Matrix_at(matrix, i, j);
		element = Variable_new(VAR_TYPE_NUMBER, NULL, val);
	}
	else {
		printf("ERROR: Invalid indices!\n");
		return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
	}
	return element;
}

Variable* convert_list(DynArr* dimensions, Variable* list) {
	
	Variable* arr = ((DynArr*)list->data)->data;
	int len = ((DynArr*)list->data)->len;
	double* values = malloc(sizeof(double)*len);
	for (int i = 0; i < len; i++) {
		if (arr[i].type != VAR_TYPE_NUMBER) {
			goto return_error;
		} else {
			values[i] = *(double*)arr[i].data;
		}
	}
	Variable* var;
	if (dimensions->len == 1) {
		int dim = *(double*)((Variable*)dimensions->data)->data;
		if (dim != len) {
			goto return_error;
		}
		Vector* vector = Vector_new(dim);
		vector->val = values;
		var = Variable_new(VAR_TYPE_VECTOR, NULL, vector);
	}
	else if (dimensions->len == 2) {
		int dim1 = *(double*)((Variable*)dimensions->data)->data;
		int dim2 = *(double*)((Variable*)dimensions->data + 1)->data;
		if (dim1 * dim2 != len) {
			goto return_error;
		}
		Matrix* matrix = Matrix_new(dim1, dim2);
		matrix->val = values;
		var = Variable_new(VAR_TYPE_MATRIX, NULL, matrix);
	}
	else {
		goto return_error;
	}
	return var;
return_error:
	printf("ERROR: invalid dimensions!\n");
	free(values);
	return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
}

Variable* calculate_function(char* func, Variable* arg) {
	if(arg == NULL) {
		printf("ERROR: function without arguments!\n");
		return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
	}

	if(strsame(func, "sqrt")) {
		if (arg->type != VAR_TYPE_NUMBER) {
			printf("ERROR: sqrt requires a number!\n");
		}
		double* res = malloc(sizeof(double));
		*res = sqrt(*(double*)arg->data);
		return Variable_new(VAR_TYPE_NUMBER, NULL, res);
	}
	
	if(strsame(func, "sin")) {
		if (arg->type != VAR_TYPE_NUMBER) {
			printf("ERROR: sin requires a number!\n");
		}
		double* res = malloc(sizeof(double));
		*res = sin(*(double*)arg->data);
		return Variable_new(VAR_TYPE_NUMBER, NULL, res);
	}

	if(strsame(func, "cos")) {
		if (arg->type != VAR_TYPE_NUMBER) {
			printf("ERROR: cos requires a number!\n");
		}
		double* res = malloc(sizeof(double));
		*res = cos(*(double*)arg->data);
		return Variable_new(VAR_TYPE_NUMBER, NULL, res);
	}

	if(strsame(func, "tan")) {
		if (arg->type != VAR_TYPE_NUMBER) {
			printf("ERROR: tan requires a number!\n");
		}
		double* res = malloc(sizeof(double));
		*res = tan(*(double*)arg->data);
		return Variable_new(VAR_TYPE_NUMBER, NULL, res);
	}
		
	if(strsame(func, "ln")) {
		if (arg->type != VAR_TYPE_NUMBER) {
			printf("ERROR: ln requires a number!\n");
		}
		double* res = malloc(sizeof(double));
		*res = log(*(double*)arg->data);
		return Variable_new(VAR_TYPE_NUMBER, NULL, res);
	}
		
	if(strsame(func, "log")) {
		Variable* arr;
		size_t len;
		if (arg->type == VAR_TYPE_LIST) {
			arr = ((DynArr*)arg->data)->data;
			len = ((DynArr*)arg->data)->len;
			if (len == 2) {
				if (arr[0].type == VAR_TYPE_NUMBER &&
						arr[1].type == VAR_TYPE_NUMBER) {
					goto calculate_log;
				}
			}
		}
		printf("ERROR: log requires 2 numbers!\n");
		return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
	calculate_log:
		double* res = malloc(sizeof(double));
		*res = log(*(double*)arr[0].data) / log(*(double*)arr[1].data);
		return Variable_new(VAR_TYPE_NUMBER, NULL, res);
	}
		
	printf("ERROR: Invalid function!\n");
	return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
}



Variable* calculate_operation(Variable* a, Variable* b, char* op) {
	if (a == NULL && b == NULL) {
		printf("ERROR: Operation %s with nothing!\n", op);
		return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
	}

	if (a != NULL && b == NULL) {
		if (a->type == VAR_TYPE_PERCENT) {
			a->type = VAR_TYPE_NUMBER;
			*(double*)a->data /= 100;
		}

		if (a->type == VAR_TYPE_NUMBER) {
			if(a->name != NULL) {
				Variable* var = HashMap_search(hashmap, a->name);
				if(strsame(op, "++")) {
					*(double*)var->data = *(double*)a->data + 1;
					return Variable_copy(a);
				}
				else if(strsame(op, "--")) {
					*(double*)var->data = *(double*)a->data - 1;
					return Variable_copy(a);
				}
			}
			double* res = malloc(sizeof(double));
			if (strsame(op, "!")) {
				*res = factorial(*(double*)a->data);
				return Variable_new(VAR_TYPE_NUMBER, NULL, res);
			}
			if(strsame(op, "%")) {
				*res = *(double*)a->data;
				return Variable_new(VAR_TYPE_PERCENT, NULL, res);
			}
			else {
				free(res);
				printf("ERROR: Operation %s is invalid for a number!\n", op);
				return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
			}
		}

		else if (a->type == VAR_TYPE_MATRIX) {
			Matrix* res;
			if (strsame(op, "'")) {
				res = Matrix_transpose(a->data);
				return Variable_new(VAR_TYPE_MATRIX, NULL, res);
			}
			else {
				printf("ERROR: Operation %s is invalid for a matrix!\n", op);
				return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
			}
		}

		printf("ERROR: Invalid variable type!\n");
		return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
	}		

	if (a == NULL && b != NULL) {
		if (b->type == VAR_TYPE_PERCENT) {
			b->type = VAR_TYPE_NUMBER;
			*(double*)b->data /= 100;
		}

		if (b->type == VAR_TYPE_NUMBER) {
			if(b->name != NULL) {
				Variable* var = HashMap_search(hashmap, b->name);
				if(strsame(op, "++")) {
					*(double*)var->data = *(double*)b->data + 1;
					return Variable_copy(var);
				}
				else if(strsame(op, "--")) {
					*(double*)var->data = *(double*)b->data - 1;
					return Variable_copy(var);
				}
			}
			double* res = malloc(sizeof(double));
			if(strsame(op, "-")) {
				*res = -(*(double*)b->data);
				return Variable_new(VAR_TYPE_NUMBER, NULL, res);
			}
			else {
				free(res);
				printf("ERROR: Operation %s is invalid for a number!\n", op);
				return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
			}
		}
		else if (b->type == VAR_TYPE_MATRIX) {
			Matrix *res;
			if(strsame(op, "-")) {
				res = Matrix_scale(b->data, -1);
				return Variable_new(VAR_TYPE_MATRIX, NULL, res);
			}
			else {
				printf("ERROR: Operation %s is invalid for a matrix!\n", op);
				return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
			}
		}
		printf("ERROR: Invalid variable type!\n");
		return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
	}

	if (strsame(op, ",")) {
		DynArr* list;
		if (a->type == VAR_TYPE_LIST) {
			list = (DynArr*)(a->data);
			DynArr_append(list, b);
			a->data = NULL;
		}
		else {
			list = DynArr_new(sizeof(Variable), 2, DynArrVarFunc);
			DynArr_append(list, a);
			DynArr_append(list, b);
		}
		return Variable_new(VAR_TYPE_LIST, NULL, list);
	}

	if (a->type == VAR_TYPE_NUMBER && b->type == VAR_TYPE_NUMBER) {
		double* res = malloc(sizeof(double));
		double a_val = *(double*)a->data;
		double b_val = *(double*)b->data;
		if (strsame(op, "+")) {
			*res = a_val + b_val;
		}
		else if (strsame(op, "-")) {
			*res = a_val - b_val;
		}
		else if (strsame(op, "*")) {
			*res = a_val * b_val;
		}
		else if (strsame(op, "/")) {
			if (b_val == 0) {
				printf("ERROR: Division by zero!\n");
				return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
			}
			*res = a_val / b_val;
		}

		else if (strsame(op, "//")) {
			if (b_val == 0) {
				printf("ERROR: Division by zero!\n");
				return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
			}
			*res = floor(a_val / b_val);
		}
		else if (strsame(op, "%%")) {
			if (b_val == 0) {
				printf("ERROR: Division by zero!\n");
				return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
			}
			*res = a_val - b_val * floor(a_val / b_val);
		}
		else if (strsame(op, "^")) {
			*res = pow(a_val, b_val);
		}
		else {
			free(res);
			printf("ERROR: Operation %s is invalid for two numbers!\n", op);
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
		}
		return Variable_new(VAR_TYPE_NUMBER, NULL, res);
	}

	else if (a->type == VAR_TYPE_NUMBER && b->type == VAR_TYPE_MATRIX) {
		Matrix* res;
		if (strsame(op, "*")) {
			res = Matrix_scale(b->data, *(double*)a->data);
		}
		else {
			printf("ERROR: Operation %s is invalid for a number and matrix!\n", op);
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
		}
		return Variable_new(VAR_TYPE_MATRIX, NULL, res);
	}

	else if (a->type == VAR_TYPE_MATRIX && b->type == VAR_TYPE_NUMBER) {
		Matrix* res;
		if (strsame(op, "*")) {
			res = Matrix_scale(a->data, *(double*)b->data);
		}
		else {
			printf("ERROR: Operation %s is invalid for a matrix and number!\n", op);
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
		}
		return Variable_new(VAR_TYPE_MATRIX, NULL, res);
	}

	else if (a->type == VAR_TYPE_MATRIX && b->type == VAR_TYPE_MATRIX) {
		Matrix* res;
		if (strsame(op, "+")) {
			res = Matrix_add(a->data, b->data);
		}
		else if (strsame(op, "-")) {
			res = Matrix_subtract(a->data, b->data);
		}
		else if (strsame(op, "*")) {
			res = Matrix_multiply(a->data, b->data);
		}
		else if (strsame(op, "**")) {
			res = Matrix_kronecker(a->data, b->data);
		}
		else {
			printf("ERROR: Operation %s is invalid for two matrices!\n", op);
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
		}
		if (res == NULL) {
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
		}
		return Variable_new(VAR_TYPE_MATRIX, NULL, res);
	}

	else if (a->type == VAR_TYPE_VECTOR && b->type == VAR_TYPE_VECTOR) {
		Vector* c;
		if (strsame(op, "+")) {
			c = Vector_add(a->data, b->data);
		}
		else if (strsame(op, "-")) {
			c = Vector_subtract(a->data, b->data);
		}
		else if (strsame(op, "*")) {
			double *res = Vector_dot_product(a->data, b->data);
			if (res == NULL) {
				return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
			}
			return Variable_new(VAR_TYPE_NUMBER, NULL, res);
		}
		else {
			printf("ERROR: Operation %s is invalid for two vectors!\n", op);
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
		}
		if (c == NULL) {
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
		}
		return Variable_new(VAR_TYPE_VECTOR, NULL, c);
	}

	else if (a->type == VAR_TYPE_VECTOR && b->type == VAR_TYPE_NUMBER) {
		Vector* c;
		if (strsame(op, "*")) {
			c = Vector_scale(a->data, *(double*)b->data);
		}
		else if (strsame(op, "/")) {
			c = Vector_scale(a->data, 1/(*(double*)b->data) );
		}
		else {
			printf("ERROR: Operation %s is invalid for a vector and number!\n", op);
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
		}
		return Variable_new(VAR_TYPE_VECTOR, NULL, c);
	}

	else if (b->type == VAR_TYPE_VECTOR && a->type == VAR_TYPE_NUMBER) {
		Vector* c;
		if (strsame(op, "*")) {
			c = Vector_scale(b->data, *(double*)a->data);
		}
		else {
			printf("ERROR: Operation %s is invalid for a vector and number!\n", op);
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
		}
		return Variable_new(VAR_TYPE_VECTOR, NULL, c);
	}

	else if (a->type == VAR_TYPE_MATRIX && b->type == VAR_TYPE_VECTOR) {
		Vector* c;
		if (strsame(op, "*")) {
			c = MatrixVector_multiply(a->data, b->data);
		}
		else {
			printf("ERROR: Operation %s is invalid for a matrix and vector!\n", op);
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
		}
		if (c == NULL) {
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
		}
		return Variable_new(VAR_TYPE_VECTOR, NULL, c);
	}

	else if (a->type == VAR_TYPE_VECTOR && b->type == VAR_TYPE_MATRIX) {
		Matrix* c;
		if (strsame(op, "*")) {
			c = VectorMatrix_multiply(a->data, b->data);
		}
		else {
			printf("ERROR: Operation %s is invalid for a vector and matrix!\n", op);
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
		}
		if (c == NULL) {
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
		}
		return Variable_new(VAR_TYPE_MATRIX, NULL, c);
	}

	else if (a->type == VAR_TYPE_NUMBER && b->type == VAR_TYPE_PERCENT) {
		double *res = malloc(sizeof(double));
		double number = *(double*)a->data;
		double percent = *(double*)b->data;
		if (strsame(op, "+")) {
			*res = number * (1 + percent/100);
		}
		else if (strsame(op, "-")) {
			*res = number * (1 - percent/100);
		}
		else {
			b->type = VAR_TYPE_NUMBER;
			*(double*)b->data /= 100;
			return calculate_operation(a, b, op);
		}
		return Variable_new(VAR_TYPE_NUMBER, NULL, res);
	}

	else if (a->type == VAR_TYPE_PERCENT) {
		a->type = VAR_TYPE_NUMBER;
		*(double*)a->data /= 100;
		return calculate_operation(a, b, op);
	}

	printf("ERROR: Invalid variable type!\n");
	return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
}

