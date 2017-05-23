#include "Regression.h"
#include <Eigen/Dense>
#include "VPPException.h"


// Constructor
Regression::Regression(Eigen::MatrixXd& x, Eigen::MatrixXd& y, Eigen::MatrixXd& z ) :
xp_(x),
yp_(y),
zp_(z) {

	if( (xp_.rows() != yp_.rows()) || (xp_.rows() != zp_.rows()) )
		throw VPPException(HERE, "The coordinate arrays row size mismatch");

	if( (xp_.cols() != yp_.cols()) || (xp_.cols() != zp_.cols()) )
		throw VPPException(HERE, "The coordinate arrays col size mismatch");

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
