#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/**********************************************************
	Counts numer of set bits
/**********************************************************/
int count_bits(unsigned number){
	unsigned int c; // c accumulates the total bits set in v
	while(number){
	  c++;
	  number &= number - 1; // clear the least significant bit set
	}
}


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
	
	Standard Normal Distribution
 	INPUT: Pointer to hold the distribution data
		   Size of Distribution (m,n)
	OUTPUT: Matrix filled with normal distribution

	Algorithm: Generate two random numbers R1 and R2 from
			an uniform distribution, then two random normally
			distibuted values can be as follows

			sqrt(-2*log(R1)) cos(2*pi*R2)
			sqrt(-2*log(R1)) sin(2*pi*R2)

    Formula 30.3 of Statistical Distributions (3rd ed)
	Merran Evans, Nicholas Hastings, and Brian Peacock

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

/**********************************************************
	
	Normal Distribution
 	INPUT: Pointer to hold the distribution data
		   Size of Distribution (m,n)
		   mean mu, variance var
	OUTPUT: Matrix filled with normal distribution

	Algorithm: Uses Standard normal distribution to generate
			   an arbitrary normal distribution with mean mu
			   and variance var

/**********************************************************/
void randn_mv(float *data, int m, int n, float mu, float var){

	randn(data, m, n);

	float sd = sqrt(var);

	int i,j;
	for(i=0; i<m; i++){
		for(j=0; j<n; j++){
			data[i*n+j] = mu + sd * data[i*n+j];
		}
	}

}

/**********************************************************
	
	Generate P matrix
 	INPUT: Pointer to hold the data
		   Size of Distribution (k,d)
		   Epislon e, Embedding type p
		   Total points n
	OUTPUT: Resultant P matrix

	Algorithm: P_ij = N(0,1/q) with probability q
					= 0		   with probability 1-q

				q = min( ( e^p-2 * (log n)^p /d ), 1 )
				p belongs to {1,2}

/**********************************************************/
void generatep(float *data, int n, int k, int d, float e, int p){

	float q = exp(p-2) * pow(log(n), p) / d;
	q = q < 1 ? q : 1;	

	float *rdata = (float*) malloc(k*d*sizeof(float));
	randu(rdata, k, d);

	randn_mv(data, k, d, 0, 1/q);

	int i,j;
	for(i=0; i<k; i++){
		for(j=0; j<d; j++){
			data[i*d+j] *= (rdata[i*d+j]<q);
		}
	}

}


/**********************************************************
	
	Generate H matrix
 	INPUT: Pointer to hold the data
		   Size of Distribution (d,d)
	OUTPUT: Resultant H matrix

	Algorithm: P_ij = d^-0.5 8 (-1)^<i-1,j-1>
			   <i,j> is the dot product of i,j in binary

/**********************************************************/
void generateh(float *data, int d){

	float consmul = 1/sqrt(d);

	int i,j;
	for(i=0; i<d; i++){
		for(j=0; j<d; j++){
			int currpow = count_bits( i & j );
			data[i*d+j] = consmul * ( (currpow & (currpow-1)) == 0 ? 1 : -1 );
		}
	}

}

/**********************************************************
	
	Generate D matrix
 	INPUT: Pointer to hold the data
		   Size of Distribution (d,d)
	OUTPUT: Resultant D matrix

	Algorithm: D_ii = {-1,1} with probability 0.5
			   Stored as a vector with length d to avoid
			   extra space

/**********************************************************/
void generated(float *data, int d){

	int i;
	for(i=0; i<d; i++){
		data[i] = rand()/RAND_MAX < 0.5 ? 1: -1;
	}

}

/**********************************************************
	
	Generate FJLT Transform matrix
 	INPUT: Pointer to hold the data
		   Size of Distribution (k,d)
	OUTPUT: Resultant Transform matrix

	Algorithm: FJLT = PHD

/**********************************************************/
void FJLT(float *data, int n, int k, int d){

	float *P = (float*) malloc( k * d * sizeof(float) );
	float *H = (float*) malloc( d * d * sizeof(float) );
	float *D = (float*) malloc( d * sizeof(float) );

	// k = e^-2 * log(n)
	float e = sqrt(log(n)/k);

	generatep(P, n, k, d, e, 2);
	generateh(H, d);
	generated(D, d);

	//FJLT = PHD
	float *temP = (float*) malloc( k * d * sizeof(float) );
	memset(temP, 0 , k * d * sizeof(float) ); 

	//Need to multiply the matrices here
	int a,b,c;	
	for(a=0; a<k; a++){
		for(b=0; b<d; b++){
			register float temp = 0.;
			for(c=0; c<d; c++){
				temp += P[a*d+c]*H[c*d+b];
			}
			temP[a*d+b] = temp;
		}
	}

	//Final matrix multiplication
	for(a=0; a<k; a++){
		for(b=0; b<d; b++){
			register float temp = 0.;
			for(c=0; c<d; c++){
				temp += temP[a*d+c]*(c==b?H[c]:0);
			}
			data[a*d+b] = temp;
		}
	}

}


int main(int argc, char *argv[]){

	return 0;
		
}


