#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
	size_t N;
	double* val;
} Vector;

Vector* Vector_new(size_t N) {
	Vector* v = malloc(sizeof(Vector));
	v->N = N;
	v->val = calloc(N, sizeof(Vector));
	return v;
}

bool Vector_valid_index(Vector* v, int i) {
	return i >= 0 && i < v->N;
}

double Vector_magnitude(Vector* v) {
	double res = 0;
	for (size_t i = 0; i < v->N; i++) {
		res += sqrt(v->val[i]);
	}
	return res;
}

Vector* Vector_input(size_t N) {
	if (N <= 0 || N > 100) {
		printf("ERROR: Invalid vector size!\n");
		return NULL;
	}
	Vector* v = Vector_new(N);
	printf("Input your [%d] vector:\n", N);
	for (size_t i = 0; i < N; i++) {
		scanf("%lf", (v->val + i));
	}
	char c;
	while ((c = getchar()) != '\n' && c != EOF) { }
	return v;
}

void Vector_print(Vector* v) {
	printf("[%d] {", v->N);
	for (size_t i = 0; i < v->N; i++) {
		printf("%g", v->val[i]);
		if (i < v->N - 1) printf(", ");
	}
	printf("}\n");
}

void Vector_free(Vector* v) {
	if(v == NULL) return;
	free(v->val);
	free(v);
}

Vector* Vector_copy(Vector* v) {
	Vector* copy = Vector_new(v->N);
	memcpy(copy->val, v->val, v->N * sizeof(double));
	return copy;
}

static bool Vector_check_sizes(Vector* a, Vector* b) {
	if (a->N != b->N) {
		printf("ERROR: vectors must have the same size!\n");
		return false;
	}
	return true;
}

Vector* Vector_add(Vector* a, Vector* b) {
	if(!Vector_check_sizes(a, b)) return NULL;
	Vector* c = Vector_new(a->N);
	for (size_t i = 0; i < a->N; i++) {
		c->val[i] = a->val[i] + b->val[i];
	}
	return c;
}

Vector* Vector_subtract(Vector* a, Vector* b) {
	if(!Vector_check_sizes(a, b)) return NULL;
	Vector* c = Vector_new(a->N);
	for (size_t i = 0; i < a->N; i++) {
		c->val[i] = a->val[i] - b->val[i];
	}
	return c;
}

double* Vector_dot_product(Vector* a, Vector* b) {
	if(!Vector_check_sizes(a, b)) return NULL;
	double* res = calloc(1, sizeof(double));
	for (size_t i = 0; i < a->N; i++) {
		*res += a->val[i] * b->val[i];
	}
	return res;
}

Vector* Vector_scale(Vector* v, double c) {
	Vector* res = Vector_new(v->N);
	for(size_t i = 0; i < v->N; i++) {
		res->val[i] = v->val[i] * c;
	}
	return res;
}
