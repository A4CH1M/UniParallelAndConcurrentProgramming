#include <omp.h>
#include <stdio.h>
#include <math.h>

int sumAbsoluteOfSquareMatrixProduct(const int**, const int);
int sumAbsoluteOfSquareMatrixProductParallel(const int**, const int, const int);

int main() {

	int i, j, k, o, matrixSum, numThreads;
	double start, stop, time, tSeq = 0.0f, speedup;
	int testValues[] = { 100, 174, 245, 347 };

	//int testValues[] = { 10000, 30000, 60000, 120000 };
	//
	//int** matrix = (int**)malloc(N * sizeof(int*));
	//for (i = 0; i < N; i++) {
	//	matrix[i] = (int*)malloc(N * sizeof(int));
	//	for (j = 0; j < N; j++) {
	//		if (j % 2 == 0) {
	//			matrix[i][j] = 1;
	//		}
	//		else {
	//			matrix[i][j] = -1;
	//		}
	//	}
	//}
	//
	////for (i = 0; i < N; i++) {
	////	for (j = 0; j < N; j++) {
	////		printf("%d ", matrix[i][j]);
	////	}
	////	printf("\n");
	////}
	//
	//start = omp_get_wtime();
	//matrixSum = sumAbsoluteOfSquareMatrixProduct(matrix, N);
	//stop = omp_get_wtime();
	//printf("%d\n", matrixSum);
	//printf("start = %.30g\nend = %.30g\ndiff = %.30g\n", start, stop, stop - start);
	//
	//omp_set_num_threads(7);
	//start = omp_get_wtime();
	//matrixSum = sumAbsoluteOfSquareMatrixProductParallel(matrix, N);
	//stop = omp_get_wtime();
	//printf("%d\n", matrixSum);
	//printf("start = %.30g\nend = %.30g\ndiff = %.30g\n", start, stop, stop - start);
	//
	//for (i = 0; i < N; i++) {
	//	free(matrix[i]);
	//}
	//free(matrix);
	//
	//
	//for (o = 0; o < 4; o++) {
	//	//const long N = 100 * pow(10, o);
	//	const int N = testValues[o];
	//
	//	int** matrix = (int**)malloc(N * sizeof(int*));
	//	for (i = 0; i < N; i++) {
	//		matrix[i] = (int*)malloc(N * sizeof(int));
	//		for (j = 0; j < N; j++) {
	//			if (j % 2 == 0) {
	//				matrix[i][j] = 1;
	//			}
	//			else {
	//				matrix[i][j] = -1;
	//			}
	//		}
	//	}
	//
	//	printf("For %d elements:\n\n", (int)pow(N, 2));
	//	start = omp_get_wtime();
	//	matrixSum = sumAbsoluteOfSquareMatrixProduct(matrix, N);
	//	stop = omp_get_wtime();
	//	if (matrixSum != (int)pow(N, 2)) {
	//		printf("Err: wrong result");
	//		break;
	//	}
	//
	//	tSeq = stop - start;
	//
	//	printf("Sequential execution\n\tElapsed: %f [s]\n\n", tSeq);
	//	//printf("==================================================================\n\n");
	//	//printf("%d\n", matrixSum);
	//	//printf("start = %.30g\nend = %.30g\ndiff = %.30g\n", start, stop, stop - start);
	//
	//	for (k = 0; k < 4; k++) {
	//
	//		numThreads = 1 << k;
	//		//printf("Threads: %d\n\n", numThreads);
	//		//omp_set_num_threads(numThreads);
	//
	//		start = omp_get_wtime();
	//		matrixSum = sumAbsoluteOfSquareMatrixProductParallel(matrix, N, numThreads);
	//		stop = omp_get_wtime();
	//
	//		if (matrixSum != (int)pow(N, 2)) {
	//			printf("Err: wrong result");
	//			break;
	//		}
	//		//printf("Result: %d\n", matrixSum);
	//		time = stop - start;
	//		//if (numThreads == 1) {
	//		//	tSeq = time;
	//		//}
	//
	//		speedup = tSeq / time;
	//
	//		printf("Parallel execution for %d threads\n\tElapsed: %f [s]\n\tSpeedup: %f\n\n", numThreads, time, speedup);
	//		//printf("Parallel execution for %d threads\n\tElapsed: %f [s]\n\tSpeedup: %f\n\n", numThreads, time, speedup);
	//	}
	//
	//	printf("==================================================================\n\n");
	//
	//	for (i = 0; i < N; i++) {
	//		free(matrix[i]);
	//	}
	//	free(matrix);
	//}

	//const int N = testValues[3];
	const int N = 1000;
	numThreads = 2;

	int** matrix = (int**)malloc(N * sizeof(int*));
	for (i = 0; i < N; i++) {
		matrix[i] = (int*)malloc(N * sizeof(int));
		for (j = 0; j < N; j++) {
			if (j % 2 == 0) {
				matrix[i][j] = 1;
			}
			else {
				matrix[i][j] = -1;
			}
		}
	}


	printf("For %d elements:\n\n", (int)pow(N, 2));
	start = omp_get_wtime();
	matrixSum = sumAbsoluteOfSquareMatrixProduct(matrix, N);
	stop = omp_get_wtime();
	tSeq = stop - start;

	printf("Sequential execution\n\tElapsed: %f [s]\n\n", tSeq);


	start = omp_get_wtime();
	matrixSum = sumAbsoluteOfSquareMatrixProductParallel(matrix, N, numThreads);
	stop = omp_get_wtime();

	time = stop - start;
	speedup = tSeq / time;

	printf("Parallel execution for %d threads\n\tElapsed: %f [s]\n\tSpeedup: %f\n\n", numThreads, time, speedup);

	for (i = 0; i < N; i++) {
		free(matrix[i]);
	}
	free(matrix);

	return 0;
}

int sumAbsoluteOfSquareMatrixProduct(const int** matrix, const int n) {
	int sum = 0;
	int i, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			sum += abs(matrix[i][j]);
		}
	}

	return sum;
}

int sumAbsoluteOfSquareMatrixProductParallel(const int** matrix, const int n, const int threads) {
	int sum = 0;
	int i, j;
#pragma omp parallel reduction(+:sum) private(i, j) num_threads(threads)
	{
#pragma omp for
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++) {
				sum += abs(matrix[i][j]);
			}
			// printf("Thread: %d\ti: %d\t j: %d\tcurrent sum: %d\n", omp_get_thread_num(), i, j, sum);
		}
		//printf("Thread: %d\tthread sum: %d\n", omp_get_thread_num(), sum);
	}
	return sum;
}