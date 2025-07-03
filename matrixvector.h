#include "vector.h"
#include "matrix.h"

Matrix* Vector_toMatrix(Vector* v) {
	if (v == NULL) return NULL;
	Matrix* M = malloc(sizeof(Matrix));
	M->N = v->N;
	M->M = 1;
	M->val = v->val;
	return M;
}

Vector* Matrix_toVector(Matrix* M) {
	if (M == NULL) return NULL;
	if (M->M != 1) {
		printf("ERROR in Matrix_toVector: number of columns must be 1!\n");
		return NULL;
	}
	Vector* v = malloc(sizeof(Vector));
	v->N = M->N;
	v->val = M->val;
	return v;
}

Vector* MatrixVector_multiply(Matrix* A, Vector* v) {
	Matrix* V = Vector_toMatrix(v);
	Matrix* resm = Matrix_multiply(A, V);
	Vector* res = Matrix_toVector(resm);
	free(V);
	free(resm);
	return res;
}

Matrix* VectorMatrix_multiply(Vector* v, Matrix* A) {
	Matrix* V = Vector_toMatrix(v);
	Matrix* res = Matrix_multiply(V, A);
	free(V);
	return res;
}
