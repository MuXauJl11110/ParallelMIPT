#include <mpi.h>
#include <stdio.h>
#include <math.h> // for sqrt
#include <stdlib.h> // for atoll

// Configuration
#define LEFT_BORDER 0
#define RIGHT_BORDER 2
#define FUNCTION_PTR &sqrt

int main(int argc, char **argv)
{
	// Initialization
	unsigned long long int n = atoll(argv[1]);
	double a = LEFT_BORDER;
	double b = RIGHT_BORDER;
	// Grid step
	double h = (b - a) / n;
	
	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	double sum = 0;
	double received_sum = 0;
	double start, end;

	if (rank == 0) {
		start = MPI_Wtime();
	}

	unsigned long long i;
	double xi;
	for (i = rank; i < n; i += size) {
		xi = a + i * h;
		sum += (FUNCTION_PTR)(4 - xi * xi);
	}
	sum = sum * h;

	if (rank == 0) {
		for (i = 1; i < size; i++) {
			MPI_Recv(&received_sum, 1, MPI_DOUBLE, MPI_ANY_TAG, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			sum += received_sum;
		}
		sum += h * ((FUNCTION_PTR)(4 - b * b)+(FUNCTION_PTR)(4 - a * a)) / 2;
		end = MPI_Wtime();
		
		//printf("Sum: %f\n", sum);
		printf("%f\n", end - start); // Time in seconds
	} else {
		MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
	
	MPI_Finalize();
	return 0;
}
