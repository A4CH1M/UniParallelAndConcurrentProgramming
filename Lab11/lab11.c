#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>


double integrate(double(*f)(double), double lowerBoundary, double upperBoundary, double eps);

double integrateThreads(double(*f)(double), double lowerBoundary, double upperBoundary, double eps, const int);

double f(double x) {
	return 0.5 * (3.0 + sin(x) * cos(3.0*x));
}

int main() {

	int lowerBound = -15;
	int upperBound = 16;
	unsigned int nThreads = 4;

	double start, stop, fBaseSeq, fBasePar;

	start = omp_get_wtime();
	fBaseSeq = integrate(f, lowerBound, upperBound, 0.000001);
	stop = omp_get_wtime();
	printf("Integrate of f(x) [%d, %d] = %f [time: %f]\n", lowerBound, upperBound, fBaseSeq, stop - start);

	start = omp_get_wtime();
	fBasePar = integrateThreads(f, lowerBound, upperBound, 0.000001, 4);
	stop = omp_get_wtime();
	printf("Integrate of f(x) [%d, %d] = %f [time: %f]\n", lowerBound, upperBound, fBasePar, stop - start);
}

double integrate(double(*f)(double), double lowerBoundary, double upperBoundary, double eps) {

	if (lowerBoundary > upperBoundary) {
		double tmp = lowerBoundary;
		lowerBoundary = upperBoundary;
		upperBoundary = tmp;
	}

	int i, div = 2;
	double sum = 0.0;
	double sumOld, a, b;
	a = lowerBoundary;
	b = upperBoundary;

	do {

		sumOld = sum;
		sum = 0.0;
		double h = (b - a) / div;

		for (int i = 0; i < div; i++) {
			sum += 0.5 * (f(i * h) + f(i * (h + 1)));
		}
		sum *= h;

		div *= 2;

	} while (fabs(sum - sumOld) > eps);

	return sum;
}

double integrateThreads(double(*f)(double), double lowerBoundary, double upperBoundary, double eps, const int nThreads) {

	if (lowerBoundary > upperBoundary) {
		double tmp = lowerBoundary;
		lowerBoundary = upperBoundary;
		upperBoundary = tmp;
	}

	int i, div = 2;
	double sum = 0.0;
	double sumOld, a, b;
	a = lowerBoundary;
	b = upperBoundary;

	do {

		sumOld = sum;
		sum = 0.0;
		double h = (b - a) / div;

#pragma omp parallel for private(i) reduction(+:sum) num_threads(nThreads)
		for (i = 0; i < div; i++) {
			sum += 0.5 * (f(i * h) + f(i * (h + 1)));
		}
		sum *= h;

		div *= 2;

	} while (fabs(sum - sumOld) > eps);

	return sum;
}

//double integrate(double(*f)(double), double lowerBoundary, double upperBoundary, double eps) {
//
//	if (lowerBoundary > upperBoundary) {
//		double tmp = lowerBoundary;
//		lowerBoundary = upperBoundary;
//		upperBoundary = tmp;
//	}
//
//	int i, div = 2;
//	double sum = 0.0;
//	double sumOld, a, b;
//	a = lowerBoundary;
//	b = upperBoundary;
//
//	do {
//
//		sumOld = sum;
//		sum = 0.0;
//		double h = (b - a) / div;
//
//		for (int i = 0; i < div; i++) {
//			sum += f(i * h);
//		}
//		sum *= h;
//
//		div *= 2;
//
//	} while (fabs(sum - sumOld) > eps);
//	return sum;
//}