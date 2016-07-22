#include "Regression.h"
#include <Eigen/Dense>
#include "VPPException.h"
#include "VPPPlotter.h"


// Constructor
Regression::Regression(Eigen::MatrixXd& x, Eigen::MatrixXd& y, Eigen::MatrixXd& z ) :
xp_(x),
yp_(y),
zp_(z) {

	if( (xp_.rows() != yp_.rows()) || (xp_.rows() != zp_.rows()) )
		throw VPPException(HERE, "The coordinate arrays row size mismatch");

	if( (xp_.cols() != yp_.cols()) || (xp_.cols() != zp_.cols()) )
		throw VPPException(HERE, "The coordinate arrays col size mismatch");

	// Test comparing the z- values of the regression wrt the source points
	// runNumericalTest();
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

	VPPMagnitudeColoredPlotter3d plotter(
			x,y,z,
			xp, yp, zp,
			"Regression Plot", "x", "y");

}
