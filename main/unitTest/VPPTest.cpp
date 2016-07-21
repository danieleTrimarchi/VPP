#include "VPPTest.h"
#include "Regression.h"

// Test the resistance components
void TVPPTest::resistanceTest() {

}

// Test the regression algorithm
void TVPPTest::regressionTest() {

	std::cout<<"=== Testing the regression algorithm === "<<std::endl;

	// Set a test polynomial vector that will be reconstructed point-wise
	Eigen::VectorXd polynomial(6);
	// x^2 xy y^2 x y 1
	polynomial << 0.01, .5, 0.03, .2, .3 , 1;

	// Set the domain bounds
	double xMin=0, xMax=1, yMin=0, yMax=1;

	// Generate a grid of point coordinates
	Eigen::MatrixXd xp(3,3), yp(3,3), zp(3,3);

	// Define the iso-parametric coordinates of the domain, where
	// the points are about to be calculated
	Eigen::VectorXd factsx(xp.rows());
	factsx << 0.25, 0.5, 0.75;
	Eigen::VectorXd factsy(yp.cols());
	factsy << 0.25, 0.5, 0.75;

	Eigen::VectorXd coords(6);

	// Loop on the mx*my points
	for(size_t i=0; i<xp.rows(); i++){
		for(size_t j=0; j<xp.cols(); j++){

			// Compute xi, yi scaling the domain with factors
			xp(i,j) = xMin + factsx(i) * ( xMax - xMin );
			yp(i,j) = yMin + factsy(j) * ( yMax - yMin );

			// Compute the coordinate vector to feed the polynomial
			// x^2 xy y^2 x y 1
			coords << xp(i,j)*xp(i,j),
								xp(i,j)*yp(i,j),
								yp(i,j)*yp(i,j),
								xp(i,j),
								yp(i,j),
								1;

			// compute z with the given polynomial. We now have the nx*ny points to
			// be used to compute the regression and rebuild the initial polynomial
			zp(i,j) = coords.transpose() * polynomial;

		}
	}

	// Instantiate a Regression
	Regression regr(xp, yp, zp );

	// Compute the polynomial array by polynomial regression on the given point array
	Eigen::VectorXd p = regr.compute();

	// Compare p with the original polynomial array
	//std::cout<< polynomial-p<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( polynomial.norm(), p.norm(), 1.e-8 );

//	for(size_t i=0; i<p.size(); i++)
//		CPPUNIT_ASSERT_DOUBLES_EQUAL( polynomial(i), p(i), 1.e-8 );


}
