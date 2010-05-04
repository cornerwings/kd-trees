#include <stdio.h>
#include <stdlib.h>

float rand_float(float a, float b){
    float t = (float)rand() / (float)RAND_MAX;
    return (1.0f - t) * a + t * b;
}

int main(int argc, char *argv[]){

    if(argc < 4){
        printf("Need atleast three parameters: n, d and filename\n");
        exit(-1);
    }

    int n = atoi(argv[1]);
    int d = atoi(argv[2]);

    FILE *fp;
    fp = fopen(argv[3],"w");

    fprintf(fp, "%d %d\n", n, d);

    int i,j;
    for(i=0; i<n; i++){
        for(j=0; j<d; j++){
            float feat = rand_float(-2,2);
            fprintf(fp, "%f ", feat);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);

    return 0;
}


    
