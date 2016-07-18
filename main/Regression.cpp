#include "Regression.h"
#include <Eigen/Dense>
#include "VPPException.h"
#include "Plotter.h"

// Test Constructor
Regression::Regression() {

	// Set the size of the coordinate matrices
	x_.resize(3,3);
	y_.resize(3,3);
	z_.resize(3,3);

	runAnalyticalTest();

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

	//std::cout<<"A= \n"<<A<<std::endl;
	//std::cout<<"b= \n"<<b<<std::endl;

	// Compute the polynomial array p = inv(A) * b
	return A.colPivHouseholderQr().solve(b);

}

void Regression::runAnalyticalTest() {

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
		throw VPPException(HERE,"\n\n==>> Analytical regression test failed! <<==\n\n");
	else
		std::cout<<"\n\n=>> Analytical regression test succeeded! <<==\n\n"<<std::endl;

}

// Run a test : get the points from outside, reconstruct the regression
// and verify the difference in the z-values. This will be embedded into
// into an autotest
void Regression::runNumericalTest() {

	// Assume the coordinates have already been filled using the constructor with
	// three args

	// Compute the polynomial array by polynomial regression on the given points
	Eigen::VectorXd p = compute();
	//std::cout<<"Numerical p= "<<p.transpose()<<std::endl;

	Eigen::MatrixXd z(x_.rows(), y_.cols());

	// Loop on nx and ny
	for(size_t i=0; i<x_.rows(); i++){
		for(size_t j=0; j<y_.cols(); j++){

			Eigen::VectorXd Q(6);
			Q << x_(i,j)*x_(i,j), x_(i,j)*y_(i,j), y_(i,j)*y_(i,j), x_(i,j), y_(i,j), 1;

			z(i,j)= p.transpose() * Q;

//			if( std::fabs( z - z_(i,j) ) > 1e-10 ){
//				char msg[256];
//				sprintf(msg,"\n\n==>> Numerical regression test failed! <<== value= %f \n\n", std::fabs( z - z_(i,j) ) );
//				throw VPPException(HERE,msg);
//			} else
//					std::cout<<"\n\n=>> Numerical regression test succeeded! <<==\n\n"<<std::endl;


		}
	}

	// Plot the points and its regression surface
	Eigen::ArrayXd x(x_.rows()), y(y_.cols());

	for(size_t i=0; i<x_.rows(); i++)
		x(i) = x_(i,0);
	for(size_t i=0; i<y_.cols(); i++)
		y(i) = y_(0,i);


//	MagnitudeColoredPlotter3d plotter(
//			x,y,z_,x,y,z,
//			"regression plot", "x", "y");

}
