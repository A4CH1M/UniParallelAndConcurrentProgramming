#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

//#define MATRIX_SIZE 1400
#define TEST_ENV 0

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

void matrixMultiplicationThreads(const int** pMatrixA, const int** pMatrixB, int** pMatrixOutput, const int size, int numberOfThreads, int packageSize) {
    int i, j, k;
    int ii, jj;
    int numberOfPackages = (size % packageSize == 0) ? size / packageSize : size / packageSize + 1;
#pragma omp parallel for private(i, j, k, ii, jj) num_threads(numberOfThreads) schedule(static)
    for (ii = 0; ii < numberOfPackages; ii++) {
        for (jj = 0; jj < numberOfPackages; jj++) {
            for (i = ii * packageSize; i < (ii + 1) * packageSize && i < size; i++) {
                for (j = jj * packageSize; j < (jj + 1) * packageSize && j < size; j++) {
                    pMatrixOutput[i][j] = 0;
                    for (k = 0; k < size; k++) {
                        pMatrixOutput[i][j] += pMatrixA[i][k] * pMatrixB[k][j];
                    }
                }
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

int isNumber(const char* ciag) {
    while (*ciag) {
        if (!isdigit(*ciag)) {
            return 0;
        }
        ciag++;
    }
    return 1;
}

int main(int argc, char** argv) {

#if TEST_ENV
    argc = 3;
    argv[1] = "1000";
    argv[2] = "8";
#endif

    if (argc != 3) {
        fprintf(stderr, "ERR: INCORRECT NUMBER OF ARGUMENTS\n");
        return 1;
    }

    if (!isNumber(argv[1])) {
        fprintf(stderr, "ERR: ARGUMENT 'MATRIX_SIZE' MUST BE A NUMBER\n");
        return 2;
    }

    if (!isNumber(argv[1])) {
        fprintf(stderr, "ERR: ARGUMENT 'PACKAGE_SIZE' MUST BE A NUMBER\n");
        return 2;
    }

    char* pEnd;
    const long MATRIX_SIZE = strtol(argv[1], &pEnd, 10);

    if (*pEnd != '\0' || errno == ERANGE) {
        fprintf(stderr, "ERR: INVALID ARGUMENT OR OUT OF RANGE\n");
        return 3;
    }

    const long PACKAGE_SIZE = strtol(argv[2], &pEnd, 10);

    if (*pEnd != '\0' || errno == ERANGE) {
        fprintf(stderr, "ERR: INVALID ARGUMENT OR OUT OF RANGE\n");
        return 3;
    }

    int i, j;

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
            //matrixResultSeq[i][j] = 0;
            //matrixResultPar[i][j] = 0;
        }
    }
    //int takCzyNieQuestionMark = 0;
    double start, stop, tSeq, tPar, speedup;

    start = omp_get_wtime();
    matrixMultiplication(matrixA, matrixB, matrixResultSeq, MATRIX_SIZE);
    stop = omp_get_wtime();
    tSeq = stop - start;

    start = omp_get_wtime();
    matrixMultiplicationThreads(matrixA, matrixB, matrixResultPar, MATRIX_SIZE, 4, PACKAGE_SIZE);
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