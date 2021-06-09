#include <omp.h>
#include <stdio.h>
#include <stdlib.h> // for atoll, strtod

// Configuration
#define A 0.0
#define B 1.0
#define L 1.0
#define C 1.0
#define PHI_0 0
#define INV 0.3
#define CHUNK_SIZE 8

int main(int argc, char **argv)
{
	// First Initialization
	unsigned long long int N = atoll(argv[1]);
    char *eptr;
    double T = strtod(argv[2], &eptr);
	double a = A;
	double b = B;
    double l = L;
	double c = C;
    double phi_0 = PHI_0;
	double inv = INV;
    int chunk_size = CHUNK_SIZE;
    // Grid step
	double h = (b-a) / N;
    double tau = h * h  / c / c * inv;
	
    // Second initialization
    double *u0 = (double *)calloc(N, sizeof(double));
	if (!u0) {
		printf("Can't allocate memory!\n");
		return 1;
	}
	double *u1 = (double *)calloc(N, sizeof(double));
	if (!u1) {
		printf("Can't allocate memory!\n");
		return 1;
	}
    u0[0] = u1[0] = a;
	u0[N-1] = u1[N-1] = b;

    // Time measuring
    double start, end;
	
	int i, j;
    start = omp_get_wtime(); 
    #pragma omp parallel private(i)
    {
        for (i = 0; i < T; i++) {
            #pragma omp for schedule (static)//, chunk_size)
            for (j = 1; j < N - 1; j++) {
                u1[j] = u0[j] + inv * (u0[j-1] - 2 * u0[j] + u0[j+1]);
            }
            // Swapping
            #pragma omp single
            {
                double *tmp = u1;
                u1 = u0;
                u0 = tmp;
            }
        }
    }
    end = omp_get_wtime(); 

    printf("%lf\n", end - start);

    /*
	for (i = 0; i < N; i++) {
		printf("%d %lf\n", i, u0[i]);
	}*/
	// Clear memory
	free(u0);
	free(u1);

	return 0;
}