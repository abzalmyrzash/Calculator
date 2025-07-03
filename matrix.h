#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	double* val;
	int N, M;
} Matrix;

bool Matrix_valid_indices(Matrix* A, int i, int j) {
	return i >= 0 && i < A->N && j >= 0 && j < A->M;
}

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
	matrix->val = calloc(N * M, sizeof(double));
	if (matrix->val == NULL) {
		perror("Failed to allocate matrix values");
		exit(1);
	}
	return matrix;
}

Matrix* Matrix_copy(Matrix* matrix) {
	Matrix* copy = Matrix_new(matrix->N, matrix->M);
	memcpy(copy->val, matrix->val, matrix->N * matrix->M * sizeof(double));
	return copy;
}

void Matrix_free(Matrix* matrix)
{
	if(matrix == NULL) return;
	free(matrix->val);
	free(matrix);
}

Matrix* Matrix_input(int N, int M)
{
	if (N <= 0 || M <= 0 || N > 100 || M > 100) {
		printf("ERROR: Invalid matrix size!\n");
		return NULL;
	}
	Matrix* matrix = Matrix_new(N, M);
	printf("Enter your %dx%d matrix:\n", N, M);
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			scanf("%lf", Matrix_at(matrix, i, j));
		}
	}
	char c;
	while ((c = getchar()) != '\n' && c != EOF) { }
	return matrix;
}

void Matrix_print(Matrix* A)
{
	printf("[%d, %d] {\n", A->N, A->M);
	for (int i = 0; i < A->N; i++) {
		for (int j = 0; j < A->M; j++) {
			printf("%g\t", *Matrix_at(A, i, j));
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

Matrix* Matrix_scale(Matrix* A, double c) {
	Matrix* B = Matrix_new(A->N, A->M);
	for (int i = 0; i < A->N; i++) {
		for (int j = 0; j < A->M; j++) {
			*Matrix_at(B, i, j) = *Matrix_at(A, i, j) * c;
		}
	}
	return B;
}

Matrix* Matrix_transpose(Matrix* A) {
	Matrix* B = Matrix_new(A->M, A->N);
	for (int i = 0; i < B->N; i++) {
		for (int j = 0; j < B->M; j++) {
			*Matrix_at(B, i, j) = *Matrix_at(A, j, i);
		}
	}
	return B;
}

Matrix* Matrix_kronecker(Matrix* A, Matrix* B) {
	Matrix* C = Matrix_new(A->N * B->N, A->M * B->M);
	for (int i = 0; i < A->N; i++) {
		for (int j = 0; j < A->M; j++) {
			for (int k = 0; k < B->N; k++) {
				for (int l = 0; l < B->M; l++) {
					*Matrix_at(C, i * B->N + k, j * B->M + l) =
						*Matrix_at(A, i, j) * *Matrix_at(B, k, l);
				}
			}
		}
	}
	return C;
}
