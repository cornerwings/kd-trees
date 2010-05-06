#ifndef IO_H_
#define IO_H_

/*
 * Generates uniform floating numbers between a and b
 */
float rand_float(float a, float b);

/*
 * Generates and writes uniform distribution of size (dxn)
 */
void write_data(char* filename, int d, int n);

/*
 * Generates and returns uniform distribution of size (dxn)
 */
float* generate_data(int d, int n);

/*
 * Reads data from file
 * TODO: Not working
 */
void read_data(char* filename, float *data);

#endif /* IO_H_ */

