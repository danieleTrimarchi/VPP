#ifndef MATHUTILS_H
#define MATHUTILS_H

#include "math.h"
#include <cmath>

namespace mathUtils {

	// Define a static utility function to be used to convert deg to rad
	static double toRad( double deg ){
		return deg * M_PI / 180.0;
	};

	// Define a static utility function to be used to convert rad to deg
	static double toDeg( double rad ){
		return rad * 180.0 / M_PI;
	};

}

#endif
