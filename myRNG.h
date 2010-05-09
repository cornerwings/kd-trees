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
	float next(){
		t = a * fmod(x, q) - r * floor(x / q);
		if (t < 0) {
			t = t + m;
		}
		x = t;
		return (x / m);
	}

	/* returns a time spent at cash register
	 * uniformly distributed between 1 and 2 minutes */
	int nextPos() {
		return floor(diff * next() + low);
	}

	/* this constructor sets all values used in Lehmer's algorithm
	 * to the provided values setting up an RNG */
	void setLimits(float newLow, float newHigh) {
		low = newLow;
		diff = newHigh - newLow;
	}

	/* resets x to initial value */
	void resetSeed(float newseed){
		x = newseed;
	}
};
