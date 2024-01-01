#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int sumKluskisEaten(const int*, const int);
int sumKluskisEaten(const int*, const int);
double successRate(const int*, const int*, const int);

int main() {
	//heh
	int numberOfKluskis = 100000;

	char philosophers[5] = { 'A', 'B', 'C', 'D', 'E' };
	int philosopherNumbers[5] = { 0, 1, 2, 3, 4 };

	int forks[5] = { 1, 1, 1, 1, 1 };
	int attempts[5] = { 0, 0, 0, 0, 0 };
	int eaten[5] = { 0, 0, 0, 0, 0 };
	int id, next;

#pragma omp parallel private(id, next) shared(numberOfKluskis) num_threads(5)
	{
		id = omp_get_thread_num();
		next = id + ((id != 4) ? 1 : -4);
#pragma omp sections
		{
#pragma omp section
			{
				printf("id: %d; next: %d\n", id, next);
				while (numberOfKluskis > 0) {
					attempts[id]++;
					if (forks[id] && forks[next]) {
						forks[id] = 0;
						forks[next] = 0;
#pragma omp critical
						{
							eaten[id]++;
							numberOfKluskis--;
						}
						forks[id] = 1;
						forks[next] = 1;
					}
				}
			}
#pragma omp section
			{
				printf("id: %d; next: %d\n", id, next);
				while (numberOfKluskis > 0) {
					attempts[id]++;
					if (forks[id] && forks[next]) {
						forks[id] = 0;
						forks[next] = 0;
#pragma omp critical
						{
							eaten[id]++;
							numberOfKluskis--;
						}
						forks[id] = 1;
						forks[next] = 1;
					}
				}
			}
#pragma omp section
			{
				printf("id: %d; next: %d\n", id, next);
				while (numberOfKluskis > 0) {
					attempts[id]++;
					if (forks[id] && forks[next]) {
						forks[id] = 0;
						forks[next] = 0;
#pragma omp critical
						{
							eaten[id]++;
							numberOfKluskis--;
						}
						forks[id] = 1;
						forks[next] = 1;
					}
				}
			}
#pragma omp section
			{
				printf("id: %d; next: %d\n", id, next);
				while (numberOfKluskis > 0) {
					attempts[id]++;
					if (forks[id] && forks[next]) {
						forks[id] = 0;
						forks[next] = 0;
#pragma omp critical
						{
							eaten[id]++;
							numberOfKluskis--;
						}
						forks[id] = 1;
						forks[next] = 1;
					}
				}
			}
#pragma omp section
			{
				printf("id: %d; next: %d\n", id, next);
				while (numberOfKluskis > 0) {
					attempts[id]++;
					if (forks[id] && forks[next]) {
						forks[id] = 0;
						forks[next] = 0;
#pragma omp critical
						{
							eaten[id]++;
							numberOfKluskis--;
						}
						forks[id] = 1;
						forks[next] = 1;
					}
				}
			}
		}
	}

	printf("Number of kluskis eaten: %d\n", sumKluskisEaten(eaten, 5));
	printf("Number of kluskis left: %d\n", numberOfKluskis);

	for (size_t i = 0; i < 5; i++) {
		printf("Philosopher %c:\n\tAttempts:\t%d\n\tAte:\t\t%d\n", philosophers[i], attempts[i], eaten[i]);
	}

	printf("Success rate: %f\n", successRate(eaten, attempts, 5));

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
