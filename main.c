#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
	char* name;
	float* val;
	int N, M;
} Matrix;

float* matrix_at(Matrix A, int i, int j)
{
	return A.val + i * A.M + j;
}

void print_matrix(Matrix A)
{
	printf("%s\n{", A.name);
	for (int i = 0; i < A.N; i++) {
		for (int j = 0; j < A.M; j++) {
			printf("%f ", *matrix_at(A, i, j));
		}
		printf("\n");
	}
	printf("}\n");
}

Matrix add_matrices(Matrix A, Matrix B)
{
	if (A.N != B.N || A.M != B.M) {
		printf("ERROR: Wrong sizes for matrix addition! ");
		printf("(%d %d; %d %d)\n", A.N, A.M, B.N, B.M);
		exit(1);
	}
	float* C_values = malloc(A.N * B.M * sizeof(float));
	Matrix C = {"", C_values, A.N, B.M};
	for(int i = 0; i < A.N; i++) {
		for (int j = 0; j < B.M; j++) {
			*matrix_at(C, i, j) = *matrix_at(A, i, j) + *matrix_at(B, i, j);
		}
	}
	return C;
}

Matrix subtract_matrices(Matrix A, Matrix B)
{
	if (A.N != B.N || A.M != B.M) {
		printf("ERROR: Wrong sizes for matrix subtraction! ");
		printf("(%d %d; %d %d)\n", A.N, A.M, B.N, B.M);
		exit(1);
	}
	float* C_values = malloc(A.N * B.M * sizeof(float));
	Matrix C = {"", C_values, A.N, B.M};
	for(int i = 0; i < A.N; i++) {
		for (int j = 0; j < B.M; j++) {
			*matrix_at(C, i, j) = *matrix_at(A, i, j) - *matrix_at(B, i, j);
		}
	}
	return C;
}

Matrix multiply_matrices(Matrix A, Matrix B)
{
	if (A.M != B.N) {
		printf("ERROR: Wrong sizes for matrix multiplication! ");
		printf("(%d %d; %d %d)\n", A.N, A.M, B.N, B.M);
		exit(1);
	}
	float* C_values = malloc(A.N * B.M * sizeof(float));
	Matrix C = {"", C_values, A.N, B.M};
	for(int i = 0; i < A.N; i++) {
		for (int j = 0; j < B.M; j++) {
			for(int k = 0; k < A.M; k++){
				*matrix_at(C, i, j) += *matrix_at(A, i, k) * *matrix_at(B, k, j);
			}
		}
	}
	return C;
}

Matrix input_matrix()
{
	int N, M;
	// printf("Enter N and M: ");
	// scanf("%d %d", &N, &M);
	char* name = malloc(4*sizeof(char));
	scanf("create %3s %d %d", name, &N, &M);
	float* values = malloc(N*M*sizeof(float));
	printf("Enter your %dx%d matrix:\n", N, M);
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			scanf("%f", &values[i*M+j]);
		}
	}
	return (Matrix){name, values, N, M};
}

int main()
{
	Matrix A, B;
	A = input_matrix();
	print_matrix(A);
	B = input_matrix();
	print_matrix(B);
	printf("\n");

	print_matrix(A);
	printf("+\n");
	print_matrix(B);
	printf("=\n");
	Matrix C = add_matrices(A, B);
	print_matrix(C);
	return 0;
}
