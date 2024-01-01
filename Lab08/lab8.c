#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#define TEST_ENV 0

void matrixMultiplication(const int**, const int**, int**, const int);
void matrixMultiplicationThreads(const int**, const int**, int**, const int, int);
void printMatrix(const int**, const unsigned int);
int isNumber(const char*);

int main(int argc, char** argv) {

#if TEST_ENV
    argc = 2;
    argv[1] = "300";
#endif

    if (argc != 2) {
        fprintf(stderr, "ERR: INCORRECT NUMBER OF ARGUMENTS\n");
        return 1;
    }

    if (!isNumber(argv[1])) {
        fprintf(stderr, "ERR: ARGUMENT MUST BE A NUMBER\n");
        return 2;
    }

    int i, j;

    char* pEnd;
    const long MATRIX_SIZE = strtol(argv[1], &pEnd, 10);

    if (*pEnd != '\0' || errno == ERANGE) {
        fprintf(stderr, "ERR: INVALID ARGUMENT OR OUT OF RANGE\n");
        return 3;
    }

    int** matrixA = (int**)malloc(MATRIX_SIZE * sizeof(int*));
    int** matrixB = (int**)malloc(MATRIX_SIZE * sizeof(int*));
    int** matrixResultSeq = (int**)malloc(MATRIX_SIZE * sizeof(int*));
    int** matrixResultPar = (int**)malloc(MATRIX_SIZE * sizeof(int*));

    if (matrixA == NULL || matrixB == NULL || matrixResultSeq == NULL || matrixResultPar == NULL) {
        fprintf(stderr, "ERR: UNABLE TO ALLOCATE MEMORY\n");
        return 4;
    }

    for (i = 0; i < MATRIX_SIZE; i++) {
        matrixA[i] = (int*)malloc(MATRIX_SIZE * sizeof(int));
        matrixB[i] = (int*)malloc(MATRIX_SIZE * sizeof(int));
        matrixResultSeq[i] = (int*)malloc(MATRIX_SIZE * sizeof(int));
        matrixResultPar[i] = (int*)malloc(MATRIX_SIZE * sizeof(int));

        if (matrixA[i] == NULL || matrixB[i] == NULL || matrixResultSeq[i] == NULL || matrixResultPar[i] == NULL) {
            fprintf(stderr, "ERR: UNABLE TO ALLOCATE MEMORY\n");
            return 4;
        }

        for (j = 0; j < MATRIX_SIZE; j++) {
            matrixA[i][j] = 1;
            matrixB[i][j] = 1;
        }
    }

    double start, stop, tSeq, tPar, speedup;

    start = omp_get_wtime();
    matrixMultiplication(matrixA, matrixB, matrixResultSeq, MATRIX_SIZE);
    stop = omp_get_wtime();
    tSeq = stop - start;

    start = omp_get_wtime();
    matrixMultiplicationThreads(matrixA, matrixB, matrixResultPar, MATRIX_SIZE, 4);
    stop = omp_get_wtime();
    tPar = stop - start;
    speedup = tSeq / tPar;

#if TEST_ENV
    printf("For N = %d:\n", MATRIX_SIZE);
    printf("\tSequential time:\t%.16g\n", tSeq);
    printf("\tParallel time:\t\t%.16g\n", tPar);
    printf("\tSpeedup:\t\t%.16g\n", speedup);

    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            if (matrixResultSeq[i][j] != matrixResultPar[i][j]) {
                //takCzyNieQuestionMark = 1;
                fprintf(stderr, "ERR: Matrices are not the same");
            }
        }
    }
#endif

    fprintf(stdout, "%d;%.16g;%.16g;%.16g", MATRIX_SIZE, tSeq, tPar, speedup);

    for (i = 0; i < MATRIX_SIZE; i++) {
        free(matrixA[i]);
        free(matrixB[i]);
        free(matrixResultSeq[i]);
        free(matrixResultPar[i]);
    }

    free(matrixA);
    free(matrixB);
    free(matrixResultSeq);
    free(matrixResultPar);

    return 0;
}

void matrixMultiplication(const int** pMatrixA, const int** pMatrixB, int** pMatrixOutput, const int size) {
    int i, j, k;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            pMatrixOutput[i][j] = 0;
            for (k = 0; k < size; k++) {
                pMatrixOutput[i][j] += pMatrixA[i][k] * pMatrixB[k][j];
            }
        }
    }
}

void matrixMultiplicationThreads(const int** pMatrixA, const int** pMatrixB, int** pMatrixOutput, const int size, int numberOfThreads) {
    int i, j, k;
#pragma omp parallel for private(i, j, k) num_threads(numberOfThreads) schedule(static)
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            pMatrixOutput[i][j] = 0;
            for (k = 0; k < size; k++) {
                pMatrixOutput[i][j] += pMatrixA[i][k] * pMatrixB[k][j];
            }
        }
    }
}

void printMatrix(const int** pMatrix, const unsigned int size) {
    unsigned int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            printf("%d ", pMatrix[i][j]);
        }
        printf("\n");
    }
}

int isNumber(const char* str) {
    while (*str) {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}