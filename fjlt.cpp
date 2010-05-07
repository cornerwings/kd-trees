#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cblas.h>
#include <fftw3.h>
#include <omp.h>
#include "io.h"

/*
 * Counts number of set bits in an integer
 */
int count_bits(unsigned number) {
	unsigned int c; // c accumulates the total bits set in v
	while (number) {
		c++;
		number &= number - 1; // clear the least significant bit set
	}
}


/*
 *  Uniform Distribution
 *
 *  INPUT: Pointer to hold the distribution data
 *  	   Size of Distribution (m,n)
 *  OUTPUT: Matrix filled with uniform distribution
 *
 *  Algorithm: Nothing Special
 */
void randu(float *data, int m, int n) {

    #pragma omp parallel for shared(data) 
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			data[i * n + j] = (float) rand() / RAND_MAX;
		}
	}

}


/*
 *  Standard Normal Distribution
 *
 *  INPUT: Pointer to hold the distribution date
 *  	   Size of Distribution (m,n)
 *  OUTPUT: Matrix filled with normal distribution
 *
 *  Algorithm: Generate two random numbers R1 and R2 from
 *  an uniform distribution, then two random normally
 *  distributed values can be as follows
 *
 *  sqrt(-2*log(R1)) cos(2*pi*R2)
 *  sqrt(-2*log(R1)) sin(2*pi*R2)
 *
 *  Formula 30.3 of Statistical Distributions (3rd ed)
 *  Merran Evans, Nicholas Hastings, and Brian Peacock
 */
void randn(float *data, int m, int n) {

	float *udata = (float*) malloc((m * n + 1) * sizeof(float));
	randu(udata, m * n + 1, 1);

	int i, j, k;

	float pi = 4. * atan(1.);
	float square, amp, angle;

	k = 0;
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			if (k % 2 == 0) {
				square = -2. * log(udata[k]);
				if (square < 0.)
					square = 0.;
				amp = sqrt(square);
				angle = 2. * pi * udata[k + 1];
				data[i * n + j] = amp * sin(angle);
			} else {
				data[i * n + j] = amp * cos(angle);
			}
			k++;
		}
	}

	free(udata);

}


/*
 *  Normal Distribution
 *
 *  INPUT: Pointer to hold the distribution data
 *  	   Size of Distribution (m,n)
 *  	   mean mu, variance var
 *  OUTPUT: Matrix filled with normal distribution
 *
 *  Algorithm: Uses Standard normal distribution to
 *  generate an arbitrary normal distribution with
 *  mean mu and variance var
 */
void randn_mv(float *data, int m, int n, float mu, float var) {

	randn(data, m, n);

	float sd = sqrt(var);

    #pragma omp parallel for shared(data)
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			data[i * n + j] = mu + sd * data[i * n + j];
		}
	}

}


/*
 * Inverse CND for Normal distribution
 *
 * INPUT: Any value from uniform distribution
 * OUPUT: Value from normal distribution
 *
 * Algorithm: q belongs to (0,1)
 * 			  INVCND(q) belongs to normal distribution
 *
 * We use Moro's Inverse CND approximation as there exists no
 * closed form solutions
 */
float moroinv_cnd(float P){
    const float a1 = 2.50662823884;
    const float a2 = -18.61500062529;
    const float a3 = 41.39119773534;
    const float a4 = -25.44106049637;
    const float b1 = -8.4735109309;
    const float b2 = 23.08336743743;
    const float b3 = -21.06224101826;
    const float b4 = 3.13082909833;
    const float c1 = 0.337475482272615;
    const float c2 = 0.976169019091719;
    const float c3 = 0.160797971491821;
    const float c4 = 2.76438810333863E-02;
    const float c5 = 3.8405729373609E-03;
    const float c6 = 3.951896511919E-04;
    const float c7 = 3.21767881768E-05;
    const float c8 = 2.888167364E-07;
    const float c9 = 3.960315187E-07;
    float y, z;

    if(P <= 0 || P >= 1.0){
        printf("MoroInvCND(): bad parameter %f\n", P);
    }

    y = P - 0.5;
    if(fabs(y) < 0.42){
        z = y * y;
        z = y * (((a4 * z + a3) * z + a2) * z + a1) / ((((b4 * z + b3) * z + b2) * z + b1) * z + 1);
    }else{
        if(y > 0)
            z = log(-log(1.0 - P));
        else
            z = log(-log(P));

        z = c1 + z * (c2 + z * (c3 + z * (c4 + z * (c5 + z * (c6 + z * (c7 + z * (c8 + z * c9)))))));
        if(y < 0) z = -z;
    }

    return z;
}

