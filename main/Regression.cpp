#include "Regression.h"
#include <Eigen/Dense>

// Constructor
Regression::Regression() {

	// Set the test polynomial vector
	Eigen::VectorXd polynomial(6);
	// x^2 xy y^2 x y 1
	polynomial << 0.01, .5, 0.03, .2, .3 , 1;

	// Set the number of points m
	size_t mx=3, my=3;

	// Set the coordinate arrays
	Eigen::MatrixXd x(mx,my), y(mx,my), z(mx,my);

	Eigen::VectorXd factsx(mx);
	factsx << 0.25, 0.5, 0.75;
	Eigen::VectorXd factsy(my);
	factsy << 0.25, 0.5, 0.75;

	Eigen::VectorXd coords(6);

	// Set the domain bounds
	double xMin=0, xMax=1, yMin=0, yMax=1;

	// Loop on the mx*my points
	for(size_t i=0; i<mx; i++){
		for(size_t j=0; j<my; j++){

			// compute xi, yi, zi
			x(i,j) = xMin + factsx(i) * ( xMax - xMin );
			y(i,j) = yMin + factsy(j) * ( yMax - yMin );

			// x^2 xy y^2 x y 1
			coords << x(i,j)*x(i,j), x(i,j)*y(i,j), y(i,j)*y(i,j), x(i,j), y(i,j), 1;

			// compute z with the given polynomial. We now have the nx*ny points to
			// be used to compute the regression and rebuild the initial polynomial
			Eigen::MatrixXd z = coords * polynomial.transpose();

			// Send this (scalar) value to the z matrix
			z(i,j) = z(0,0);
		}
	}

	// Declare the coordinate matrix A to be inverted
	Eigen::MatrixXd A(6,6);

	// Declare the rhs b
	Eigen::VectorXd b(6);

	// Loop on the mx*my points
	for(size_t i=0; i<mx; i++){
		for(size_t j=0; j<my; j++){

			// Define the coordinate vector for the current point
			Eigen::VectorXd Q(6);
			Q << x(i,j)*x(i,j),  x(i,j)*y(i,j), y(i,j)*y(i,j),x(i,j),y(i,j),1;

			// Add the local A matrix to the global one
			A += Q * Q.transpose();

			// Add the local RHS to the global one
			b += z(i,j) * Q;

		}
	}

	// Compute the polynomial array p = inv(A) * b
	Eigen::VectorXd p = A.colPivHouseholderQr().solve(b);

	// Compare p with the original polynomial array
	std::cout<<"Polynomial - p ="<<std::endl;
	std::cout<< polynomial-p<<std::endl;



}

// Destructor
Regression::~Regression() {

}

