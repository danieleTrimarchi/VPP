#include "Interpolator.h"
#include <math.h>
#include "Plotter.h"

// Constructor
Interpolator::Interpolator() {
	// Make nothing
}

// Destructor
Interpolator::~Interpolator() {
	// Make nothing
}

// Interpolate the function X-Y for the value val
double Interpolator::interpolate(double val,Eigen::ArrayXd& X,Eigen::ArrayXd& Y) {

	// Subtract XMinusVal = (X^2 - val^2)^2
	Eigen::ArrayXd XMinusVal= (X.square() - val*val).square();

	// Find the index of the closest value to val
	int closest=0;
	XMinusVal.minCoeff(&closest);

	// Find index of the the second closest value
	int secondIndex=0;
	if( X(closest)<=val )
		secondIndex = closest+1;
	else
		secondIndex = closest-1;

	// Fix the case in which secondIndex is > than the size of the vector
	if(secondIndex>=X.size())
		secondIndex = closest-1;

	// Find the normalized distance mu=(x-x1)/(x2-x1)
	double mu = fabs(( val - X(closest) )/( X(secondIndex) - X(closest) ));

	return CosineInterpolate(double(Y(closest)),double(Y(secondIndex)),mu);

}

// Linearly interpolate
double Interpolator::LinearInterpolate(double y1,double y2,double mu) {
   return(y1*(1-mu)+y2*mu);
}

// Interpolate using cosines
double Interpolator::CosineInterpolate(double y1,double y2,double mu) {

	double mu2;

   mu2 = (1-cos(mu*M_PI))/2;
   return(y1*(1-mu2)+y2*mu2);
}

void Interpolator::test() {

	std::cout<<"Beginning of Interpolator::test() "<<std::endl;

	// Define an arbitrary function by points
	Eigen::ArrayXXd VALS;
	VALS.resize(2,5);

	// Define point X0,Y0
	VALS(0,0) = 0;
	VALS(1,0) = 3.5;
	// Define point X1,Y1
	VALS(0,1) = 3;
	VALS(1,1) = 7;
	// Define point X2,Y2
	VALS(0,2) = 5;
	VALS(1,2) = 7;
	// Define point X3,Y3
	VALS(0,3) = 7.5;
	VALS(1,3) = 2.5;
	// Define point X3,Y3
	VALS(0,4) = 10;
	VALS(1,4) = 4.5;

	// compute a dx dividing the range by 1000
	double dx = ( VALS(0,4) - VALS(0,0) ) / 100;

	// Define a container for the interpolated values
	Eigen::ArrayXXd InterpVals;
	InterpVals.resize(2,101);

	for(size_t i=0; i<InterpVals.cols();i++) {

		InterpVals(0,i) = i*dx;

		Eigen::ArrayXd x=VALS.row(0);
		Eigen::ArrayXd y=VALS.row(1);

		double interp=interpolate(i*dx,x,y);

		InterpVals(1,i) = interp;

	}

	std::cout<<"before calling the Plotter "<<std::endl;

	// Now plot the interpolated values
	Plotter plotter;
	ArrayXd x=InterpVals.row(0);
	ArrayXd y=InterpVals.row(1);

	std::cout<<"after calling the Plotter "<<std::endl;


	plotter.plot(x,y);

}
