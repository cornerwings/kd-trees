#include <stdio.h>
#include <stdlib.h>

/**********************************************************
	
	Uniform Distribution
 	INPUT: Pointer to hold the distribution data
		   Size of Distribution (m,n)
	OUTPUT: Matrix filled with uniform distribution

	Algorithm: Nothing Special

/**********************************************************/
void randu(float *data, int m, int n){
	int i,j;
	for(i=0; i<m; i++){
		for(j=0; j<n; j++){
			data[i*n+j] = (float) rand() / RAND_MAX;
		}
	}
}

/**********************************************************
	
	Normal Distribution
 	INPUT: Pointer to hold the distribution data
		   Size of Distribution (m,n)
	OUTPUT: Matrix filled with normal distribution

	Algorithm: Generate two random numbers R1 and R2 from
			an uniform distribution, then two random normally
			distibuted values can be as follows

			sqrt(2*pi*R1) cos(2*pi*R2)
			sqrt(2*pi*R1) sin(2*pi*R2)

/**********************************************************/
void randn(float *data, int m, int n){
	float *udata = (float*) malloc( (m*n+1)*sizeof(float));
	randu(udata, m*n+1, 1);

	int i,j,k;

	float pi = 4. * atan(1.);
	float square, amp, angle;

	k = 0;
	for(i=0; i<m; i++){
		for(j=0; j<n; j++){
			if(k%2 == 0){
				square = -2. * log( udata[k] );
				if(square < 0.)
				square = 0.;
				amp = sqrt(square);
				angle = 2. * pi * udata[k+1];
				data[i*n+j] = amp * sin(angle);
			}
			else{
				data[i*n+j] = amp * cos(angle);
			}
			k++;
		}
	}

	free(udata);
}

int main(int argc, char *argv[]){

	return 0;
		
}


