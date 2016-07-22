#include "VPPTest.h"
#include "VPPPlotter.h"
#include "Regression.h"
#include "VariableFileParser.h"
#include "boost/shared_ptr.hpp"
#include "SailSet.h"
#include "VPPItemFactory.h"

// Test the resistance components
void TVPPTest::resistanceTest() {
	std::cout<<"=== Running resistance tests === "<<std::endl;

	// Do as in main :

	// Instantiate a parser with the variables
	VariableFileParser parser("variableFile_test.txt");

	// Declare a ptr with the sail configuration
	// This is based on the variables that have been read in
	boost::shared_ptr<SailSet> pSails;

	// Declare a container for all the items that
	// constitute the VPP components (Wind, Resistance, RightingMoment...)
	boost::shared_ptr<VPPItemFactory> pVppItems;

	// Parse the variables file
	parser.parse();

	// Instantiate the sailset
	pSails.reset( SailSet::SailSetFactory(parser) );

	// Instantiate the items
	pVppItems.reset( new VPPItemFactory(&parser,pSails) );

	// Define the state vector
	Eigen::VectorXd x(4);
	x << 1, 0.1, 0.9, 3;

	// Feed the items with the current state vector
	pVppItems->update(2,2,x);

	Eigen::VectorXd baseLines(10);
	baseLines << 13.7794, -0.320969, 8.24592, -0.0451873, 0.0257853, 5.11719, 2.82575, -0.264885, 0, 29.94;

	// ==== Compute and compare to baseline frictional resistance
	// std::cout<<"FRICT= "<<pVppItems->getFrictionalResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getFrictionalResistanceItem()->get(),baseLines(0), 1.e-4 );

	// ==== Compute and compare to baseline residual resistance
	//std::cout<<"RESID= "<<pVppItems->getResiduaryResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getResiduaryResistanceItem()->get(),baseLines(1), 1.e-4 );

	// ==== Compute and compare to baseline induced resistance
	//std::cout<<"INDUC= "<<pVppItems->getInducedResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getInducedResistanceItem()->get(), baseLines(2), 1.e-4 );

	// ==== Compute and compare to baseline Delta_FrictRes_Heel resistance
	//std::cout<<"dFRICT_HEEL= "<<pVppItems->getDelta_FrictionalResistance_HeelItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getDelta_FrictionalResistance_HeelItem()->get(), baseLines(3), 1.e-4 );

	// ==== Compute and compare to baseline Delta_ResidRes_Heel resistance
	//std::cout<<"dRESID_HEEL= "<<pVppItems->getDelta_ResiduaryResistance_HeelItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getDelta_ResiduaryResistance_HeelItem()->get(), baseLines(4), 1.e-4 );

	// ==== Compute and compare to baseline ViscousRes_Keel resistance
	//std::cout<<"VISCOUS_KEEL= "<<pVppItems->getViscousResistanceKeelItem()->get()<<std::endl; //-> this does not plot
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getViscousResistanceKeelItem()->get(), baseLines(5), 1.e-4 );

	// ==== Compute and compare to baseline FrictionalRes_Rudder resistance
	//std::cout<<"VISCOUS_RUDDER= "<<pVppItems->getViscousResistanceRudderItem()->get()<<std::endl; //-> this does not plot
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getViscousResistanceRudderItem()->get(), baseLines(6), 1.e-4 );

	// ==== Compute and compare to baseline ResidRes_Keel resistance
	//std::cout<<"RESID KEEL= "<<pVppItems->getResiduaryResistanceKeelItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getResiduaryResistanceKeelItem()->get(), baseLines(7), 1.e-4 );

	// ==== Compute and compare to baseline NegativeResistance resistance
	//std::cout<<"NEGATIVE= "<<pVppItems->getNegativeResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getNegativeResistanceItem()->get(), baseLines(8), 1.e-4 );

	// ==== Compute and compare to baseline TOTAL resistance
	//std::cout<<"TOTAL= "<<pVppItems->getResistance()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getResistance(), baseLines(9), 1.e-4 );


}

