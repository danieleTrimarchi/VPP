#include "VPPTest.h"
#include "VPPPlotter.h"
#include "Regression.h"
#include "VariableFileParser.h"
#include "boost/shared_ptr.hpp"
#include "SailSet.h"
#include "VPPItemFactory.h"
#include "NRSolver.h"
#include <nlopt.hpp>
#include "VPPJacobian.h"
#include "Optimizer.h"

namespace Test {

/// Test the variables parsed in the variable file
void TVPPTest::variableParseTest() {

	// Instantiate a parser with the variables
	VariableFileParser parser("variableFile_test.txt");

	// Parse the variables file
	parser.parse();

	// VPP CONFIGURATION
	CPPUNIT_ASSERT_EQUAL( parser.get("V_MIN"), 0. );
	CPPUNIT_ASSERT_EQUAL( parser.get("V_MAX"), 15. );
	CPPUNIT_ASSERT_EQUAL( parser.get("PHI_MIN"), 0. );
	CPPUNIT_ASSERT_EQUAL( parser.get("PHI_MAX"), mathUtils::toRad(85) );
	CPPUNIT_ASSERT_EQUAL( parser.get("B_MIN"), 0. );
	CPPUNIT_ASSERT_EQUAL( parser.get("B_MAX"), 3. );
	CPPUNIT_ASSERT_EQUAL( parser.get("F_MIN"), 0.4 );
	CPPUNIT_ASSERT_EQUAL( parser.get("F_MAX"), 1. );

	// WIND CONFIGURATION
	CPPUNIT_ASSERT_EQUAL( parser.get("V_TW_MIN"), .5 );
	CPPUNIT_ASSERT_EQUAL( parser.get("V_TW_MAX"), 10. );
	CPPUNIT_ASSERT_EQUAL( parser.get("N_TWV"), 45. );
	CPPUNIT_ASSERT_EQUAL( parser.get("ALPHA_TW_MIN"), mathUtils::toRad(35) );
	CPPUNIT_ASSERT_EQUAL( parser.get("ALPHA_TW_MAX"), mathUtils::toRad(179) );
	CPPUNIT_ASSERT_EQUAL( parser.get("N_ALPHA_TW"), 40. );

	// These data are measurements and estimates for a Freedom 25
	CPPUNIT_ASSERT_EQUAL( parser.get("DIVCAN"), 1.549 );
	CPPUNIT_ASSERT_EQUAL( parser.get("LWL"), 6.096 );
	CPPUNIT_ASSERT_EQUAL( parser.get("BWL"), 1.737 );
	CPPUNIT_ASSERT_EQUAL( parser.get("B"), 2.591 );
	CPPUNIT_ASSERT_EQUAL( parser.get("AVGFREB"), 0.853 );
	CPPUNIT_ASSERT_EQUAL( parser.get("XFB"), 3.483 );
	CPPUNIT_ASSERT_EQUAL( parser.get("XFF"), 3.483 );
	CPPUNIT_ASSERT_EQUAL( parser.get("CPL"), 0.55 );
	CPPUNIT_ASSERT_EQUAL( parser.get("HULLFF"), 1.0 );
	CPPUNIT_ASSERT_EQUAL( parser.get("AW"), 6.503 );
	CPPUNIT_ASSERT_EQUAL( parser.get("SC"), 7.432 );
	CPPUNIT_ASSERT_EQUAL( parser.get("CMS"), 0.710 );
	CPPUNIT_ASSERT_EQUAL( parser.get("T"), 1.372 );
	CPPUNIT_ASSERT_EQUAL( parser.get("TCAN"), 0.305 );
	CPPUNIT_ASSERT_EQUAL( parser.get("ALT"), 5.528 );
	CPPUNIT_ASSERT_EQUAL( parser.get("KG"), 0.305 );
	CPPUNIT_ASSERT_EQUAL( parser.get("KM"), 2.511 );

	// KEEL
	CPPUNIT_ASSERT_EQUAL( parser.get("DVK"), 0.046 );
	CPPUNIT_ASSERT_EQUAL( parser.get("APK"), 1.007 );
	CPPUNIT_ASSERT_EQUAL( parser.get("ASK"), 0.850 );
	CPPUNIT_ASSERT_EQUAL( parser.get("SK"), 2.014 );
	CPPUNIT_ASSERT_EQUAL( parser.get("ZCBK"), 0.653 );
	CPPUNIT_ASSERT_EQUAL( parser.get("CHMEK"), 0.925 );
	CPPUNIT_ASSERT_EQUAL( parser.get("CHRTK"), 1.197 );
	CPPUNIT_ASSERT_EQUAL( parser.get("CHTPK"), 0.653 );
	CPPUNIT_ASSERT_EQUAL( parser.get("KEELFF"), 1. );
	CPPUNIT_ASSERT_EQUAL( parser.get("DELTTK"), 0. );
	CPPUNIT_ASSERT_EQUAL( parser.get("TAK"), 0.545 );

	// RUDDER
	CPPUNIT_ASSERT_EQUAL( parser.get("DVR"), 0. );
	CPPUNIT_ASSERT_EQUAL( parser.get("APR"), 0.480 );
	CPPUNIT_ASSERT_EQUAL( parser.get("SR"), 0.960 );
	CPPUNIT_ASSERT_EQUAL( parser.get("CHMER"), 0.490 );
	CPPUNIT_ASSERT_EQUAL( parser.get("CHRTR"), 0.544 );
	CPPUNIT_ASSERT_EQUAL( parser.get("CHTPR"), 0.435 );
	CPPUNIT_ASSERT_EQUAL( parser.get("DELTTR"), 0. );
	CPPUNIT_ASSERT_EQUAL( parser.get("RUDDFF"), 1. );

  // SAILS
	CPPUNIT_ASSERT_EQUAL( parser.get("SAILSET"), 3. );
	CPPUNIT_ASSERT_EQUAL( parser.get("P"), 8.9 );
	CPPUNIT_ASSERT_EQUAL( parser.get("E"), 4.084 );
	CPPUNIT_ASSERT_EQUAL( parser.get("MROACH"), 1.3 );
	CPPUNIT_ASSERT_EQUAL( parser.get("MFLB"), 0. );
	CPPUNIT_ASSERT_EQUAL( parser.get("BAD"), 0.610 );
	CPPUNIT_ASSERT_EQUAL( parser.get("I"), 8.626 );
	CPPUNIT_ASSERT_EQUAL( parser.get("J"), 1.890 );
	CPPUNIT_ASSERT_EQUAL( parser.get("LPG"), 4.115 );
	CPPUNIT_ASSERT_EQUAL( parser.get("SL"), 8.077 );
	CPPUNIT_ASSERT_EQUAL( parser.get("EHM"), 9.754 );
	CPPUNIT_ASSERT_EQUAL( parser.get("EMDC"), 0.254 );

	// CREW
	CPPUNIT_ASSERT_EQUAL( parser.get("MMVBLCRW"), 228. );

}

// Test the resistance components
void TVPPTest::itemComponentTest() {
	std::cout<<"=== Running item component (resistance, sailForce) tests === \n"<<std::endl;

	// Declare a ptr with the sail configuration
	// This is based on the variables that have been read in
	boost::shared_ptr<SailSet> pSails;

	// Declare a container for all the items that
	// constitute the VPP components (Wind, Resistance, RightingMoment...)
	boost::shared_ptr<VPPItemFactory> pVppItems;

	// Instantiate a parser with the variables
	VariableFileParser parser("variableFile_test.txt");

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

	//==>> TEST RESISTANCE COMPONENTS

	Eigen::VectorXd baseLines(10);
	baseLines << 13.7793728326077, -0.320969, 8.24591799105104,
			-0.0451873, 0.0257853, 5.11719177721191, 2.82574575845714,
			-0.264885, 0, 29.9399568769181;

	// ==== Compute and compare to baseline frictional resistance
	// std::cout<<"FRICT= "<<pVppItems->getFrictionalResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getFrictionalResistanceItem()->get(),baseLines(0), 1.e-6 );

	// ==== Compute and compare to baseline residual resistance
	//std::cout<<"RESID= "<<pVppItems->getResiduaryResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getResiduaryResistanceItem()->get(),baseLines(1), 1.e-6 );

	// ==== Compute and compare to baseline induced resistance
	//std::cout<<"INDUC= "<<pVppItems->getInducedResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getInducedResistanceItem()->get(), baseLines(2), 1.e-6 );

	// ==== Compute and compare to baseline Delta_FrictRes_Heel resistance
	//std::cout<<"dFRICT_HEEL= "<<pVppItems->getDelta_FrictionalResistance_HeelItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getDelta_FrictionalResistance_HeelItem()->get(), baseLines(3), 1.e-6 );

	// ==== Compute and compare to baseline Delta_ResidRes_Heel resistance
	//std::cout<<"dRESID_HEEL= "<<pVppItems->getDelta_ResiduaryResistance_HeelItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getDelta_ResiduaryResistance_HeelItem()->get(), baseLines(4), 1.e-6 );

	// ==== Compute and compare to baseline ViscousRes_Keel resistance
	//std::cout<<"VISCOUS_KEEL= "<<pVppItems->getViscousResistanceKeelItem()->get()<<std::endl; //-> this does not plot
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getViscousResistanceKeelItem()->get(), baseLines(5), 1.e-6 );

