#include <stdio.h>
#include <limits.h>
#include <omp.h>

#include "rerank.h"

int parallel_find_max (int* ranks, int size) {
	int max = - INT_MAX;
	int i;
	int n_threads = omp_get_num_threads();
	#pragma omp parallel private(i) 
	{
		int local_max = - INT_MAX;
		int tid = omp_get_thread_num();
		int share = (size / n_threads) + 1;
		int begin = share * tid;
		int end = share *(tid + 1);
		if(end > size) end = size;
		for(int i = begin; i < end; i++) {
			if(local_max < ranks[i]) local_max = ranks[i];
		}
		#pragma omp critical 
		{
			if(local_max > max) max = local_max;
		}
	}
	return max;
}

//Scan not parallelized because only one thread will be executing the function
void scan (int* aux_scan, int size) {
	for(int i = 0; i < size - 1; i++) {
		aux_scan[i+1] += aux_scan[i]; 
	}
}

//Find_max not parallelized because only one thread will be executing the function
int find_max(int* ranks, int size) {
	int max = - INT_MAX;
	for(int i = 0; i < size; i++) {
		if(*(ranks + i) > max) max = *(ranks + i);
	}
	return max;
}

void rerank(int* ranks, int size) {
	int max_element =  find_max(ranks, size);
	int aux_size = max_element + 1;
	int* aux_scan = new int[aux_size];
	for(int i=0; i < aux_size; i++ ) {
		aux_scan[i] = 0;
	}
	for(int i=0; i < size; i++ ) {
		aux_scan[ranks[i]] = 1;
	}
	scan(aux_scan, aux_size); //scan yet to be parallelized
	for(int i = 0; i < size; i++) {
		int temp = aux_scan[ranks[i]] - 1;
		ranks[i] = temp;
	}
	delete [] aux_scan;
}
