#include "Regression.h"
#include <Eigen/Dense>
#include "VPPException.h"

// Test Constructor
Regression::Regression() {

	// Set the size of the coordinate matrices
	x_.resize(3,3);
	y_.resize(3,3);
	z_.resize(3,3);

	runTest();

}

// Constructor
Regression::Regression(Eigen::MatrixXd& x, Eigen::MatrixXd& y, Eigen::MatrixXd& z ) :
x_(x),
y_(y),
z_(z) {

	if( (x_.rows() != y_.rows()) || (x_.rows() != z_.rows()) )
		throw VPPException(HERE, "The coordinate arrays row size mismatch");

	if( (x_.cols() != y_.cols()) || (x_.cols() != z_.cols()) )
		throw VPPException(HERE, "The coordinate arrays col size mismatch");

}


// Destructor
Regression::~Regression() {

}

Eigen::VectorXd Regression::compute() {

	// Declare the coordinate matrix A to be inverted
	Eigen::MatrixXd A(6,6);

	// Declare the rhs b
	Eigen::VectorXd b(6);

	// Loop on the mx*my points
	for(size_t i=0; i<x_.rows(); i++){
		for(size_t j=0; j<x_.cols(); j++){

			// Define the coordinate vector for the current point
			Eigen::VectorXd Q(6);
			Q << x_(i,j)*x_(i,j), x_(i,j)*y_(i,j), y_(i,j)*y_(i,j), x_(i,j), y_(i,j), 1;

			// Add the local A matrix to the global one
			A += Q * Q.transpose();

			// Add the local RHS to the global one
			b += z_(i,j) * Q;

		}
	}

	// Compute the polynomial array p = inv(A) * b
	return A.colPivHouseholderQr().solve(b);

}

void Regression::runTest() {


	// Fill the test coordinate matrices :

	// Set the test polynomial vector
	Eigen::VectorXd polynomial(6);
	// x^2 xy y^2 x y 1
	polynomial << 0.01, .5, 0.03, .2, .3 , 1;

	Eigen::VectorXd factsx(x_.rows());
	factsx << 0.25, 0.5, 0.75;
	Eigen::VectorXd factsy(y_.cols());
	factsy << 0.25, 0.5, 0.75;

	Eigen::VectorXd coords(6);

	// Set the domain bounds
	double xMin=0, xMax=1, yMin=0, yMax=1;

	// Loop on the mx*my points
	for(size_t i=0; i<x_.rows(); i++){
		for(size_t j=0; j<x_.cols(); j++){

			// compute xi, yi, zi
			x_(i,j) = xMin + factsx(i) * ( xMax - xMin );
			y_(i,j) = yMin + factsy(j) * ( yMax - yMin );

			// x^2 xy y^2 x y 1
			coords << x_(i,j)*x_(i,j), x_(i,j)*y_(i,j), y_(i,j)*y_(i,j), x_(i,j), y_(i,j), 1;

			// compute z with the given polynomial. We now have the nx*ny points to
			// be used to compute the regression and rebuild the initial polynomial
			z_(i,j) = coords.transpose() * polynomial;

		}
	}

	// Compute the polynomial array by polynomial regression on the given points
	Eigen::VectorXd p = compute();

	// Compare p with the original polynomial array
	//std::cout<< polynomial-p<<std::endl;

	if((polynomial-p).norm() > 1e-10)
		throw VPPException(HERE,"\n\n==>> Regression test failed! <<==\n\n");
	else
		std::cout<<"\n\n=>> Regression test succeeded! <<==\n\n"<<std::endl;

}

