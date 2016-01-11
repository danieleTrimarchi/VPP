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
	bool reversed=false;
	if( X(closest)<=val )
		secondIndex = closest+1;
	else {
		reversed=true;
		secondIndex = closest-1;
	}

	// Fix the case in which secondIndex is > than the size of the vector
	if(secondIndex>=X.size())
		secondIndex = closest-1;

	// Find the normalized distance mu=(x-x1)/(x2-x1). Note that mu does
	// not necessarily reaches one, because X(closest) and (secondIndex)
	// might be reversed and in this case it returns to zero in the second
	// part of the abscissa
	double mu = fabs(( val - X(closest) )/( X(secondIndex) - X(closest) ));

	// In the very first range use linear interpolation, then pass to cosine
	// that preserves c1 continuity on the rest of the plot
	bool useLinearInterp = (
			val<=X(1) &&
			 !reversed ? mu<=0.5 : (1-mu)<=0.5  );

	// Use linear for the very first trait, cosine for the rest
	if(useLinearInterp)
		return LinearInterpolate(double(Y(closest)),double(Y(secondIndex)),mu);
	else
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
	VALS(1,1) = 4;
	// Define point X2,Y2
	VALS(0,2) = 5;
	VALS(1,2) = 7;
	// Define point X3,Y3
	VALS(0,3) = 7.5;
	VALS(1,3) = 2.5;
	// Define point X3,Y3
	VALS(0,4) = 10;
	VALS(1,4) = 1.5;

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

	// Now plot the interpolated values

	ArrayXd x0=VALS.row(0);
	ArrayXd y0=VALS.row(1);
	ArrayXd x1=InterpVals.row(0);
	ArrayXd y1=InterpVals.row(1);

	Plotter plotter;
	plotter.plot(x0,y0,x1,y1);

}
