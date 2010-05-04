#include "parSort.h"
#include <math.h>

/**********************************************************
 * sample sort (returns rank) 
/**********************************************************/
int * sampleSort(float * a, int *index, int n) {
	int i, j, k, p, start, end, avg, split, * core, * coreCount, * newIndex;
	float * splitters, * newA;
	int * rank = (int *)malloc(sizeof(int) * n);
#pragma omp parallel shared(a, n, p, avg, splitters, core, coreCount, rank, index) private(i, j, k, start, end, split, newA, newIndex)
	{
	/* determine section of array to sort */
	p = omp_get_num_threads();
	avg = (int)floor((float)n / (float)p);
	start = (int)omp_get_thread_num() * avg;
	if (omp_get_thread_num() == (p-1)) {
		end = n;
		splitters = (float *)malloc(sizeof(float) * p * p);
	}
	else {		
		end = (omp_get_thread_num() + 1) * avg;
	}
#pragma omp barrier
	//printf("tid %d :: start = %d, end = %d\n", omp_get_thread_num(), start, end);
	
	/* conduct sequential sort on "owned" portion */
	seqShellSort((a + start), (index + start), (end - start));

	/* determine personal splitters and save to shared location */
	split = (int)floor((float)(end - start) / (float)p);
	j = (omp_get_thread_num() + 1) * p - 1;
	for(i=(end-1); i>start; i-=split) {
		*(splitters + j) = *(a + i);
		j--;
	}
	
#pragma omp barrier

	if(omp_get_thread_num() == 0) {
		seqShellSort(splitters, p*p);

		/*printf("splitters = [%f,", *(splitters + 0)); 
		for(i=1; i<(p*p - 1); i++) {
			printf("%f, ", *(splitters + i));
		}
		printf("%f]\n", *(splitters + (p*p) - 1));*/
		j = 0;
		for(i=(p-1); i<(p*p); i+=p){
			*(splitters + j) = *(splitters + i);
			j++;
		}

		/*printf("splitters = [%f,", *(splitters + 0)); 
		for(i=1; i<(p-1); i++) {
			printf("%f, ", *(splitters + i));
		}
		printf("%f]\n", *(splitters + p - 1));*/
		
		core = (int *) malloc(sizeof(int)*n);
		coreCount = (int *) malloc(sizeof(int) * p);
		for(i=0; i<p; i++) {
			*(coreCount + i) = 0;
		}
	}

#pragma omp barrier
	
	#pragma omp for
	for(i=0; i<n; i++) {
		j = 0;
		while(*(a + i) > *(splitters + j)) {
			j++;
		}
		*(core + i) = j;
	}
	
	/*if(omp_get_thread_num() == 0) {
		printf("core = [%d, ", *(core + 0)); 
		for(i=1; i<(n-1); i++) {
			printf("%d, ", *(core + i));
		}
		printf("%d]\n", *(core + n - 1));
	}*/

#pragma omp barrier

	newA = (float *)malloc(sizeof(float) * 2 * avg);
	newIndex = (int *)malloc(sizeof(float) * 2 * avg);
	j = 0;
	k = omp_get_thread_num();
	for(i=0; i<n; i++) {
		if(*(core + i) == k) {
			*(newA + j) = *(a + i);
			*(newIndex + j) = *(index + i);
			j++;
		}
	}
	*(coreCount + k) = j;

/*#pragma omp barrier

	if(omp_get_thread_num() == 0) {
		printf("coreCount = [%d, ", *(coreCount + 0)); 
		for(i=1; i<(p-1); i++) {
			printf("%d, ", *(coreCount + i));
		}
		printf("%d]\n", *(coreCount + p - 1));
	}*/
	
#pragma omp barrier

	seqShellSort(newA, newIndex, j);
	start = 0;
	for(i=0; i<k; i++) {
		start += *(coreCount + i);
	}

#pragma omp barrier

	/* each process copies its data items back to the full shared array */
	for(i=0; i<j; i++) {
		*(a + start + i) = *(newA + i);
		*(index + start + i) = *(newIndex + i);
	}

	free(newA);
	free(newIndex);

	/** Determine Rank given INDEX array */
#pragma omp barrier

		#pragma omp for
		for(i = 0; i<n; i++) {
			*(rank + *(index + i)) = i;
		}
	}// END parallel region

	return rank;
}



/**********************************************************
 * sequential Shell Sorts
/**********************************************************/
void seqShellSort(float *a, int *index, int n){
	//printf("tid = %d, n = %d\n", omp_get_thread_num(), n);
	int inc, i, j, k, tempI;
	float tempF;
	inc = n/2;
	while (inc > 0) {
		for(i=inc; i<n; i++) {
			tempF = *(a + i);
			tempI = *(index + i);
			j = i;
			while((j >= inc) && (*(a + j - inc) > tempF)) {
				*(a+j) = *(a + j - inc);
				*(index + j) = *(index + j - inc);
				j = j - inc;
			}
			*(a + j) = tempF;
			*(index + j) = tempI;
		}
		inc = (int)floor((inc / 2.2) + 0.5);
	}
}
void seqShellSort(float *a, int n){
	//printf("tid = %d, n = %d\n", omp_get_thread_num(), n);
	int inc, i, j, k;
	float tempF;
	inc = n/2;
	while (inc > 0) {
		for(i=inc; i<n; i++) {
			tempF = *(a + i);
			j = i;
			while((j >= inc) && (*(a + j - inc) > tempF)) {
				*(a+j) = *(a + j - inc);
				j = j - inc;
			}
			*(a + j) = tempF;
		}
		inc = (int)floor((inc / 2.2) + 0.5);
	}
}
