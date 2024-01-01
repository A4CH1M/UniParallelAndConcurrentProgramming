#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define VECTOR_TEST_SIZE 8

void ex21nj();
void ex21j();
void ex22nj();
void ex22j();
void ex23();
void ex24(int* vec1, int* vec2, int* sum, int n);
int ex25(int* vec1, int* vec2, int n);
void ex26(int matrix1[][2], int matrix2[][2], int resultMatrix[][2]);
void ex27(int matrix[][2], int* vec, int* resultVec);
int ex28(int* vec, int vecSize);
void ex29();
void ex210();
void ex211();
void ex212();

void printVector(int* vector, int vectorSize) {
	for (int i = 0; i < vectorSize; i++) {
		printf("%d ", vector[i]);
	}
}

void printMatrix2x2(int matrix2x2[2][2]) {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			printf("%d\t", matrix2x2[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char* argv[]) {
	int i;
	omp_set_num_threads(3);


	/* Fork a team of threads giving them their own copies of variables */
//#pragma omp parallel if(1) private(nthreads, tId) num_threads(4)
//	{
//		/* Obtain thread number */
//		tId = omp_get_thread_num();
//		printf("Hello World from thread = %d\n", tId);
//		/* Only master thread does this */
//		if (tId == 0) {
//			nthreads = omp_get_num_threads();
//			printf("Number of threads = %d\n", nthreads);
//		}
//	} /* All threads join master thread and disband */

	// wa¿noœæ w kolejnoœci:
	// klauzula if
	// klauzula dyrektywy parallel
	// funkcja biblioteki OpenMP
	// zmienna œrodowiskowa

	printf("Ex 2.1:\n");
	ex21nj();
	printf("\n");
	ex21j();
	printf("\n");
	printf("Ex 2.2:\n");
	ex22nj();
	printf("\n");
	ex22j();
	printf("\n");
	printf("Ex 2.3:\n");
	ex23();
	printf("\n");

	printf("Ex 2.4:\n");
	int vec1[VECTOR_TEST_SIZE] = { 5, 8, 3, 2, 4, 5, 6, 1 };
	int vec2[VECTOR_TEST_SIZE] = { 0, 1, 4, 7, 3, 4, 2, 8 };

	int sumStatic[16] = { 0 };
	int sumDynamic[16] = { 0 };

	printf("Vector 1:\n");
	printVector(&vec1, VECTOR_TEST_SIZE);
	printf("\n");
	printf("Vector 2:\n");
	printVector(&vec2, VECTOR_TEST_SIZE);
	printf("\n");

#pragma omp parallel for
	for (i = 0; i < VECTOR_TEST_SIZE; i++)
	{
		int tId = omp_get_thread_num();
		sumStatic[i] = vec1[i] + vec2[i];
		printf("Thread: %d;\tv1[%d] = %d;\tv2[%d] = %d\n", tId, i, vec1[i], i, vec2[i]);
	}

#pragma omp parallel
	{
		ex24(&vec1, &vec2, &sumDynamic, VECTOR_TEST_SIZE);
	}

	printf("Sum static:\n");
	printVector(&sumStatic, VECTOR_TEST_SIZE);
	printf("\n");
	printf("Sum dynamic:\n");
	printVector(&sumDynamic, VECTOR_TEST_SIZE);
	printf("\n");

	printf("Ex 2.5:\n");
	int dotProduct = ex25(&vec1, &vec2, VECTOR_TEST_SIZE);
	printf("Dot Product: %d\n\n", dotProduct);

	printf("Ex 2.6:\n");
	int matrix1[2][2] = { {2, 4}, {1, 3} };
	int matrix2[2][2] = { {3, 5}, {5, 2} };
	int matrixResult[2][2] = { 0 };

	printf("Matrix1:\n");
	printMatrix2x2(matrix1);
	printf("\n");

	printf("Matrix2:\n");
	printMatrix2x2(matrix2);
	printf("\n");

#pragma omp parallel
	{
		ex26(matrix1, matrix2, matrixResult, 2);
	}

	printf("Matrix1 x Matrix2:\n");
	printMatrix2x2(matrixResult);
	printf("\n");

	printf("Ex 2.7:\n");
	int vec[2] = { 3, 4 };
	int vecResult[2] = { 0 };
	printf("Matrix:\n");
	printMatrix2x2(matrix1);
	printf("Vector:\n");
	printVector(vec, 2);
	printf("\n");

#pragma omp parallel
	{
		ex27(matrix1, vec, vecResult);
	}
	printf("Result vector:\n");
	printVector(vecResult, 2);
	printf("\n");
	printf("\n");


	printf("Ex 2.8:\n");
	int sumVec = ex28(vec1, VECTOR_TEST_SIZE);
	printf("Sum of vector = %d\n", sumVec);
	printf("\n");

	printf("Ex 2.9:\n");
	ex29();
	printf("\n");

	printf("Ex 2.10:\n");
	ex210();
	printf("\n");

	printf("Ex 2.11:\n");
	ex211();
	printf("\n");

	printf("Ex 2.12:\n");
	ex212();
	printf("\n");

	return 0;
}

void ex21nj() {
#pragma omp parallel
	{
		int tId = omp_get_thread_num();
		int privateVariable = 5;
		privateVariable += tId;
		printf("Thread: %d;\tValue: %d\n", tId, privateVariable);
	}
}

void ex21j() {
	int privateVariable;
#pragma omp parallel private(privateVariable)
	{
		int tId = omp_get_thread_num();
		privateVariable = 5;
		privateVariable += tId;
		printf("Thread: %d;\tValue: %d\n", tId, privateVariable);
	}
}

void ex22j() {
	int sharedVariable = 10;
#pragma omp parallel shared(sharedVariable)
	{
		int tId = omp_get_thread_num();
		printf("Thread: %d;\tValue: %d\n", tId, sharedVariable + tId);
	}
}

void ex22nj() {
	int sharedVariable = 10;
#pragma omp parallel 
	{
		int tId = omp_get_thread_num();
		printf("Thread: %d;\tValue: %d\n", tId, sharedVariable + tId);
	}
}

void ex23() {
	int pfp = 10;

#pragma omp parallel firstprivate(pfp)
	{
		int tId = omp_get_thread_num();
		pfp = pfp + tId;
		printf("Thread: %d;\tValue: %d\n", tId, pfp);
	}
}

void ex24(int* vec1, int* vec2, int* sum, int n) {
	int i;
#pragma omp for
	for (i = 0; i < n; i++)
	{
		sum[i] = vec1[i] + vec2[i];
	}
}

int ex25(int* vec1, int* vec2, int n) {
	int i, dotProduct = 0;
#pragma omp parallel for reduction(+:dotProduct)
	for (i = 0; i < VECTOR_TEST_SIZE; i++)
	{
		int tId = omp_get_thread_num();
		printf("Thread: %d;\tv1[%d] = %d;\tv2[%d] = %d\n", tId, i, vec1[i], i, vec2[i]);
		dotProduct += vec1[i] * vec2[i];
	}
	return dotProduct;
}

void ex26(int matrix1[][2], int matrix2[][2], int resultMatrix[][2]) {
	int i, j, k;
#pragma omp for
	for (i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				resultMatrix[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}
}

void ex27(int matrix[][2], int* vec, int* resultVec) {
	int i, j;
#pragma omp for
	for (i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++) {
			resultVec[i] += matrix[i][j] * vec[j];
		}
	}
}

int ex28(int* vec, int vecSize) {
	int sumVec = 0;
	int i;
#pragma omp parallel for reduction(+:sumVec)
	for (i = 0; i < vecSize; i++)
	{
		int tId = omp_get_thread_num();
		sumVec += vec[i];
		printf("Thread: %d;\tVector[%d]: %d;\tCurrent thread sum: %d\n", tId, i, vec[i], sumVec);
	}
	printf("After join sum = %d\n", sumVec);
	return sumVec;
}

void ex29() {
	char text[] = "This is an example sentence";
	int charCount = 0;
	int i;
#pragma omp parallel for reduction(+:charCount)
	for (i = 0; i < strlen(text); i++)
	{
		int tId = omp_get_thread_num();
		if (text[i] != ' ') {
			charCount++;
		}
		printf("Thread: %d;\tText[%d]: '%c';\tCurrent thread count: %d\n", tId, i, text[i], charCount);
	}

	printf("Text: %s\nNumber of characters: %d\n", text, charCount);
}

void ex210() {
	char text[] = "This is an example sentence";
	int wordCount = 0;
	int i;
	bool inWord = false;

#pragma omp parallel for reduction(+:wordCount)
	for (i = 0; i < strlen(text); i++)
	{
		int tId = omp_get_thread_num();

		if (text[i] == ' ' || text[i] == '\0' || text[i] == '\t' || text[i] == '\n') {
			inWord = false;
		}
		if (!inWord) {
			inWord = true;
			wordCount++;
		}

		printf("Thread: %d;\tText[%d]: '%c';\tCurrent thread count: %d\n", tId, i, text[i], wordCount);
	}

	printf("Text: %s\nNumber of words: %d\n", text, wordCount);
}

void ex211() {
	char binary = 0b10110101;
	int zeroCount = 0;
	int i;
#pragma omp parallel for reduction(+:zeroCount)
	for (i = 0; i < 8; i++)
	{
		if (!(binary & (0b1 << i))) {
			zeroCount += 1;
		}
	}

	printf("Value: 0b10110101\nNumber of zeros: %d\n", zeroCount);
}

void ex212() {
	char binary = 0b10110101;
	int oneCount = 0;
	int i;
#pragma omp parallel for reduction(+:oneCount)
	for (i = 0; i < 8; i++)
	{
		if (binary & (0b1 << i)) {
			oneCount += 1;
		}
	}

	printf("Value: 0b10110101\nNumber of ones: %d\n", oneCount);
}