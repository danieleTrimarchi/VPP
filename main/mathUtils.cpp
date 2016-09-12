#include "mathUtils.h"

namespace mathUtils {


// Define a smoothed step function, f(x) = 1 / ( 1 + e^(-(x-a)/b ) )
// This is a function that ranges from Zero to One and it is Cinf
// Ctor using ranges
SmoothedStepFunction::SmoothedStepFunction(double xMin, double xMax) :
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

double SmoothedStepFunction::f(double x) {
	return 1. / ( 1. + exp( - ( x - b_(0) ) / b_(1) ) );
};

// Dtor
SmoothedStepFunction::~SmoothedStepFunction(){ /* make nothing */ };

} // end namespace mathUtils
