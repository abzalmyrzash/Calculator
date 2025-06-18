#pragma once
#include <math.h>
#include "mymath.h"
#include "globals.h"

Variable* calculate(Variable* a, Variable* b, char* op) {
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
		else {
			printf("ERROR: Operation %s is invalid for two matrices!\n", op);
			return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
		}
		return Variable_new(VAR_TYPE_MATRIX, NULL, res);
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
			return calculate(a, b, op);
		}
		return Variable_new(VAR_TYPE_NUMBER, NULL, res);
	}

	else if (a->type == VAR_TYPE_PERCENT) {
		a->type = VAR_TYPE_NUMBER;
		*(double*)a->data /= 100;
		return calculate(a, b, op);
	}

	printf("ERROR: Invalid variable type!\n");
	return Variable_new(VAR_TYPE_ERROR, NULL, NULL);
}

