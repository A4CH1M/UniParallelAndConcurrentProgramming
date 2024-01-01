#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int sumKluskisEaten(const int*, const int);
int sumAttempts(const int*, const int);
double successRate(const int*, const int*, const int);
void pholosopher(omp_lock_t, omp_lock_t, int*, int*, int*, const int);	//mistake in name on purpose just fyi

int main() {

	int numberOfKluskis = 100000;

	char philosophers[5] = { 'A', 'B', 'C', 'D', 'E' };
	int philosopherNumbers[5] = { 0, 1, 2, 3, 4 };

	omp_lock_t forkA, forkB, forkC, forkD, forkE;
	omp_init_lock(&forkA);
	omp_init_lock(&forkB);
	omp_init_lock(&forkC);
	omp_init_lock(&forkD);
	omp_init_lock(&forkE);

	int attempts[5] = { 0, 0, 0, 0, 0 };
	int eaten[5] = { 0, 0, 0, 0, 0 };
	int id, next;

#pragma omp parallel private(id, next) shared(numberOfKluskis) num_threads(5)
	{
		//id = omp_get_thread_num();
		//next = id + ((id != 4) ? 1 : -4);
#pragma omp sections
		{
#pragma omp section
			{
				//printf("id: %d; next: %d\n", id, next);
				pholosopher(forkA, forkB, &numberOfKluskis, attempts, eaten, 0);
			}
#pragma omp section
			{
				//printf("id: %d; next: %d\n", id, next);
				pholosopher(forkB, forkC, &numberOfKluskis, attempts, eaten, 1);
			}
#pragma omp section
			{
				//printf("id: %d; next: %d\n", id, next);
				pholosopher(forkC, forkD, &numberOfKluskis, attempts, eaten, 2);
			}
#pragma omp section
			{
				//printf("id: %d; next: %d\n", id, next);
				pholosopher(forkD, forkE, &numberOfKluskis, attempts, eaten, 3);
			}
#pragma omp section
			{
				//printf("id: %d; next: %d\n", id, next);
				pholosopher(forkE, forkA, &numberOfKluskis, attempts, eaten, 4);
			}
		}
	}

	printf("Number of attempts: %d\n", sumAttempts(attempts, 5));
	printf("Number of kluskis eaten: %d\n", sumKluskisEaten(eaten, 5));
	//printf("Number of kluskis left: %d\n", numberOfKluskis);

	for (size_t i = 0; i < 5; i++) {
		printf("Philosopher %c:\n\tAttempts:\t%d\n\tAte:\t\t%d\n", philosophers[i], attempts[i], eaten[i]);
	}

	printf("Success rate: %f\n", successRate(eaten, attempts, 5));

	omp_destroy_lock(&forkA);
	omp_destroy_lock(&forkB);
	omp_destroy_lock(&forkC);
	omp_destroy_lock(&forkD);
	omp_destroy_lock(&forkE);

	return 0;
}

int sumKluskisEaten(const int* pEaten, const int n) {
	int sumEaten = 0;
	for (int i = 0; i < n; i++) {
		sumEaten += pEaten[i];
	}
	return sumEaten;
}

int sumAttempts(const int* pAttempts, const int n) {
	int sumAttempts = 0;
	for (int i = 0; i < n; i++) {
		sumAttempts += pAttempts[i];
	}
	return sumAttempts;
}

double successRate(const int* pEaten, const int* pAttempts, const int n) {

	return (double)sumKluskisEaten(pEaten, n) / sumAttempts(pAttempts, n);
}

void pholosopher(omp_lock_t lLock, omp_lock_t rLock, int* kluskis, int* pAttempts, int* pEaten, const int philosopherId) {

	while (*kluskis > 0) {
		pAttempts[philosopherId]++;

		int testLock1 = omp_test_lock(&lLock);
		int testLock2 = omp_test_lock(&rLock);

		if (testLock1 && !testLock2) {
			omp_unset_lock(&lLock);
		}

		if (!testLock1 && testLock2) {
			omp_unset_lock(&rLock);
		}

		if (testLock1 && testLock2) {

			pEaten[philosopherId]++;
			(*kluskis)--;

			omp_unset_lock(&lLock);
			omp_unset_lock(&rLock);
		}
	}
}