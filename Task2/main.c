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
	if (argc < 3) {
		printf("Too few arguments!\n");
		return 1;
	}
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
	int Nk = N / size;
	
	if (rank == 0) {
		Nk = Nk+1;	
	} else if (rank == size - 1) {
		Nk = N - Nk * (size - 1) + 1;
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
	
	// Time measuring
    double start, end;
	if (rank == 0) {
		start = MPI_Wtime();
	}

	int i, j;
	for (i = 0; i * tau < T; i++) {
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
		// Swapping
		double *tmp = u1;
    	u1 = u0;
    	u0 = tmp;
    }

	// Time
	if (rank == 0) {
		end = MPI_Wtime();
		printf("%f", end - start); // Time in seconds
	}

	// Collecting data
	/*if (rank == 0) {
		int total_num = Nk - 1, num;
		double *u = (double *)calloc(N, sizeof(double));
		if (!u) {
			printf("Can't allocate memory!\n");
			return 1;
		}
		for (i = 0; i < Nk - 1; i++) {
			u[i] = u1[i];
		}
		for(i = 1; i < size - 1; i++) {
			MPI_Recv(&num, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			//printf("Recv: %d %d %d\n", i, total_num, num);
			MPI_Recv(u + total_num + 1, num - 2, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			total_num += num - 2;
		}
		//printf("Recv: %d %d %d\n", i, total_num, num);
		MPI_Recv(&num, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(u + total_num + 1, num - 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		// Printing
		if (rank == 0) {
			for (i = 0; i < N; i++) {
				printf("%d %lf\n", i, u[i]);
			}
		}
		// Clear memory
		free(u);
	} else if (rank == size - 1){
		MPI_Send(&Nk, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send(u1 + 1, Nk - 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	} else {
		MPI_Send(&Nk, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send(u1 + 1, Nk - 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}*/
	
	// Clear memory
	free(u0);
	free(u1);
	MPI_Finalize();
	return 0;
}