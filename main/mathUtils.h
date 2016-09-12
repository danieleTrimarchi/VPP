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

			// Ctor using ranges: the value of the function varies from zero to 1
			// in the interval xMin, xMax
			SmoothedStepFunction(double xMin, double xMax) :
				b_(Eigen::VectorXd(2)),
				eps_(0.001) {

				// Declare matrix A
				Eigen::Matrix2d A;

				// Fill matrix A
				A(0,0) = 1;
				A(0,1) = - log((1-eps_)/eps_);
				A(1,0) = 1;
				A(1,1) = - log( eps_/(1-eps_));

				if( A.determinant()>0 ){

					// Declare vector x with the coordinates
					Eigen::VectorXd x(2);

					// Fill coordinate vector x
					x << xMin, xMax;

					// Compute b = inv(A) * x
					b_= A.inverse() * x;

				} else
						throw VPPException(HERE, "Det(A) <= 0");

				//std::cout<<"b= "<<b_.transpose()<<std::endl;

			};

			// Compute the value of the function for a given x
			double f(double x){
				return 1. / ( 1. + exp( - ( x - b_(0) ) / b_(1) ) );
			}

			// Dtor
			~SmoothedStepFunction(){ /* make nothing */ };

		private:

			// Vector containing the values of the coefficients of the function
			Eigen::VectorXd b_;

			// Value of eps, the tolerance value used as a limit at x1 and x2
			double eps_;

	};
}

#endif
