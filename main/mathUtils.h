#ifndef MATHUTILS_H
#define MATHUTILS_H

#include "math.h"
#include <cmath>
#include "VPPException.h"
#include "Eigen/Core"
#include <Eigen/LU>

namespace mathUtils {

// Define a static utility function to be used to convert deg to rad
static double toRad( double deg ){
	return deg * M_PI / 180.0;
};

// Define a static utility function to be used to convert rad to deg
static double toDeg( double rad ){
	return rad * 180.0 / M_PI;
};

// Define a static utility function to check that a value is numeric and finite
static bool isNotValid( double val ){
	if( isnan(val) || isinf(val) )
		return true;

	return false;
};


// Define a smoothed step function, f(x) = 1 / ( 1 + e^(-(x-a)/b ) )
// This is a function that ranges from Zero to One and it is Cinf
class SmoothedStepFunction {

	public:

		// Ctor
		SmoothedStepFunction(double xMin, double xMax);

		// Compute the value of the function for a given x
		double f(double x);

		// Dtor
		~SmoothedStepFunction();

	private:

		// Vector containing the values of the coefficients of the function
		Eigen::VectorXd b_;

		// Value of eps, the tolerance value used as a limit at x1 and x2
		double eps_;

};
}

#endif
