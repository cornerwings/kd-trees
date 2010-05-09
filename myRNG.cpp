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
