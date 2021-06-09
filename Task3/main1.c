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
    #pragma omp parallel reduction (+: sum) private(xi)
    {
        #pragma omp for
        for (i = 0; i < n; i++) {
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