	// ==== Compute and compare to baseline FrictionalRes_Rudder resistance
	//std::cout<<"VISCOUS_RUDDER= "<<pVppItems->getViscousResistanceRudderItem()->get()<<std::endl; //-> this does not plot
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getViscousResistanceRudderItem()->get(), baseLines(6), 1.e-6 );

	// ==== Compute and compare to baseline ResidRes_Keel resistance
	//std::cout<<"RESID KEEL= "<<pVppItems->getResiduaryResistanceKeelItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getResiduaryResistanceKeelItem()->get(), baseLines(7), 1.e-6 );

	// ==== Compute and compare to baseline NegativeResistance resistance
	//std::cout<<"NEGATIVE= "<<pVppItems->getNegativeResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getNegativeResistanceItem()->get(), baseLines(8), 1.e-6 );

	// ==== Compute and compare to baseline TOTAL resistance
	//std::cout<<"TOTAL= "<<pVppItems->getResistance()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( pVppItems->getResistance(), baseLines(9), 1.e-6 );

	//==>> TEST AEREO FORCE-MOMENT COMPONENTS
	CPPUNIT_ASSERT_DOUBLES_EQUAL(pVppItems->getAeroForcesItem()->getLift(),278.191922062844, 1.e-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(pVppItems->getAeroForcesItem()->getDrag(),127.118973356939, 1.e-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(pVppItems->getAeroForcesItem()->getFDrive(),-22.1321642219849, 1.e-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(pVppItems->getAeroForcesItem()->getFSide(),305.057611272351, 1.e-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(pVppItems->getAeroForcesItem()->getMHeel(),1590.44564961559, 1.e-6 );

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

	std::cout<<"=== Testing the regression algorithm === \n"<<std::endl;

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

// Test the computation of the Jacobian matrix
void TVPPTest::jacobianTest() {

	std::cout<<"=== Testing the computation of the Jacobian matrix === \n"<<std::endl;

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

	// Define a state vector: v, phi, crew, flat
	Eigen::VectorXd x(4);
	x << 2, 0.4, 2, .9;

	size_t subProblemSize=2;

	// Instantiate a Jacobian
	VPPJacobian J(x, pVppItems.get(), subProblemSize);

	// Compute the Jacobian matrix
	J.run(3,6);

	//	for(size_t i=0; i<J.rows(); i++)
	//		for(size_t j=0; j<J.cols(); j++)
	//			printf("(%d %d) %8.12f\n",i,j,J(i,j));

	CPPUNIT_ASSERT_DOUBLES_EQUAL( J(0,0), -168.144225120544, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( J(1,0), 340.367431640625, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( J(0,1), -27.884762287140, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( J(1,1), -31634.527587890625, 1.e-6);

}

// Test the Newton-Raphson algorithm
void TVPPTest::newtonRaphsonTest() {

	std::cout<<"=== Testing the Newton-Raphson algorithm === \n"<<std::endl;

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

	// Define the state vector: V, Phi, B, f
	Eigen::VectorXd x(4);
	x << .2, 0.1, .2, .99;

	size_t dimension=4, subPbsize=2;
	// Instantiate a NR solver
	NRSolver solver(pVppItems.get(),dimension,subPbsize);

	// ask the NR solver to solve -- as the Optimizer
	x.block(0,0,subPbsize,1) = solver.run(4,2,x).block(0,0,subPbsize,1);

	// compare the solution with a given refererence
	CPPUNIT_ASSERT_DOUBLES_EQUAL( x(0), 1.06887731574018, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( x(1), 0.0125887, 1.e-6);

}

// number of iterations required for this run
int optIterations = 0;

// Define static functions for the nlOpt test
// Set the objective function and gradient for NLOpt example1:
// Find min : sqrt(x2)
// Subjected to y>0, y>=(a1*x + b1)^3 , y>=(a2*x +b2)^3
double myfunc(unsigned n, const double *x, double *grad, void *my_func_data) {

	// Increment the number of iterations for each call of the objective function
	++optIterations;

	if (grad) {
		grad[0] = 0.0;
		grad[1] = 0.5 / sqrt(x[1]);
	}
	return sqrt(x[1]);
}

/// Struct requested by NLOpt example1. Coefficient for each constraint in the
/// shape : y >= (ax+b)^3
typedef struct {
		double a, b;
} my_constraint_data;

// Function requested for NLOpt example1. Set the constraint function
// Set the constraint function and gradient for example1:
// Find min : sqrt(x2)
// Subjected to y>0, y>=(a1*x + b1)^3 , y>=(a2*x +b2)^3
double myconstraint(unsigned n, const double *x, double *grad, void *data) {

	my_constraint_data *d = (my_constraint_data *) data;
	double a = d->a, b = d->b;
	if (grad) {
		grad[0] = 3 * a * (a*x[0] + b) * (a*x[0] + b);
		grad[1] = -1.0;
	}
	return ((a*x[0] + b) * (a*x[0] + b) * (a*x[0] + b) - x[1]);
}

// Test NLOpt
void TVPPTest::nlOptTest_mma() {

	std::cout<<"\n=== Testing NLOpt MMA algorithm === "<<std::endl;

	// Instantiate a NLOpobject and set the MMA (Method of Moving Asymptotes)
	// algorithm and the dimensionality.
	// This is a gradient-based local optimization including nonlinear inequality
	// constraints (but not equality constraints)
	nlopt::opt opt(nlopt::LD_MMA,2);

	// Set the and apply the lower lower bounds ( no need to apply the upper bounds,
	// which are inf by default
	std::vector<double> lb(2);
	lb[0] = -HUGE_VAL;
	lb[1] = 0;
	opt.set_lower_bounds(lb);

	// Set the objective function
	opt.set_min_objective(myfunc, NULL);

	// Set the coefficients for the constraint equations :
	// a1 = 2, b1=0, a2=-1, b2=1
	my_constraint_data data[2] = { {2,0}, {-1,1} };

	// Set the constraint equations
	opt.add_inequality_constraint(myconstraint, &data[0], 1e-8);
	opt.add_inequality_constraint(myconstraint, &data[1], 1e-8);

	// Set the relative tolerance
	opt.set_xtol_rel(1e-4);

	// Set some initial guess
	std::vector<double> xp(2);
	xp[0]= 1.234;
	xp[1]= 5.678;

	// Instantiate the minimum objective value, upon return
	double minf;

	// Reset the nlOpt iteration counter to 0
	optIterations=0;

	// Launch the optimization; negative retVal implies failure
	nlopt::result result = opt.optimize(xp, minf);

	if (result < 0) {
		printf("nlopt failed!\n");
	}
	else {
		printf("found minimum after %d evaluations\n", optIterations);
		printf("found minimum at f(%g,%g) = %0.10g\n", xp[0], xp[1], minf);
	}

	CPPUNIT_ASSERT_DOUBLES_EQUAL( xp[0], 0.333333, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( xp[1], 0.296296, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( minf, 0.5443310474, 1.e-6);

	///////////////////////////////////////////////////////////////////////
}

// Test NLOpt
void TVPPTest::nlOptTest_cobyla() {

	std::cout<<"\n=== Testing NLOpt COBYLA algorithm === "<<std::endl;

	// Execute example 2: COBYLA derivative-free algorithm

	// Instantiate a NLOpobject and set the COBYLA (Constrained Optimization BY Linear Approximations)
	// algorithm and the dimensionality.
	// This is a derivative-free local optimization with nonlinear inequality and equality constraints
	nlopt::opt opt(nlopt::LN_COBYLA,2);

	// Set the and apply the lower lower bounds ( no need to apply the upper bounds,
	// which are inf by default
	std::vector<double> lb(2);
	lb[0] = -HUGE_VAL;
	lb[1] = 0;
	opt.set_lower_bounds(lb);

	// Set the objective function
	opt.set_min_objective(myfunc, NULL);

	// Set the coefficients for the constraint equations :
	// a1 = 2, b1=0, a2=-1, b2=1
	my_constraint_data data[2] = { {2,0}, {-1,1} };

	// Set the constraint equations
	opt.add_inequality_constraint(myconstraint, &data[0], 1e-8);
	opt.add_inequality_constraint(myconstraint, &data[1], 1e-8);

	// Set the relative tolerance
	opt.set_xtol_rel(1e-4);

	// Set some initial guess
	std::vector<double> xp(2);
	xp[0]= 1.234;
	xp[1]= 5.678;

	// Instantiate the minimum objective value, upon return
	double minf;

	// Reset the nlOpt iteration counter to 0
	optIterations=0;

	// Launch the optimization; negative retVal implies failure
	nlopt::result result = opt.optimize(xp, minf);

	if (result < 0) {
		printf("nlopt failed!\n");
	}
	else {
		printf("found minimum after %d evaluations\n", optIterations);
		printf("found minimum at f(%g,%g) = %0.10g\n", xp[0], xp[1], minf);
	}

	CPPUNIT_ASSERT_DOUBLES_EQUAL( xp[0], 0.333329199075669, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( xp[1], 0.296199682914189, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( minf, 0.544242301658176, 1.e-6);

}

// Test NLOpt -- ISRES "Improved Stochastic Ranking Evolution Strategy" algorithm.
// Global optimization algorithm with non-linear equality constraints
void TVPPTest::runISRESTest(){

	std::cout<<"\n=== Testing NLOpt ISRES algorithm === "<<std::endl;

	// Set the dimension of this problem
	size_t dimension=2;

	// Instantiate a NLOpobject and set the ISRES "Improved Stochastic Ranking Evolution Strategy"
	// algorithm for nonlinearly-constrained global optimization
	///GN_ISRES
	nlopt::opt opt(nlopt::GN_ISRES,dimension);

	// Set the and apply the lower and the upper bounds for the constraints
	// -> make sure the bounds are larger than the initial
	// 		guess!
	std::vector<double> lb(2),ub(2);
	for(size_t i=0; i<2; i++){
		lb[i] = 0;
		ub[i] = 6.;
	}

	// Set the bounds for the constraints
	opt.set_lower_bounds(lb);
	opt.set_upper_bounds(ub);

	// Set the objective function to be minimized (or maximized, using set_max_objective)
	opt.set_min_objective(myfunc, NULL);

	// Set the coefficients for the constraint equations :
	// a1 = 2, b1=0, a2=-1, b2=1
	my_constraint_data data[2] = { {2,0}, {-1,1} };

	// Set the constraint equations
	opt.add_inequality_constraint(myconstraint, &data[0], 1e-8);
	opt.add_inequality_constraint(myconstraint, &data[1], 1e-8);

	// Set the relative tolerance
	opt.set_xtol_rel(1e-4);

	// Set some initial guess. Make sure it is within the
	// bounds that have been set
	std::vector<double> xp(dimension);
	xp[0]= 1.234;
	xp[1]= 5.678;

	// Instantiate the minimum objective value, upon return
	double minf;

	// Reset the nlOpt iteration counter to 0
	optIterations=0;

	// Launch the optimization; negative retVal implies failure
	nlopt::result result = opt.optimize(xp, minf);

	if (result < 0) {
		printf("nlopt failed!\n");
	}
	else {
		printf("found minimum after %d evaluations\n", optIterations);
		printf("found minimum at f(%g,%g) = %0.10g\n", xp[0], xp[1], minf);
	}

	CPPUNIT_ASSERT_DOUBLES_EQUAL( xp[0], 0.333068000574163, 1.e-3);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( xp[1], 0.296318043091439, 1.e-3);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( minf, 0.544348535721313, 1.e-3);

}


// Set the objective function and gradient for example1:
// Find min : sqrt(x2)
// Subjected to y>0, y>=(a1*x + b1)^3 , y>=(a2*x +b2)^3
double myfunc_g06(unsigned n, const double *x, double *grad, void *my_func_data) {

	// Increment the number of iterations for each call of the objective function
	++optIterations;

	return (std::pow((x[0]-10),3) + std::pow((x[1]-20),3) );

}

/// Struct requested by example g06. Coefficient for each constraint in the
/// shape : y >= (ax+b)^3
typedef struct {

		int s1;
		double a;
		int s2;
		double b, c;
} g06_constraint_data;

// Set the constraint function and gradient for example1:
// Set the inequality in the shape: (-1)^s1 (x1 + a)^2 + (-1)^s2 (x2 + b)^2 + c ≤ 0
double myconstraint_g06(unsigned n, const double *x, double *grad, void *data) {

	g06_constraint_data *d = (g06_constraint_data *) data;

	return (
			d->s1 * std::pow(x[0]+d->a,2) +
			d->s2 * std::pow(x[1]+d->b,2) +
			d->c
	);
}

// Test NLOpt -- ISRES "Improved Stochastic Ranking Evolution Strategy" algorithm.
// Global optimization algorithm with non-linear equality constraints. Here in then example g06
void TVPPTest::runISRESTest_g06(){

	std::cout<<"\n=== Testing NLOpt ISRES algorithm -- example06 === "<<std::endl;

	// g06:
	// Test function: f(⃗x) = (x1 − 10)^3 + (x2 − 20)^3
	// Constraints: g1(x) : −(x1 −5)^2 − (x2 −5)^2 + 100 ≤ 0
	//					 		g2(x) :  (x1 −6)^2 + (x2 −5)^2 − 82.81 ≤ 0
	// where 13 ≤ x1 ≤ 100 and 0 ≤ x2 ≤ 100.
	// The optimum solution is x = (14.095, 0.84296)
	// where f(x) = −6961.81388

	// Set the dimension of this problem
	size_t dimension=3;

	// Instantiate a NLOpobject and set the ISRES "Improved Stochastic Ranking Evolution Strategy"
	// algorithm for nonlinearly-constrained global optimization
	///GN_ISRES
	nlopt::opt opt(nlopt::GN_ISRES,dimension);

	// Set the and apply the lower and the upper bounds
	// -> make sure the bounds are larger than the initial
	// 		guess!

	std::vector<double> lb(dimension),ub(dimension);
	lb[0] = 13;
	ub[0] = 100;
	lb[1] = 0;
	ub[1] = 100;

	// Set the bounds for the constraints
	opt.set_lower_bounds(lb);
	opt.set_upper_bounds(ub);

	// Set the objective function to be minimized (or maximized, using set_max_objective)
	opt.set_min_objective(myfunc_g06, NULL);

	// Set the constraint equations
	g06_constraint_data data[2] = { {-1,-5,-1,-5,+100}, {1,-6,1,-5,-82.81} };

	opt.add_inequality_constraint(myconstraint_g06, &data[0], 1e-8);
	opt.add_inequality_constraint(myconstraint_g06, &data[1], 1e-8);

	// Set the relative tolerance
	opt.set_xtol_rel(1e-4);

	opt.set_maxeval(100000);

	// Set some initial guess. Make sure it is within the
	// bounds that have been set
	std::vector<double> xp(dimension);
	xp[0]= 13.5;
	xp[1]= 0.1;

	// Instantiate the minimum objective value, upon return
	double minf;

	// Launch the optimization; negative retVal implies failure
	nlopt::result result = opt.optimize(xp, minf);

	if (result < 0) {
		printf("nlopt failed!\n");
	}
	else {
		printf("found minimum after %d evaluations\n", optIterations);
		printf("found minimum at f(%g,%g) = %0.10g\n", xp[0], xp[1], minf);
	}

	CPPUNIT_ASSERT_DOUBLES_EQUAL( xp[0], 14.09789016015, 1.e-2);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( xp[1], 0.845411, 1.e-2);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( minf, -6958.48271784916, 6958*0.01);

}

/// Test a run on a complete computation point : initial guess, NR and solution with NLOpt
void TVPPTest::vppPointTest() {

	std::cout<<"=== Testing one point computed by the vpp === \n"<<std::endl;

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

	// Instantiate an optimizer
	Optim::Optimizer solver(pVppItems);

	// Set the input: twv and twa, and run the optimizer for the current wind speed/angle
	size_t vTW=0, aTW=5;
	solver.run(vTW,aTW);

	Eigen::VectorXd res( solver.getResult(vTW,aTW) );

	//std::cout<<"RESULT: \n"<<res<<std::endl;

	CPPUNIT_ASSERT_DOUBLES_EQUAL( res(0), 0.578552, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( res(1), 2.15527e-05, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( res(2), 0.0738957, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( res(3), 1., 1.e-6);

}

} // namespace Test




