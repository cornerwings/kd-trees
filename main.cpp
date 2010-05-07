#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cblas.h>
#include <fftw3.h>
#include <sys/time.h>
#include "fjlt.h"
#include "io.h"

/*
 * Macro to find minimum of two numbers
 */
#define MIN(a,b) (a < b ? a : b)

int main(int argc, char *argv[]) {

	//write_data((char*)"sample.dat", 16, 8);

	int n = 10000;
	int d = 256;

	float *data;
	data = generate_data(d, n);

	/*
	 * reducing to this dimension
	 */
	int k = 150;

    struct timeval start, stop;

	float *result;
    gettimeofday(&start, 0);
	result = FJLT(data, n, k, d);
    gettimeofday(&stop, 0);
    //result = FJLT_fftw(data, n, k, d);

    printf("Time taken for FJLT: %f\n", (stop.tv_sec -  start.tv_sec) + 1.0e-6 * (stop.tv_usec  - start.tv_usec));

	/*
	 * verify if it works
	 */

	for (int pt1 = 0; pt1 < MIN(n,5); pt1++) {
		for (int pt2 = pt1; pt2 < MIN(n,5); pt2++) {

            if(pt1 == pt2)
                continue;

			float orig_dist = 0.0;
			for (int i = 0; i < d; i++){
				orig_dist += pow(data[pt1 * d + i] - data[pt2 * d + i], 2);
            }

			float new_dist = 0.0;
			for (int i = 0; i < k; i++){
				new_dist += pow(result[pt1 * k + i] - result[pt2 * k + i], 2);
            }

			printf("[%d %d]: %f %f\n", pt1, pt2, sqrt(orig_dist), sqrt((float)d/k)*sqrt(new_dist));
		}
	}

	return 0;

}
