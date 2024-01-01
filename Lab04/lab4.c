#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int sumOfSquareMatrixProductSequential(const int**, const int);
int sumOfSquareMatrixProductParallel(const int**, const int, const int);

int main() {

	int i, j, matrixSum;
	double start, stop, time, tSeq = 0.0f, speedup;
	//int testValues[] = { 100, 174, 245, 347 };
	int testValues[] = { 500, 1000, 10000 };

	const long N = testValues[2];
	int numThreads = 4;

	int** matrix = (int**)malloc(N * sizeof(int*));
	if (matrix == NULL) {
		return 1;
	}
	for (i = 0; i < N; i++) {
		matrix[i] = (int*)malloc(N * sizeof(int));
		if (matrix[i] == NULL) {
			return 1;
		}
		for (j = 0; j < N; j++) {
			matrix[i][j] = 1;
		}
	}

	//for (i = 0; i < N; i++) {
	//	for (j = 0; j < N; j++) {
	//		if (j % 2 == 0) {
	//			printf("%d ", matrix[i][j] = 1);
	//		}
	//	}
	//	printf("\n");
	//}


	printf("Number of elements in the matrix: %d\n\n", N * N);

	start = omp_get_wtime();
	matrixSum = sumOfSquareMatrixProductSequential(matrix, N);
	stop = omp_get_wtime();

	tSeq = stop - start;

	printf("Sequential execution\n\tElapsed: %f [s]\nMatrix sum = %d\n\n", tSeq, matrixSum);

	start = omp_get_wtime();
	matrixSum = sumOfSquareMatrixProductParallel(matrix, N, numThreads);
	stop = omp_get_wtime();

	time = stop - start;

	speedup = tSeq / time;
	printf("Parallel execution for %d threads\n\tElapsed: %f [s]\n\tSpeedup: %f\nMatrix sum = %d\n\n", numThreads, time, speedup, matrixSum);
	for (i = 0; i < N; i++) {
		free(matrix[i]);
	}
	free(matrix);

	return 0;
}

int sumOfSquareMatrixProductSequential(const int** matrix, const int n) {
	int sum = 0;
	int i, j;

	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++) {
			sum += matrix[i][j];
		}
		//printf("Thread: %d\ti: %d\t j: %d\tcurrent sum: %d\n", omp_get_thread_num(), i, j, sum);
	}
	//printf("Thread: %d\tthread sum: %d\n", omp_get_thread_num(), sum);
	return sum;
}

int sumOfSquareMatrixProductParallel(const int** matrix, const int n, const int threads) {
	int sum = 0;
	int i, j;
#pragma omp parallel reduction(+:sum) private(i, j) num_threads(threads)
	{
#pragma omp for schedule(dynamic)
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++) {
				sum += matrix[i][j];
			}
			//printf("Thread: %d\ti: %d\t j: %d\tcurrent sum: %d\n", omp_get_thread_num(), i, j, sum);
		}
		//printf("Thread: %d\tthread sum: %d\n", omp_get_thread_num(), sum);
	}
	return sum;
}