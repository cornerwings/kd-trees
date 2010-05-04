#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

    if(argc < 2){
        printf("Need atleast one argument: filename\n");
        exit(-1);
    }

    printf("Opening file: %s\n", argv[1]);

    FILE *fp;
    fp = fopen(argv[1], "r");

    int i,j;

    int n=0, d=0;
    fscanf(fp, "%d %d", &n, &d);

    //Allocate space for the matrix - 2D style
    float **data;
    data = (float**) malloc(n*sizeof(float*));
    for(i=0; i<n; i++)
        data[i] = (float*) malloc(d*sizeof(float));

    for(i=0; i<n; i++){
        for(j=0; j<d; j++){
            fscanf(fp, "%f", &data[i][j]);
        }
    }

    for(i=0; i<n; i++){
        for(j=0; j<d; j++){
            printf("%f ", data[i][j]);
        }
        printf("\n");
    }

    fclose(fp);

    return 0;

}
