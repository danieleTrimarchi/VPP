#include "Interpolator.h"
#include <math.h>
#include "Plotter.h"
#include "VPPException.h"

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

	// throw if out of bounds
	if( val<X(0) || val>X(X.size()-1) )
		throw VPPException(HERE,"the interpolator is not supposed to extrapolate!");

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

	size_t nVals=100;
	size_t extrapVals=20;

	// compute a dx dividing the range by 1000
	double dx = ( VALS(0,4) - VALS(0,0) ) / nVals;

	// Define a container for the interpolated values
	Eigen::ArrayXXd InterpVals;
	InterpVals.resize(2,nVals+extrapVals);

	Eigen::ArrayXd x=VALS.row(0);
	Eigen::ArrayXd y=VALS.row(1);

	for(size_t i=0; i<nVals;i++) {

		InterpVals(0,i) = i*dx;
		InterpVals(1,i) = interpolate(i*dx,x,y);

	}

	// Now append 20 values that are to be extrapolated
	for(size_t i=nVals; i<nVals+extrapVals;i++) {

		InterpVals(0,i) = i*dx;
		InterpVals(1,i) = interpolate(i*dx,x,y);

	}

	// Now plot the interpolated values

	ArrayXd x0=VALS.row(0);
	ArrayXd y0=VALS.row(1);
	ArrayXd x1=InterpVals.row(0);
	ArrayXd y1=InterpVals.row(1);

	Plotter plotter;
	plotter.plot(x0,y0,x1,y1);

}

////////////////////////////////////////////////////////////////////////////////////

// Default constructor (test)
SplineInterpolator::SplineInterpolator() {

	//	std::vector<double> X(5), Y(5);
	//	X[0]=0.1; X[1]=0.4; X[2]=1.2; X[3]=1.8; X[4]=2.0;
	//	Y[0]=0.1; Y[1]=0.7; Y[2]=1.5; Y[3]=1.1; Y[4]=0.9;
	//
	//	// Generate the underlying spline
	//	generate(X,Y);
	//
	//	size_t nPoints=50;
	//
	//	ArrayXd x0(5);
	//	ArrayXd y0(5);
	//	for(size_t i=0; i<5; i++){
	//		x0(i)=X[i];
	//		y0(i)=Y[i];
	//	}
	//
	//	ArrayXd x1(nPoints);
	//	ArrayXd y1(nPoints);
	//
	//	double dx= (X[4]-X[0])/(nPoints-1);
	//
	//	for(size_t i=0; i<nPoints; i++){
	//		x1(i)=X[0]+i*dx-0.5;
	//		y1(i)= s_( x1(i) );
	//	}
	//
	//	Plotter plotter;
	//	plotter.plot(x0,y0,x1,y1);

}

// Constructor
SplineInterpolator::SplineInterpolator(Eigen::ArrayXd& X0,Eigen::ArrayXd& Y0) {

	if(X0.size() != Y0.size())
		throw VPPException(HERE,"In SplineInterpolator: Size mismatch");

	// transform the Eigen arrays into vectors
	std::vector<double> X(X0.size()), Y(Y0.size());

	// Copy the values
	for(size_t i=0; i<X0.size(); i++){
		X[i]=X0(i);
		Y[i]=Y0(i);
	}

	// Generate the underlying spline
	generate(X,Y);

}

SplineInterpolator::SplineInterpolator(std::vector<double>& x, std::vector<double>& y) {

	if(x.size() != y.size())
		throw VPPException(HERE,"In SplineInterpolator: Size mismatch");

	// Generate the underlying spline
	generate(x,y);

}

// Interpolate the function X-Y for the value val
void SplineInterpolator::generate(std::vector<double>& x, std::vector<double>& y ) {

	// throw if x is not sorted (todo dtrimarchi: instantiate a sorter)
	for(size_t i=1; i<x.size(); i++)
		if(x[i]<=x[i-1])
			throw VPPException(HERE,"In SplineInterpolator, vector not sorted");

	// Instantiate a spline out of the xy vectors
	s_.set_points(x,y);

}


// Destructor
SplineInterpolator::~SplineInterpolator() {

}


double SplineInterpolator::interpolate(double val) {

	return s_(val);
}

// Plot the spline and its underlying source points
void SplineInterpolator::plot(double minVal,double maxVal,int nVals,
		string title, string xLabel, string yLabel) {

	std::vector<double> x(nVals+1), y(nVals+1);
	double dx= (maxVal-minVal)/(nVals);

	// Generate the n.points for the current plot
	for(size_t i=0; i<nVals+1; i++){
		x[i] = minVal + i*dx;
		y[i] = s_(x[i]);
	}

	// Instantiate a plotter and prepare the data
	Plotter plotter;
	std::vector<double> x0(s_.get_points(0));
	std::vector<double> y0(s_.get_points(1));

	// Ask the plotter to plot the curves
	plotter.plot(x0,y0,x,y,title,xLabel,yLabel);

}


////////////////////////////////////////////////////////////////////////////////////

// Constructor
Extrapolator::Extrapolator(	double xm2, const std::vector<double>* vm2,
														double xm1, const std::vector<double>* vm1) :
				xm2_(xm2),
				pVm2_(vm2),
				xm1_(xm1),
				pVm1_(vm1) {

	// check the size of the base vector is equal
	if(pVm2_->size() != pVm1_->size())
		throw VPPException(HERE, "In Extrapolator, base vector size mismatch");

	// Allocate the space for the vector with the extrapolated values
	v_.resize(pVm2_->size());
}

// Get the vector with the value extrapolated for the abscissa x
std::vector<double> Extrapolator::get(double x) {

	// Instantiate the extrapolated vector
	std::vector<double> vRes(pVm2_->size());

	// Distance between the two known solutions
	double dx= xm1_-xm2_;

	for(size_t i=0; i<pVm2_->size(); i++){

		// Compute the angular coeff for this set of coeffs
		double m= (pVm1_->at(i)-pVm2_->at(i))/dx;

		// y=mx+q  => (y2-y1)/(x2-x1) = (yext-y1)/(xext-x1)
		// yext = y1 + (y2-y1 / x2-x1) * xext-x1
		//      = y1 + m * xext-x1
		// with __1 => Vm2 and __2 Vm1
		vRes[i]= pVm2_->at(i) + m * (x-xm2_);
	}

	return vRes;
}