/*
 *  Normal Distribution
 *
 *  INPUT: Pointer to hold the distribution data
 *  	   Size of Distribution (m,n)
 *  	   mean mu, variance var
 *  OUTPUT: Matrix filled with normal distribution
 *
 *  Algorithm: Uses Moro's Inverse CND distribution to
 *  generate an arbitrary normal distribution with
 *  mean mu and variance var
 */
void inv_randn(float *data, int m, int n, float mu, float var){

	float sd = sqrt(var);

    #pragma omp parallel for shared(data)
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			data[i * n + j] = mu + sd * moroinv_cnd((float)rand()/RAND_MAX);
		}
	}
}


/*
 * Generates P matrix
 *
 * INPUT: size of distribution (k,d)
 * 		  epsilon e
 * 		  embedding type p
 * 		  number of points n
 * OUTPUT: P matrix
 *
 * Algorithm: P_ij = N(0,1/q) with probability q
 * 				   = 0 elsewhere
 *
 * q = min( ( e^p-2 * (log n)^p /d ), 1 )
 * p belongs to {1,2}
 */
float* generatep(int n, int k, int d, float e, int p) {

	float *data = (float*) malloc(k * d * sizeof(float));
	memset(data, 0, k * d * sizeof(float));

	float q = exp(p - 2) * pow(log(n), p) / d;
	q = q < 1 ? q : 1;

    //printf("Value of q: %f\n", q);

	float *rdata = (float*) malloc(k * d * sizeof(float));

	//randn_mv(data, k, d, 0, 1 / q);
    inv_randn(data, k, d, 0, 1/q);
	randu(rdata, k, d);

    #pragma omp parallel for shared(data, rdata)
	for (int i = 0; i < k; i++) {
		for (int j = 0; j < d; j++) {
			data[i * d + j] *= (rdata[i * d + j] < q);
		}
	}

	free(rdata);

	return data;

}


/*
 * Generates Hadamard matrix
 *
 * INPUT: size of matrix
 * OUTPUT: Resultanta H matrix
 *
 * Algorithm: P_ij = d^-0.5 8 (-1)^<i-1,j-1>
 *  <i,j> is the dot product of i,j in binary
 *
 *  Not really used because this takes O(d^2) operations
 */
float* generateh(int d) {

    float *data = (float*) malloc(d*d*sizeof(float));

	float consmul = 1 / sqrt(d);

    #pragma omp parallel for shared(data)
	for (int i = 0; i < d; i++) {
		for (int j = 0; j < d; j++) {
			int currpow = count_bits(i & j);
			data[i * d + j] = consmul * ((currpow & (currpow - 1)) == 0 ? 1 : -1);
		}
	}

    return data;

}

/*
 * Generates diagnol matrix D
 *
 * INPUT: size of matrix d
 * OUTPUT: Matrix D
 *
 * Algorithm: D_ii = {-1,1} with probability 0.5
 *
 * Returned as a vector with length d to avoid extra space
 */
float* generated(int d) {

	float *data = (float*) malloc(d * sizeof(float));

    #pragma omp parallel for shared(data)
	for (int i = 0; i < d; i++) {
		data[i] = ((float) rand() / RAND_MAX) < 0.5 ? 1 : -1;
	}

	return data;

}


