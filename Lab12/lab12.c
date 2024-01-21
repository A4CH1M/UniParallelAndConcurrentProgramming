#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

//#define min(x,y)    ((x) < (y) ? (x) : (y))
//#define floord(n,d) (((n)<0) ? -((-(n)+(d)-1)/(d)) : (n)/(d))

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

    double start, stop, startPar, stopPar, tPar, speedup;
    start = omp_get_wtime();

#if TEST_ENV
    argc = 4;
    argv[1] = "600";
    argv[2] = "8";
    argv[3] = "0";
    argv[4] = "4";
#endif
    if (argc != 5) {
        fprintf(stderr, "ERR: INCORRECT NUMBER OF ARGUMENTS\n");
        return 1;
    }

    if (!isNumber(argv[1])) {
        fprintf(stderr, "ERR: ARGUMENT 'MATRIX_SIZE' MUST BE A NUMBER\n");
        return 2;
    }

    if (!isNumber(argv[2])) {
        fprintf(stderr, "ERR: ARGUMENT 'PACKAGE_SIZE' MUST BE A NUMBER\n");
        return 3;
    }

    if (!isNumber(argv[3])) {
        fprintf(stderr, "ERR: ARGUMENT 'IS_PARALLEL' MUST BE A NUMBER\n");
        return 4;
    }

    if (!isNumber(argv[4])) {
        fprintf(stderr, "ERR: ARGUMENT 'NUM_THREADS' MUST BE A NUMBER\n");
        return 5;
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

    const int IS_PARALLEL = strtol(argv[3], &pEnd, 10);
    const int NUM_THREADS = strtol(argv[4], &pEnd, 10);

    int** matrixA = (int**)malloc(MATRIX_SIZE * sizeof(int*));
    int** matrixB = (int**)malloc(MATRIX_SIZE * sizeof(int*));
    int** matrixResultSeq = (int**)malloc(MATRIX_SIZE * sizeof(int*));
    int** matrixResultPar = (int**)malloc(MATRIX_SIZE * sizeof(int*));

    if (matrixA == NULL || matrixB == NULL || matrixResultSeq == NULL || matrixResultPar == NULL) {
        fprintf(stderr, "ERR: UNABLE TO ALLOCATE MEMORY\n");
        return 4;
    }

    int i, j;

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

    startPar = omp_get_wtime();
    if (IS_PARALLEL) {
        matrixMultiplicationThreads(matrixA, matrixB, matrixResultPar, MATRIX_SIZE, NUM_THREADS, PACKAGE_SIZE);
    }
    else {
        matrixMultiplication(matrixA, matrixB, matrixResultSeq, MATRIX_SIZE);
    }
    stopPar = omp_get_wtime();
    tPar = stopPar - startPar;


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

    stop = omp_get_wtime();

    double t = stop - start;
    double f = (t - tPar) / t;
    double maxSpeedup = 1 / f;
#if TEST_ENV
    printf("For N = %d; num_threads=%d:\n", MATRIX_SIZE, NUM_THREADS);
    printf("\tTime:\t\t%.16g\n", t);
    printf("\tParallel time:\t%.16g\n", tPar);
    printf("\tMax speedup:\t%.16g\n", maxSpeedup);
    //for (i = 0; i < MATRIX_SIZE; i++) {
    //    for (j = 0; j < MATRIX_SIZE; j++) {
    //        if (matrixResultSeq[i][j] != matrixResultPar[i][j]) {
    //            //takCzyNieQuestionMark = 1;
    //            fprintf(stderr, "ERR: Matrices are not the same");
    //        }
    //    }
    //}
#else
    //printf("For N = %d; num_threads=%d:\n", MATRIX_SIZE, NUM_THREADS);
    //printf("\tTime:\t\t%.16g\n", t);
    //printf("\tParallel time:\t%.16g\n", tPar);
    //printf("\tMax speedup:\t%.16g\n", maxSpeedup);
    if (IS_PARALLEL) {
        printf("%d;%f;%f", NUM_THREADS, t, tPar);
    }
    else {
        //printf("For N = %d; num_threads=%d:\n", MATRIX_SIZE, NUM_THREADS);
        //printf("\tTime:\t\t%.16g\n", t);
        //printf("\tParallel time:\t%.16g\n", tPar);
        //printf("\tMax speedup:\t%.16g\n", maxSpeedup);
        printf("%f;%f", t, maxSpeedup);
    }

#endif

    return 0;
}