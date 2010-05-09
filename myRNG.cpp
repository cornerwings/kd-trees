#ifndef _RNG_
#include "myRNG.h"
#define _RNG
#endif


/* this constructor sets all values used in Lehmer's algorithm
 * to the provided values setting up an RNG */
myRNG::myRNG( void ) {
	x = 1.0f;
	t = 0.0f;
	a = 48271;
	m = (pow(2.0f, 31.0f) - 1);
	q = 44488;
	r = 3399;
}

/* using Lehmer's algorithm (ensuring no overflow)
 * gets a float uniformly distributed between 0 and 1 */
float myRNG::next(){
	t = a * fmod(x, q) - r * floor(x / q);
	if (t < 0) {
		t = t + m;
	}
	x = t;
	return (x / m);
}

/* returns a time spent at cash register
 * uniformly distributed between 1 and 2 minutes */
int myRNG::nextPos() {
	return floor(diff * next() + low);
}

/* this constructor sets all values used in Lehmer's algorithm
 * to the provided values setting up an RNG */
void myRNG::setLimits(float newLow, float newHigh) {
	low = newLow;
	diff = newHigh - newLow;
}

/* resets x to initial value */
void myRNG::resetSeed(float newseed){
	x = newseed;
}