/*
 * Apply FJLT transform on input data
 * INPUT: n points of d dimensions
 *        k required dimension
 * OUTPUT: Projected data
 *
 * Algorithm: FJLT = PHDx
 */
float* FJLT(float *input, int n, int k, int d) {

	float *data = (float*) malloc(n * d * sizeof(float));
	memcpy(data, input, n * d * sizeof(float));

	float eps = sqrt(log(n) / k);

	float *D;
	D = generated(d);

	float *P;
	P = generatep(n, k, d, eps, 2);

	/*
	 * Reduced data
	 */
	float *result;
	result = (float*) malloc(n * k * sizeof(float));
	memset(result, 0, k * n * sizeof(float));

	float sqrtd = 1 / sqrt(d);

	/*
	 * Process each point at once i.e each column of data
	 */
	int curr = 0;

    #pragma omp parallel for private(curr) shared(data,result)
	for(curr=0; curr < n; curr++) {
		float *point = data + d * curr;
		float *output = result + k * curr;

		int a, b, c;

		for (a = 0; a < d; a++){
            point[a] *= (/*sqrtd * */ D[a]);
        }

		/*
		 * Do Fast Walsh transform on the point
		 */
		int l2 = log2(d);
		for (a = 0; a < l2; a++) {
			for (b = 0; b < (1 << l2); b += 1 << (a + 1)) {
				for (c = 0; c < (1 << a); c++) {
					float temp = point[b + c];
					point[b + c] += point[b + c + (1 << a)];
					point[b + c + (1 << a)] = temp - point[b + c + (1 << a)];
				}
			}
		}

		/*
		 * Multiply with P
		 */
	    cblas_sgemv(CblasRowMajor, CblasNoTrans, k, d, (float)1/d, P, d, point, 1, 0.0, output, 1);

	}

    free(D);
    free(P);
    free(data);

	return result;

}

/*
 * Apply FJLT transform on input data
 * INPUT: n points of d dimensions
 *        k required dimension
 * OUTPUT: Projected data
 *
 * Algorithm: FJLT = PHDx
 * Uses FFTW instead of Walsh-Hadamard Transform
 */
float* FJLT_fftw(float *data, int n, int k, int d) {

	// k = e^-2 * log(n)
	float eps = sqrt(log(n) / k);

	float *D;
	D = generated(d);

	float *P;
	P = generatep(n, k, d, eps, 2);

	/*
	 * Reduced data
	 */
	float *result;
	result = (float*) malloc(n * k * sizeof(float));
	memset(result, 0, k * n * sizeof(float));

	float sqrtd = 1 / sqrt(d);

    /*
     * Temp array of size d to copy from fftw_complex to float
     */
    float *hdx;
    hdx = (float*) malloc(d* sizeof(float));

    /*
     * Temp arrays for fourier transform results
     */
    fftw_complex* in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * d);
    fftw_complex* out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * d);

	/*
	 * Process each point at once i.e each column of data
	 */
	int curr = 0;
    #pragma omp parallel for private(curr) shared(data,result)
	for(curr=0; curr<n; curr++) {

		float *x = data + d * curr;
		float *y = result + k * curr;

	    //setup a plan
	    fftw_plan plan = fftw_plan_dft_1d(d, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	    int i;

	    //First multiply X by D while simultaneosuly pack it in x
	    for (i = 0; i < d; i++) {
		    in[i][0] = /*sqrtd * */ D[i] * x[i];
	    }

	    //then Fourier transform
	    fftw_execute_dft( plan, in, out);

        //cast from fftw_complex to float
        //TODO - Dont know any other way.. going with the normal copy
	    for (i = 0; i < d; i++) {
		    hdx[i] = out[i][0];
	    }

	    cblas_sgemv(CblasRowMajor, CblasNoTrans, k, d, (float)1/d, P, d, hdx, 1, 0.0, y, 1);
    }

    return result;
}




