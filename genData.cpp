#include <stdlib.h>
#include "myRNG.cpp"

float * generateData(int d, int points) {
	myRNG r;
	r.resetSeed(10);
	r.setLimits(-1, 1);
	float * data = (float *)malloc(d * points * sizeof(float));
	/* check that memory was allocated */
	if(data == NULL) {
		float fail = -100.0f;
		return &fail;
	}

	int i, j, pos;
	pos = 0;
	/* generates data in such that first dimension for all
	   points is chosen, the moves up one dimension at a time */
	for(i=0; i<d; i++) {
		for(j=0; j<points; j++) {
			*(data + pos) = 2.0f * r.next() - 1.0f;
			pos++;
		}
	}

	return data;
}
