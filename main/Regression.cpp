#include "Regression.h"
#include <Eigen/Dense>
#include "VPPException.h"
#include "Plotter.h"

// Test Constructor
Regression::Regression() {

	// Set the size of the coordinate matrices
	xp_.resize(3,3);
	yp_.resize(3,3);
	zp_.resize(3,3);

	runAnalyticalTest();

}

// Constructor
Regression::Regression(Eigen::MatrixXd& x, Eigen::MatrixXd& y, Eigen::MatrixXd& z ) :
xp_(x),
yp_(y),
zp_(z) {

	if( (xp_.rows() != yp_.rows()) || (xp_.rows() != zp_.rows()) )
		throw VPPException(HERE, "The coordinate arrays row size mismatch");

	if( (xp_.cols() != yp_.cols()) || (xp_.cols() != zp_.cols()) )
		throw VPPException(HERE, "The coordinate arrays col size mismatch");

	runNumericalTest();
}


// Destructor
Regression::~Regression() {

}

Eigen::VectorXd Regression::compute() {

	// Declare the coordinate matrix A to be inverted
	Eigen::MatrixXd A( Eigen::MatrixXd::Zero(6,6) );

	// Declare the rhs b
	Eigen::VectorXd b( Eigen::VectorXd::Zero(6) );

	// Loop on the mx*my points
	for(size_t i=0; i<xp_.rows(); i++){
		for(size_t j=0; j<xp_.cols(); j++){

			// Define the coordinate vector for the current point
			Eigen::VectorXd Q(6);
			Q << xp_(i,j)*xp_(i,j), xp_(i,j)*yp_(i,j), yp_(i,j)*yp_(i,j), xp_(i,j), yp_(i,j), 1;

			// Add the local A matrix to the global one
			A += Q * Q.transpose();

			// Add the local RHS to the global one
			b += zp_(i,j) * Q;

		}
	}

	//std::cout<<"A= \n"<<A<<std::endl;
	//std::cout<<"b= \n"<<b<<std::endl;

	// Compute the polynomial array p = inv(A) * b
	return A.colPivHouseholderQr().solve(b);

}

void Regression::runAnalyticalTest() {

	// Fill the test coordinate matrices with well chosen values:

	// Set the test polynomial vector
	Eigen::VectorXd polynomial(6);
	// x^2 xy y^2 x y 1
	polynomial << 0.01, .5, 0.03, .2, .3 , 1;

	Eigen::VectorXd factsx(xp_.rows());
	factsx << 0.25, 0.5, 0.75;
	Eigen::VectorXd factsy(yp_.cols());
	factsy << 0.25, 0.5, 0.75;

	Eigen::VectorXd coords(6);

	// Set the domain bounds
	double xMin=0, xMax=1, yMin=0, yMax=1;

	// Loop on the mx*my points
	for(size_t i=0; i<xp_.rows(); i++){
		for(size_t j=0; j<xp_.cols(); j++){

			// compute xi, yi, zi
			xp_(i,j) = xMin + factsx(i) * ( xMax - xMin );
			yp_(i,j) = yMin + factsy(j) * ( yMax - yMin );

			// x^2 xy y^2 x y 1
			coords << xp_(i,j)*xp_(i,j),
								xp_(i,j)*yp_(i,j),
								yp_(i,j)*yp_(i,j),
								xp_(i,j),
								yp_(i,j),
								1;

			// compute z with the given polynomial. We now have the nx*ny points to
			// be used to compute the regression and rebuild the initial polynomial
			zp_(i,j) = coords.transpose() * polynomial;

		}
	}

	// Compute the polynomial array by polynomial regression on the given point array
	Eigen::VectorXd p = compute();

	// Compare p with the original polynomial array
	//std::cout<< polynomial-p<<std::endl;

	if((polynomial-p).norm() > 1e-10)
		throw VPPException(HERE,"\n\n==>> Analytical regression test failed! <<==\n\n");
	else
		std::cout<<"\n\n=>> Analytical regression test succeeded! <<==\n\n"<<std::endl;

}

// Run a test : get the points from outside, reconstruct the regression
// and verify the difference in the z-values. This will be embedded into
// into an autotest
void Regression::runNumericalTest() {

	// Assume the coordinates have already been filled

	// Compute the polynomial array by polynomial regression on the given points
	Eigen::VectorXd p = compute();
	//std::cout<<"Numerical p= "<<p.transpose()<<std::endl;


	Eigen::MatrixXd z(xp_.rows(), yp_.cols());

	// Loop on nx and ny
	for(size_t i=0; i<xp_.rows(); i++){
		for(size_t j=0; j<yp_.cols(); j++){

			Eigen::VectorXd X(6);
			X << 	xp_(i,j)*xp_(i,j),
						xp_(i,j)*yp_(i,j),
						yp_(i,j)*yp_(i,j),
						xp_(i,j),
						yp_(i,j),
						1;

			z(i,j)= p.transpose() * X;

//			if( std::fabs( z(i,j) - zp_(i,j) ) > 1.e-4 ){
//				char msg[256];
//				sprintf(msg,"\n\n==>> Numerical regression test failed! <<== value= %f \n\n", std::fabs( z(i,j) - zp_(i,j) ) );
//				throw VPPException(HERE,msg);
//			}

		}
	}

	// Plot the points and its regression surface
	Eigen::ArrayXd x(xp_.rows()), y(yp_.cols());

	for(size_t i=0; i<xp_.rows(); i++)
		x(i) = xp_(i,0);
	for(size_t i=0; i<yp_.cols(); i++)
		y(i) = yp_(0,i);

	Eigen::ArrayXd xp(xp_.rows()*yp_.cols());
	Eigen::ArrayXd yp(xp_.rows()*yp_.cols());
	Eigen::ArrayXd zp(xp_.rows()*yp_.cols());
	size_t k=0;
	for(size_t i=0; i<xp_.rows(); i++){
		for(size_t j=0; j<yp_.cols(); j++){
			xp(k) = xp_(i,j);
			yp(k) = yp_(i,j);
			zp(k) = zp_(i,j);
			k++;
		}
	}

	MagnitudeColoredPlotter3d plotter(
			x,y,z,
			xp, yp, zp,
			"Regression Plot", "x", "y");

}
