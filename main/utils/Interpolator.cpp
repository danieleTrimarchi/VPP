#include "Interpolator.h"
#include <math.h>
#include "VPPException.h"
#include "VppXYCustomPlotWidget.h"

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
	X_.resize(X0.size());
	Y_.resize(Y0.size());

	// Copy the values. Todo dtrimarchi : the copy can be avoided simply using
	// Eigen utils such as Maps!
	for(size_t i=0; i<X0.size(); i++){
		X_[i]=X0(i);
		Y_[i]=Y0(i);
	}

	// Generate the underlying spline
	generate();

}

// Interpolate the function X-Y for the value val
void SplineInterpolator::generate() {

	// throw if x is not sorted (todo dtrimarchi: instantiate a sorter)
	for(size_t i=1; i<X_.size(); i++)
		if(X_[i]<=X_[i-1])
			throw VPPException(HERE,"In SplineInterpolator, vector not sorted");

	// Instantiate a spline out of the XY vectors
	s_.set_points(X_,Y_);

}


// Destructor
SplineInterpolator::~SplineInterpolator() {

}

// How many points are used to build this spline?
size_t SplineInterpolator::getNumPoints() const {
	return X_.size();
}

double SplineInterpolator::interpolate(double val) {

	return s_(val);
}

// Plot the spline and its underlying source points.
// Hand the points to a QCustomPlot
void SplineInterpolator::plot(VppXYCustomPlotWidget* plot, double minVal,double maxVal,int nVals) {

	double dx= (maxVal-minVal)/(nVals);

  // Copy the data to the current plotting container
  QVector<double> x(nVals+1), y(nVals+1);
  for (int i = 0; i < nVals+1; i++) {
    x[i] = minVal + i*dx;
    y[i] = s_(x[i]);
  }

  // create graph and assign data to it:
  plot->addData(x, y, "Interpolated");

  // Also add the point data - mark them with blue crosses
  // Fill the data for graph 1, that contains the originating data
  QVector<double> xp(getNumPoints()), yp(getNumPoints());
  for (int i = 0; i < getNumPoints(); i++) {
  	xp[i]= X_[i];
  	yp[i]= Y_[i];
  }

  // Add the data to the plot
  plot->addData(xp, yp,"Primitive data");

  // Set the last curve style to be no curve but only red cross markers
  plot->graph()->setPen(QPen(Qt::red));
  plot->graph()->setLineStyle(QCPGraph::lsNone);
  plot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 4));

  // Set the plot bounds in a reasonable way
  plot->rescaleAxes();

}

// Plot the first derivative of the spline
void SplineInterpolator::plotD1(VppXYCustomPlotWidget* plot, double minVal,double maxVal,int nVals ) {

	double dx= (maxVal-minVal)/(nVals);

	// Generate the n.points for the current plot
	std::vector<double> x(nVals+1), y(nVals+1);
	for(size_t i=0; i<nVals+1; i++){
		x[i] = minVal + i*dx;
		y[i] = s_(x[i]);
	}

	// now compute the first derivative of this curve.
	QVector<double> x1(nVals), y1(nVals);
	for(size_t i=0; i<nVals; i++){
		x1[i] = (x[i]+x[i+1])/2;
		y1[i] = (y[i+1]-y[i])/(x[i+1]-x[i]);
	}

  // create graph and assign data to it:
  plot->addData(x1, y1);
  plot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));

  // Set the plot bounds in a reasonable way
  plot->rescaleAxes();

}

// Plot the second derivative spline
void SplineInterpolator::plotD2(VppXYCustomPlotWidget* plot, double minVal,double maxVal,int nVals ) {

	double dx= (maxVal-minVal)/(nVals);

	// Generate the n.points for the current plot
	QVector<double> x(nVals+1), y(nVals+1);
	for(size_t i=0; i<nVals+1; i++){
		x[i] = minVal + i*dx;
		y[i] = s_(x[i]);
	}

	x.pop_front();
	x.pop_back();

	// Compute the second derivative of this curve
	QVector<double> x2(nVals-1), y2(nVals-1);
	for(size_t i=1; i<nVals; i++){
		y2[i-1] = ( y[i+1] - 2 * y[i] + y[i-1] ) / (dx * dx) ;
	}

  // create graph and assign data to it:
  plot->addData(x, y2);
  plot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));

  // Set the plot bounds in a reasonable way
  plot->rescaleAxes();

}

////////////////////////////////////////////////////////////////////////////////////

// Constructor
Extrapolator::Extrapolator(	double xm2, const Eigen::VectorXd* vm2,
														double xm1, const Eigen::VectorXd* vm1) :
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
Eigen::VectorXd Extrapolator::get(double x) {

	// Instantiate the extrapolated vector
	Eigen::VectorXd vRes(pVm2_->size());

	// Distance between the two known solutions
	double dx= xm1_-xm2_;

	for(size_t i=0; i<pVm2_->size(); i++){

		// Compute the angular coeff for this set of coeffs
		double m= (pVm1_->coeff(i)-pVm2_->coeff(i))/dx;

		// y=mx+q  => (y2-y1)/(x2-x1) = (yext-y1)/(xext-x1)
		// yext = y1 + (y2-y1 / x2-x1) * xext-x1
		//      = y1 + m * xext-x1
		// with __1 => Vm2 and __2 Vm1
		vRes(i)= pVm2_->coeff(i) + m * (x-xm2_);
	}

	return vRes;
}

