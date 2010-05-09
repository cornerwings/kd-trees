#ifndef FJLT_H_
#define FJLT_H_

/*
 * Counts number of set bits in an integer
 */
int count_bits(unsigned number);

/*
 *  Uniform Distribution
 */
void randu(float *data, int m, int n) ;


/*
 *  Standard Normal Distribution
 */
void randn(float *data, int m, int n) ;

/*
 *  Normal Distributionr
 */
void randn_mv(float *data, int m, int n, float mu, float var);

/*
 * Inverse CND for Normal distribution
 */
float moroinv_cnd(float P);

/*
 *  Normal Distribution uses Moro's
 */
void inv_randn(float *data, int m, int n, float mu, float var);

/*
 * Generates P matrix
 */
float* generatep(int n, int k, int d, float e, int p) ;

/*
 * Generates Hadamard matrix
 */
float* generateh(int d);

/*
 * Generates diagnol matrix D
 */
float* generated(int d) ;

/*
 * Apply FJLT transform on input data
 */
float* FJLT(float *input, int n, int k, int d);

/*
 * Apply FJLT transform on input data
 * Uses FFTW instead of Walsh-Hadamard Transform
 */
float* FJLT_fftw(float *data, int n, int k, int d);

#endif /* FJLT_H_ */

