#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include "proj.h"
#include "myRNG.h"
#include "parSort.cpp")
#include <math.h>

int main(int argc, char *argv[]) {
	int i;
	double timeS1, timeS2, timeP1, timeP2, wtick;
	wtick = omp_get_wtick();
	int n = pow(2, atoi(argv[1]));
	//printf("n=%d; size=%f MB\n", n, (float)((n*sizeof(float))/1000000.0f));
	myRNG rng;

	/** SEQUENTIAL SORT *************************************************/
	// initialize random array
	float *a = (float *)malloc(sizeof(float)*n);
	int *index = (int *)malloc(sizeof(int)*n);
	int *rank;
	rng.resetSeed(10215);
	for(i=0; i<n; i++) {
		*(a + i) = rng.next();
		*(index + i) = i;
	}
	// if option is selected, print starting array
	if(atoi(argv[2]) == 1) {
		printf("[%f, ", *(a + 0));
		for(i=1; i<(n-1); i++) {
			printf("%f, ", *(a + i));
		}
		printf("%f]\n", *(a + n - 1));
	}

	// sort array (sequentially)
	timeS1 = omp_get_wtime();
	seqShellSort(a, index, n);
	timeS2 = omp_get_wtime();
	// check array
	i=0;
	while(*(a + i) <= *(a + i + 1) && i<n) {
		i++;
	}
	if(i==(n-1)) {
		//printf("Seq :: Array sorted SUCCESSFULLY in %f seconds.\n", (timeS2-timeS1));
	} else {
		printf("Seq :: Array is NOT sorted.  See index %d.\n", i);
	}
	// if option is selected, print ending array
	if(atoi(argv[3]) == 1) {
		printf("[%f, ", *(a + 0));
		for(i=1; i<(n-1); i++) {
			printf("%f, ", *(a + i));
		}
		printf("%f]\n", *(a + n - 1));
	}


	/** SAMPLE SORT ******************************************************/
	// reinitialize random array
	free(a);
	free(index);
	a = (float *)malloc(sizeof(float)*n);
	index = (int *)malloc(sizeof(int)*n);
	rng.resetSeed(10215);
	for(i=0; i<n; i++) {
		*(a + i) = rng.next();
		*(index + i) = i;
	}
	// if option is selected, print starting array
	if(atoi(argv[2]) == 1) {
		printf("[%f, ", *(a + 0));
		for(i=1; i<(n-1); i++) {
			printf("%f, ", *(a + i));
		}
		printf("%f]\n", *(a + n - 1));

		printf("[%f, ", (float)*(index + 0));
		for(i=1; i<(n-1); i++) {
			printf("%f, ", (float)*(index + i));
		}
		printf("%f]\n", (float)*(index + n - 1));
	}
	
	// sort array (in parallel using Merge)
	timeP1 = omp_get_wtime();
	rank = sampleSort(a, index, n);
	timeP2 = omp_get_wtime();
	// check array
	i=0;
	while(*(a + i) <= *(a + i + 1) && i<n) {
		i++;
	}
	if(i==(n-1)) {
		//printf("Par (Sample) :: Array sorted SUCCESSFULLY in %f seconds.\n", (timeP2-timeP1));
	} else {
		printf("Par (Sample) :: Array is NOT sorted.  See index %d.\n", i);
	}

	// if option is selected, print ending array
	if(atoi(argv[3]) == 1) {
		printf("[%f, ", *(a + 0));
		for(i=1; i<(n-1); i++) {
			printf("%f, ", *(a + i));
		}
		printf("%f]\n", *(a + n - 1));

		printf("[%f, ", (float)*(index + 0));
		for(i=1; i<(n-1); i++) {
			printf("%f, ", (float)*(index + i));
		}
		printf("%f]\n", (float)*(index + n - 1));

		printf("[%d, ", *(rank + 0));
		for(i=1; i<(n-1); i++) {
			printf("%d, ", *(rank + i));
		}
		printf("%d]\n", *(rank + n - 1));
	}
	
	printf("%d, %d, %f, %f, %f\n", omp_get_num_threads(), n, (float)((n*sizeof(float))/1000000.0f), (timeS2-timeS1), (timeP2-timeP1));
	return 0;
}