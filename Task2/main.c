#include <mpi.h>
#include <stdio.h>
#include <stdlib.h> // for atoll, strtod

// Configuration
#define A 0.0
#define B 1.0
#define L 1.0
#define C 1.0
#define PHI_0 0
#define INV 0.3

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
    // Grid step
	double h = (b-a) / N;
    double tau = h * h  / c / c * inv;
	
	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Second initialization
	// I hope that memory allocation per each worker is more cache-friendly 
	// than allocating by one and then sending for others, because compiler
	// will allocate memory which can be put into one cacheline of each proccessor 
	int Nk = N / size; // Need to fix, because may be some problems with 10/3 for examples
	
	if (rank == 0) {
		Nk = Nk+1;	
	} else if (rank == size - 1) {
		Nk = N - Nk * size + 1;
	} else {
		Nk = Nk+2;
	}
	double *u0 = (double *)calloc(Nk, sizeof(double));
	if (!u0) {
		printf("Can't allocate memory!\n");
		return 1;
	}
	double *u1 = (double *)calloc(Nk, sizeof(double));
	if (!u1) {
		printf("Can't allocate memory!\n");
		return 1;
	}
	if (rank == 0) {
		u0[0] = u1[0] = a;
	}
	if (rank == size - 1) {
		u0[Nk-1] = u1[Nk-1] = b;
	}
	
	int i, j;
	for (i = 0; i < T; i++) {
		// Exchange
        if (rank % 2 == 0) { // for even ranks
            if (rank > 0) MPI_Send(&u0[1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
            if (rank < size-1) MPI_Send(&u0[Nk - 2], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
        }
        else {
            if (rank < size-1) MPI_Send(&u0[Nk - 2], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
            if (rank > 0) MPI_Send(&u0[1], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);
        }
		MPI_Barrier(MPI_COMM_WORLD);
		if (rank % 2 == 0) { // for odd ranks
            if (rank > 0) MPI_Recv(&u0[0], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (rank < size-1) MPI_Recv(&u0[Nk - 1], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else {
            if (rank < size-1) MPI_Recv(&u0[Nk - 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (rank > 0) MPI_Recv(&u0[0], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
		MPI_Barrier(MPI_COMM_WORLD);
		
		for (j = 1; j < Nk - 1; j++) {
			u1[j] = u0[j] + inv * (u0[j-1] - 2 * u0[j] + u0[j+1]);
		}
		/*
		for (j = 0; j < Nk; j++) {
			u0[j] = u1[j];
		}
		*/
		// Swapping
		double *tmp = u1;
    	u1 = u0;
    	u0 = tmp;
    }

	for (i = 0; i < Nk; i++) {
		printf("%d %lf\n", rank * Nk + i, u0[i]);
	}
	// Clear memory
	free(u0);
	free(u1);

	MPI_Finalize();
	return 0;
}
/*
if (rank % 2 == 0) { // for even ranks
            if (rank > 0) MPI_Send(&u0[1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
            //if (rank > 0) MPI_Recv(&u0[0], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //if (rank < size-1) MPI_Recv(&u0[Nk - 1], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (rank < size-1) MPI_Send(&u0[Nk - 2], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
        }
        else {
            //if (rank < size-1) MPI_Recv(&u0[Nk - 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (rank < size-1) MPI_Send(&u0[Nk - 2], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
            if (rank > 0) MPI_Send(&u0[1], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);
            //if (rank > 0) MPI_Recv(&u0[0], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
*/