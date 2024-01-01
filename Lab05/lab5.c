#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int findMaxInMatrix(const int**, const int, const int);
int findMaxInMatrixParallel(const int**, const int, const int, const int);
int findMaxInMatrixParallelSegment(const int**, const int, const int, const int);
void fillMatrixRandom(int**, const int, const int);

int main() {

    const int MATRIX_SIZE_N = 10000;
    const int MATRIX_SIZE_M = 10000;

    const long MATRIX_SIZE = MATRIX_SIZE_N * MATRIX_SIZE_M;

    int i, j;
    int maxSeq, maxPar;

    double start, stop, tSeq, tPar, speedup;

    int nThreads = 2;

    int** matrix = NULL;
    matrix = (int**)malloc(MATRIX_SIZE_N * sizeof(int*));
    if (matrix == NULL) {
        fprintf(stderr, "ERR: UNABLE TO ALLOCATE MEMORY");
        return 1;
    }
    for (i = 0; i < MATRIX_SIZE_N; i++) {
        matrix[i] = (int*)malloc(MATRIX_SIZE_M * sizeof(int));
        if (matrix[i] == NULL) {
            fprintf(stderr, "ERR: UNABLE TO ALLOCATE MEMORY");
            return 1;
        }
    }

    fillMatrixRandom(matrix, MATRIX_SIZE_N, MATRIX_SIZE_M);

    start = omp_get_wtime();
    maxSeq = findMaxInMatrix(matrix, MATRIX_SIZE_N, MATRIX_SIZE_M);
    stop = omp_get_wtime();

    tSeq = stop - start;

    printf("Matrix size = %ld\n", MATRIX_SIZE);
    printf("Sequential execution\n\tElapsed: %f [s]\n\tMatrix MAX = %d\n\n", tSeq, maxSeq);

    start = omp_get_wtime();
    maxPar = findMaxInMatrixParallel(matrix, MATRIX_SIZE_N, MATRIX_SIZE_M, nThreads);
    stop = omp_get_wtime();

    tPar = stop - start;
    speedup = tSeq / tPar;

    printf("Parallel execution with %d thread(s)\n\tElapsed: %f [s]\n\tMatrix MAX = %d\nSpeedup = %f\n\n", nThreads, tPar, maxSeq, speedup);

    for (i = 0; i < MATRIX_SIZE_N; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}

int findMaxInMatrix(const int** matrix, const int rows, const int cols) {
    int i, j;
    int maxValue = INT_MIN;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            if (matrix[i][j] > maxValue) {
                maxValue = matrix[i][j];
            }
        }
    }
    return maxValue;
}

int findMaxInMatrixParallelSegment(const int** matrix, const int startRows, const int stopRows, const int cols) {
    int i, j;
    int maxValue = INT_MIN;
#pragma omp parallel for
    for (i = startRows; i < stopRows; i++) {
        for (j = 0; j < cols; j++) {
            if (matrix[i][j] > maxValue) {
                maxValue = matrix[i][j];
            }
        }
    }
    return maxValue;
}

int findMaxInMatrixParallel(const int** matrix, const int rows, const int cols, const int nThreads) {
    int maxValue = INT_MIN;
    int separator = rows / 2;
#pragma omp parallel firstprivate(separator) shared(maxValue) num_threads(nThreads)
    {
#pragma omp sections
        {
#pragma omp section
            {
                //printf("Section; thread: %d\n", omp_get_thread_num());
                int tmpMax;
                tmpMax = findMaxInMatrixParallelSegment(matrix, 0, separator, cols);
#pragma omp critical
                if (tmpMax > maxValue) {
                    maxValue = tmpMax;
                }
            }
#pragma omp section
            {
                //printf("Section; thread: %d\n", omp_get_thread_num());
                int tmpMax;
                tmpMax = findMaxInMatrixParallelSegment(matrix, separator, rows, cols);
#pragma omp critical
                if (tmpMax > maxValue) {
                    maxValue = tmpMax;
                }
            }
        }
    }
    return maxValue;
}


void fillMatrixRandom(int** matrix, const int rows, const int cols) {
    int i, j;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            matrix[i][j] = rand() % 100000;
        }
    }
}