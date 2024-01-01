#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#define TEST_ENV 0

#define min(x,y)    ((x) < (y) ? (x) : (y))
#define floord(n,d) (((n)<0) ? -((-(n)+(d)-1)/(d)) : (n)/(d))

void matrixMultiplication(const int**, const int**, int**, const int);
void matrixMultiplicationThreads(const int**, const int**, int**, const int, int, int);
void printMatrix(const int**, const unsigned int);

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
    argv[1] = "1500";
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
    //int i, j, k;
    //int ii, jj;
    int ph0, h1, h2, i0, i1, i3;
    int numberOfPackages = (size % packageSize == 0) ? size / packageSize : size / packageSize + 1;
#pragma omp parallel for private(ph0, h1, h2, i0, i1, i3) num_threads(numberOfThreads) schedule(static)
    for (ph0 = 0; ph0 <= floord(size - 1, packageSize); ph0 += 1) {
        for (h1 = 0; h1 <= (size - 1) / packageSize; h1 += 1) {
            for (h2 = 0; h2 <= (size - 1) / packageSize; h2 += 1) {
                for (i0 = packageSize * h1; i0 <= min(size - 1, packageSize * h1 + packageSize - 1); i0 += 1) {
                    for (i1 = packageSize * ph0; i1 <= min(size - 1, packageSize * ph0 + packageSize - 1); i1 += 1) {
                        {
                            if (h2 == 0) {
                                pMatrixOutput[i0][i1] = 0;
                            }
                            for (i3 = packageSize * h2; i3 <= min(size - 1, packageSize * h2 + packageSize - 1); i3 += 1) {
                                pMatrixOutput[i0][i1] += (pMatrixA[i0][i3] * pMatrixB[i3][i1]);
                            }
                        }
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