// static method for regressionTest()
void setPointCoordinates(
		Eigen::VectorXd& polynomial,
		double xMin, double xMax, double yMin, double yMax,
		Eigen::MatrixXd& xp, Eigen::MatrixXd& yp, Eigen::MatrixXd& zp) {

	// Define the iso-parametric coordinates of the domain, where
	// the points are about to be calculated
	Eigen::VectorXd factsx(xp.rows());
	for(size_t i=0; i<xp.rows(); i++)
		factsx(i) = double(i) / xp.rows();
	Eigen::VectorXd factsy(yp.cols());
	for(size_t i=0; i<yp.cols(); i++)
		factsy(i) = double(i) / yp.cols();

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
	Eigen::MatrixXd xp(10,10), yp(10,10), zp(10,10);

	setPointCoordinates(polynomial,
			xMin, xMax, yMin, yMax,
			xp, yp, zp );

	// Instantiate a Regression
	Regression regr(xp, yp, zp );

	// Compute the polynomial array by polynomial regression on the given point array
	Eigen::VectorXd p = regr.compute();

	// Compare p with the original polynomial array
	//std::cout<< polynomial-p<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( polynomial.norm(), p.norm(), 1.e-8 );

	for(size_t i=0; i<p.size(); i++)
		CPPUNIT_ASSERT_DOUBLES_EQUAL( polynomial(i), p(i), 1.e-8 );


	////////////////////////////////////////////////////////////


	// Attempt with a new polynomial now
	Eigen::VectorXd polynomial2(6);
	// x^2 xy y^2 x y 1. These coefficient generate an hyperboloid
	// directed on xy. -5 < z < 5 is a good estimate
	polynomial2 << 0.1, -.5, 0.0, -1.2, .3 , 0;

	// Set the domain bounds
	xMin=-10,	xMax=10,	yMin=-10, yMax=10;

	setPointCoordinates(polynomial2,
			xMin, xMax, yMin, yMax,
			xp, yp, zp );

	// Instantiate a Regression
	Regression regr2(xp, yp, zp );

	// Compute the polynomial array by polynomial regression on the given point array
	p = regr2.compute();

	// Compare p with the original polynomial array
	//std::cout<< polynomial-p<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( polynomial2.norm(), p.norm(), 1.e-8 );

	for(size_t i=0; i<p.size(); i++)
		CPPUNIT_ASSERT_DOUBLES_EQUAL( polynomial2(i), p(i), 1.e-8 );


	////////////////////////////////////////////////////////////

	// It is hard to estabilish a criterion,

	// Now perturb the solution with a pseudo-random map
	double perturbationFactor=.05;
	for(size_t i=0; i<zp.rows(); i++)
		for(size_t j=0; j<zp.cols(); j++){
			zp(i,j) *= ( 1 + perturbationFactor );
			perturbationFactor *= (-1);
		}

	// Instantiate a Regression
	Regression regr3(xp, yp, zp );

	// Compute the polynomial array by polynomial regression on the given point array
	p = regr3.compute();

	// Compute the values of z for the reconstructed polynomial
	Eigen::MatrixXd z_reconstructed(xp.rows(), yp.cols());

	// Loop on nx and ny
	for(size_t i=0; i<xp.rows(); i++){
		for(size_t j=0; j<yp.cols(); j++){

			Eigen::VectorXd X(6);
			X << 	xp(i,j)*xp(i,j),
						xp(i,j)*yp(i,j),
						yp(i,j)*yp(i,j),
						xp(i,j),
						yp(i,j),
						1;

			z_reconstructed(i,j)= p.transpose() * X;

		}
	}

//	// Plot the points and its regression surface
//	Eigen::ArrayXd x(xp.rows()), y(yp.cols());
//
//	for(size_t i=0; i<xp.rows(); i++)
//		x(i) = xp(i,0);
//	for(size_t i=0; i<yp.cols(); i++)
//		y(i) = yp(0,i);
//
//	Eigen::ArrayXd xpArr(xp.rows()*yp.cols());
//	Eigen::ArrayXd ypArr(xp.rows()*yp.cols());
//	Eigen::ArrayXd zpArr(xp.rows()*yp.cols());
//	size_t k=0;
//	for(size_t i=0; i<xp.rows(); i++){
//		for(size_t j=0; j<yp.cols(); j++){
//			xpArr(k) = xp(i,j);
//			ypArr(k) = yp(i,j);
//			zpArr(k) = zp(i,j);
//			k++;
//		}
//	}
//
//	VPPMagnitudeColoredPlotter3d plotter(
//			x, y, z_reconstructed,
//			xpArr, ypArr, zpArr,
//			"Regression Plot", "x", "y");

	for(size_t i=0; i<xp.rows(); i++){
		for(size_t j=0; j<yp.cols(); j++){
			CPPUNIT_ASSERT_DOUBLES_EQUAL( z_reconstructed(i,j), zp(i,j), fabs(2.1*perturbationFactor*zp.maxCoeff()));
		}
	}
}



