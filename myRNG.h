#include <math.h>
#include <stdio.h>

/* This class provides a random number generator... */
class myRNG {

private:
	float a, m, q, r, x, t, low, diff;

public:
	myRNG( void );

	/* using Lehmer's algorithm (ensuring no overflow)
	 * gets a float uniformly distributed between 0 and 1 */
	float next();

	/* returns a time spent at cash register
	 * uniformly distributed between 1 and 2 minutes */
	int nextPos();

	/* this constructor sets all values used in Lehmer's algorithm
	 * to the provided values setting up an RNG */
	void setLimits(float newLow, float newHigh);

	/* resets x to initial value */
	void resetSeed(float newseed);
};
