#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float rand_float(float a, float b){
    float t = (float)rand() / (float)RAND_MAX;
    return (1.0f - t) * a + t * b;
}

void write_data(char* filename, int d, int n){
    FILE *fp;
    fp = fopen(filename,"w");

    fprintf(fp, "%d %d\n", d, n);

    int i,j;
    for(i=0; i<d; i++){
        for(j=0; j<n; j++){
            float feat = rand_float(-2,2);
            fprintf(fp, "%f ", feat);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

float* generate_data(int d, int n){

	float *data = (float*) malloc(n*d*sizeof(float));
	memset(data, 0, n*d*sizeof(float));

    int i,j;
    for(i=0; i<d; i++){
        for(j=0; j<n; j++){
            data[i+d*j] = rand_float(-2,2);
        }
    }

	return data;
}

void read_data(char* filename, float *data){
    FILE *fp;
    fp = fopen(filename, "r");

    int i,j;

    int n=0, d=0;
    fscanf(fp, "%d %d", &d, &n);

    data = (float*) malloc(n*d*sizeof(float*));
	memset(data,0,n*d*sizeof(float));

    for(i=0; i<n*d; i++){
        //for(j=0; j<n; j++){
            fscanf(fp, "%f", &data[i]);
        //}
    }
}

