#include <omp.h>
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
    if (argc < 2) {
		printf("Too few arguments!\n");
		return 1;
	}
	int n = atoll(argv[1]);
	double a = LEFT_BORDER;
	double b = RIGHT_BORDER;
    // Grid step
	double h = (b - a) / n;

    // Time measuring
    double start, end; 
    
	int i;
	double sum, xi;
    start = omp_get_wtime(); 
    #pragma omp parallel reduction (+: sum) private(i, xi)
    {
        int rank = omp_get_thread_num();
        int size = omp_get_num_threads();
        for (i = rank; i < n; i += size) {
		    xi = a + i * h;
		    sum += (FUNCTION_PTR)(4 - xi * xi);
	    }
    }
    sum = sum * h;
    end = omp_get_wtime(); 

    //printf("Sum: %lf\n", sum);
    printf("%lf\n", end - start); //Work took seconds
	return 0;
}
