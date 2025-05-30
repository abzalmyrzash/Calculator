#pragma once
#include <stdlib.h>
#include <stdio.h>

typedef struct {
	double* val;
	int N, M;
} Matrix;

double* Matrix_at(Matrix* A, int i, int j)
{
	return A->val + i * A->M + j;
}

Matrix* Matrix_new(int N, int M)
{
	Matrix* matrix = malloc(sizeof(Matrix));
	if (matrix == NULL) {
		perror("Failed to allocate matrix");
		exit(1);
	}
	matrix->N = N;
	matrix->M = M;
	matrix->val = malloc(N * M * sizeof(double));
	if (matrix->val == NULL) {
		perror("Failed to allocate matrix values");
		exit(1);
	}
	return matrix;
}

void Matrix_free(Matrix* matrix)
{
	if(matrix == NULL) return;
	free(matrix->val);
	free(matrix);
}

void Matrix_print(Matrix* A)
{
	printf("(%d %d) ", A->N, A->M);
	printf("{\n");
	for (int i = 0; i < A->N; i++) {
		for (int j = 0; j < A->M; j++) {
			printf("%f ", *Matrix_at(A, i, j));
		}
		printf("\n");
	}
	printf("}\n");
}

Matrix* Matrix_add(Matrix* A, Matrix* B)
{
	if (A->N != B->N || A->M != B->M) {
		printf("ERROR: Wrong sizes for matrix addition! ");
		printf("(%d %d; %d %d)\n", A->N, A->M, B->N, B->M);
		return NULL;
	}
	Matrix* C = Matrix_new(A->N, B->M);
	for(int i = 0; i < A->N; i++) {
		for (int j = 0; j < B->M; j++) {
			*Matrix_at(C, i, j) = *Matrix_at(A, i, j) + *Matrix_at(B, i, j);
		}
	}
	return C;
}

Matrix* Matrix_subtract(Matrix* A, Matrix* B)
{
	if (A->N != B->N || A->M != B->M) {
		printf("ERROR: Wrong sizes for matrix subtraction! ");
		printf("(%d %d; %d %d)\n", A->N, A->M, B->N, B->M);
		return NULL;
	}
	Matrix* C = Matrix_new(A->N, B->M);
	for(int i = 0; i < A->N; i++) {
		for (int j = 0; j < B->M; j++) {
			*Matrix_at(C, i, j) = *Matrix_at(A, i, j) - *Matrix_at(B, i, j);
		}
	}
	return C;
}

Matrix* Matrix_multiply(Matrix* A, Matrix* B)
{
	if (A->M != B->N) {
		printf("ERROR: Wrong sizes for matrix multiplication! ");
		printf("(%d %d; %d %d)\n", A->N, A->M, B->N, B->M);
		return NULL;
	}
	Matrix* C = Matrix_new(A->N, B->M);
	for(int i = 0; i < A->N; i++) {
		for (int j = 0; j < B->M; j++) {
			for(int k = 0; k < A->M; k++){
				*Matrix_at(C, i, j) += *Matrix_at(A, i, k) * *Matrix_at(B, k, j);
			}
		}
	}
	return C;
}

