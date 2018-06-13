#include "VPPTest.h"
#include "Regression.h"
#include "VariableFileParser.h"
#include "boost/shared_ptr.hpp"
#include "SailSet.h"
#include "VPPItemFactory.h"
#include "NRSolver.h"
#include <nlopt.hpp>
#include "VPPJacobian.h"
#include "VPPGradient.h"
#include "SemiAnalyticalOptimizer.h"
#include "VPPSolver.h"
#include "mathUtils.h"
#include "VPPResultIO.h"
#include "IpIpoptApplication.hpp"

#include "VPPSolverFactoryBase.h"
#include "hs071_nlp.h"

namespace Test {

/// Test the variables parsed in the variable file
void TVPPTest::variableParseTest() {

	// Instantiate a parser with the variables
	VariableFileParser parser;

	// Parse the variables file
	parser.parse("testFiles/variableFile_test.txt");

	parser.print();

	// VPP CONFIGURATION
	CPPUNIT_ASSERT_EQUAL( parser.get("V_MIN"), 0. );
	CPPUNIT_ASSERT_EQUAL( parser.get("V_MAX"), 15. );
	CPPUNIT_ASSERT_EQUAL( parser.get("PHI_MIN"), 0. );
	CPPUNIT_ASSERT_EQUAL( parser.get("PHI_MAX"),1.4835298641951802);
	CPPUNIT_ASSERT_EQUAL( parser.get("B_MIN"), 0. );
	CPPUNIT_ASSERT_EQUAL( parser.get("B_MAX"), 3. );
	CPPUNIT_ASSERT_EQUAL( parser.get("F_MIN"), 0.4 );
	CPPUNIT_ASSERT_EQUAL( parser.get("F_MAX"), 1. );

	// WIND CONFIGURATION
	CPPUNIT_ASSERT_EQUAL( parser.get("VTW_MIN"), .5 );
	CPPUNIT_ASSERT_EQUAL( parser.get("VTW_MAX"), 10. );
	CPPUNIT_ASSERT_EQUAL( parser.get("NTW"), 45. );
	CPPUNIT_ASSERT_EQUAL( parser.get("TWA_MIN"), 0.6108652381980153 );
	CPPUNIT_ASSERT_EQUAL( parser.get("TWA_MAX"), 3.12413936106985 );
	CPPUNIT_ASSERT_EQUAL( parser.get("N_TWA"), 40. );

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

// Instantiate some items, write to xml, read them
// back and verify consistency
void TVPPTest::xmlIOTest() {

//	// Root of the model tree
//	boost::shared_ptr<SettingsItemRoot> pRootItem(new SettingsItemRoot);
//
//	// Instantiate some VppSettingItems
//	SettingsItemGroup* pVPPSettings = new SettingsItemGroup("VPP Settings");
//	pVPPSettings->setParent(pRootItem.get());
//	pRootItem->appendChild(pVPPSettings);
//	pVPPSettings->appendChild( new SettingsItemBounds<MetersPerSec>(Var::vBounds_,0,15));
//	pVPPSettings->appendChild( new SettingsItemBounds<Degrees>(Var::heelBounds_,-1e-5,85));
//	pVPPSettings->appendChild( new SettingsItemBounds<Meters>(Var::crewBounds_,0,3));
//	pVPPSettings->appendChild( new SettingsItemBounds<NoUnit>(Var::flatBounds_,0.4,1));
//
//	// Instantiate and populate a VariableFileParser
//	boost::shared_ptr<VariableFileParser> pVariableFileParser(new VariableFileParser(pRootItem.get()));
//
//	// Define the buffer file
//	QString xmlFileName("testFiles/vppSettingsTest.xml");
//
//	// Introduce a scope to make sure the file is closed at destruction
//	{
//		QFile outXml(xmlFileName);
//		if(!outXml.open(QFile::WriteOnly | QFile::Text))
//			std::cout<<"Cannot create output file: "<<xmlFileName.toStdString()<<std::endl;
//		VPPSettingsXmlWriterVisitor vw(&outXml);
//		pRootItem->accept(vw);
//	}
//
//	// Instantiate a new root
//	boost::shared_ptr<SettingsItemRoot> pRootItemTwo(new SettingsItemRoot);
//
//	// Read the items back in from the xml and assign them to
//	// the brand new root
//	{
//		QFile inXml(xmlFileName);
//		if(!inXml.open(QFile::ReadOnly | QFile::Text))
//			std::cout<<"Cannot read input file: "<<xmlFileName.toStdString()<<std::endl;
//		VPPSettingsXmlReaderVisitor vr(&inXml);
//		pRootItemTwo->accept(vr);
//	}
//
//	// Get the variables to a new VariableFileParser
//	boost::shared_ptr<VariableFileParser> pVariableFileParserTwo(new VariableFileParser(pRootItemTwo.get()));
//
//	// Compare the old and the new tree - they must match
//	CPPUNIT_ASSERT( *pRootItem == *pRootItemTwo );
//
//    // TODO! compare the variables contained in the two parsers

}

/// Test the variables we get when reading xml in the variablefile

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
	VariableFileParser parser;

	// Parse the variables file
	parser.parse("testFiles/variableFile_test.txt");

	// Instantiate the sailset
	pSails.reset( SailSet::SailSetFactory(parser) );

	// Instantiate the items
	pVppItems.reset( new VPPItemFactory(&parser,pSails) );

	// Define the state vector
	Eigen::VectorXd x(4);
	x << 1, 0.1, 0.9, 3;

	// Feed the items with the current state vector
	pVppItems->update(2,2,x);

	//==>> TEST RESISTANCE COMPONENTS @ LOW SPEED AND ANGLE

	Eigen::VectorXd baseLines(10);
	baseLines << 13.7793728326077, -0.320969, 7.2854033027145,
			-0.0538096001381927, 0., 5.11719177721191, 2.82574575845714,
			-0.264885, 0, 28.9450345146122;

	// ==== Compute and compare to baseline viscous resistance
	// std::cout<<"VISCOUS= "<<pVppItems->getViscousResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(0), pVppItems->getViscousResistanceItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline residual resistance
	//std::cout<<"RESID= "<<pVppItems->getResiduaryResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(1), pVppItems->getResiduaryResistanceItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline induced resistance
	//std::cout<<"INDUC= "<<pVppItems->getInducedResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(2), pVppItems->getInducedResistanceItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline Delta_FrictRes_Heel resistance
	//std::cout<<"dVISCOUS_HEEL= "<<pVppItems->getDelta_FrictionalResistance_HeelItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(3), pVppItems->getDelta_ViscousResistance_HeelItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline Delta_ResidRes_Heel resistance
	//std::cout<<"dRESID_HEEL= "<<pVppItems->getDelta_ResiduaryResistance_HeelItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(4), pVppItems->getDelta_ResiduaryResistance_HeelItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline ViscousRes_Keel resistance
	//std::cout<<"VISCOUS_KEEL= "<<pVppItems->getViscousResistanceKeelItem()->get()<<std::endl; //-> this does not plot
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(5), pVppItems->getViscousResistanceKeelItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline ViscousRes_Rudder resistance
	//std::cout<<"VISCOUS_RUDDER= "<<pVppItems->getViscousResistanceRudderItem()->get()<<std::endl; //-> this does not plot
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(6), pVppItems->getViscousResistanceRudderItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline ResidRes_Keel resistance
	//std::cout<<"RESID KEEL= "<<pVppItems->getResiduaryResistanceKeelItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(7), pVppItems->getResiduaryResistanceKeelItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline NegativeResistance resistance
	//std::cout<<"NEGATIVE= "<<pVppItems->getNegativeResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(8), pVppItems->getNegativeResistanceItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline TOTAL resistance
	//std::cout<<"TOTAL= "<<pVppItems->getResistance()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(9), pVppItems->getResistance(), 1.e-6 );

	//==>> TEST AEREO FORCE-MOMENT COMPONENTS
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 278.192490911082, pVppItems->getAeroForcesItem()->getLift(), 1.e-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 127.119449010664, pVppItems->getAeroForcesItem()->getDrag(), 1.e-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-22.1324116577083, pVppItems->getAeroForcesItem()->getFDrive(), 1.e-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 305.058310279037, pVppItems->getAeroForcesItem()->getFSide(), 1.e-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 1590.44929395063, pVppItems->getAeroForcesItem()->getMHeel(), 1.e-6 );

	// ASSIGN SOME REAL VELOCITY AND HEEL NOW

	x << 5, 0.9, 0.8, 3;

	// Feed the items with the current state vector
	pVppItems->update(5,5,x);

	//==>> TEST RESISTANCE COMPONENTS @ HIGH SPEED AND ANGLE

	baseLines << 258.926085131635, 2126.65183639941,
			215.616683502611, -32.4866766474215, 543.165486198652,
			91.9349840264342, 49.6072127078136,
			83.8661310956139,
			0, 3467.10323652955;

	// ==== Compute and compare to baseline viscous resistance
	// std::cout<<"VISCOUS= "<<pVppItems->getViscousResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(0), pVppItems->getViscousResistanceItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline residual resistance
	//std::cout<<"RESID= "<<pVppItems->getResiduaryResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(1), pVppItems->getResiduaryResistanceItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline induced resistance
	//std::cout<<"INDUC= "<<pVppItems->getInducedResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(2), pVppItems->getInducedResistanceItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline Delta_FrictRes_Heel resistance
	//std::cout<<"dFRICT_HEEL= "<<pVppItems->getDelta_ViscousResistance_HeelItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(3), pVppItems->getDelta_ViscousResistance_HeelItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline Delta_ResidRes_Heel resistance
	//std::cout<<"dRESID_HEEL= "<<pVppItems->getDelta_ResiduaryResistance_HeelItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(4), pVppItems->getDelta_ResiduaryResistance_HeelItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline ViscousRes_Keel resistance
	//std::cout<<"VISCOUS_KEEL= "<<pVppItems->getViscousResistanceKeelItem()->get()<<std::endl; //-> this does not plot
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(5), pVppItems->getViscousResistanceKeelItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline ViscousRes_Rudder resistance
	//std::cout<<"VISCOUS_RUDDER= "<<pVppItems->getViscousResistanceRudderItem()->get()<<std::endl; //-> this does not plot
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(6), pVppItems->getViscousResistanceRudderItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline ResidRes_Keel resistance
	//std::cout<<"RESID KEEL= "<<pVppItems->getResiduaryResistanceKeelItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(7), pVppItems->getResiduaryResistanceKeelItem()->get(),  1.e-6 );

	// ==== Compute and compare to baseline NegativeResistance resistance
	//std::cout<<"NEGATIVE= "<<pVppItems->getNegativeResistanceItem()->get()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(8), pVppItems->getNegativeResistanceItem()->get(), 1.e-6 );

	// ==== Compute and compare to baseline TOTAL resistance
	//std::cout<<"TOTAL= "<<pVppItems->getResistance()<<std::endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( baseLines(9), pVppItems->getResistance(), 1.e-6 );

	//==>> TEST AEREO FORCE-MOMENT COMPONENTS
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1211.82264674563, pVppItems->getAeroForcesItem()->getLift(), 1.e-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(385.382333087013, pVppItems->getAeroForcesItem()->getDrag(), 1.e-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-123.699174436113, pVppItems->getAeroForcesItem()->getFDrive(), 1.e-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1265.59558472088, pVppItems->getAeroForcesItem()->getFSide(), 1.e-6 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4122.16137715763, pVppItems->getAeroForcesItem()->getMHeel(), 1.e-6 );

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
	VariableFileParser parser;

	// Declare a ptr with the sail configuration
	// This is based on the variables that have been read in
	boost::shared_ptr<SailSet> pSails;

	// Declare a container for all the items that
	// constitute the VPP components (Wind, Resistance, RightingMoment...)
	boost::shared_ptr<VPPItemFactory> pVppItems;

	// Parse the variables file
	parser.parse("testFiles/variableFile_test.txt");

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

	CPPUNIT_ASSERT_DOUBLES_EQUAL( -171.797570228577, J(0,0), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(  340.33544921875, J(1,0), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( -29.4818544387817,  J(0,1), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( -31634.528503418, J(1,1), 1.e-6);

}

// Test the computation of the Gradient vector
void TVPPTest::gradientTest() {

	std::cout<<"=== Testing the computation of the Gradient vector === \n"<<std::endl;

	// Instantiate a parser with the variables
	VariableFileParser parser;

	// Declare a ptr with the sail configuration
	// This is based on the variables that have been read in
	boost::shared_ptr<SailSet> pSails;

	// Declare a container for all the items that
	// constitute the VPP components (Wind, Resistance, RightingMoment...)
	boost::shared_ptr<VPPItemFactory> pVppItems;

	// Parse the variables file
	parser.parse("testFiles/variableFile_test.txt");

	// Instantiate the sailset
	pSails.reset( SailSet::SailSetFactory(parser) );

	// Instantiate the items
	pVppItems.reset( new VPPItemFactory(&parser,pSails) );

	// Define a state vector: v, phi, crew, flat
	Eigen::VectorXd x(4);
	x << 2, 0.4, 2, .9;

	// Instantiate a VPPGradient
	VPPGradient G( x,pVppItems.get() );

	// Compute this Gradient
	G.run(3,6);

	//	std::cout.precision(15);
	//	for(int i=0; i<G.rows(); i++)
	//		printf("%d --  %8.12f\n",i,G(i));

	CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0,                 G(0), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.289655178785324,   G(1), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.000798352062702179,G(2), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.396439217858844,   G(3), 1.e-6);

}

// Test the Newton-Raphson algorithm
void TVPPTest::newtonRaphsonTest() {

	std::cout<<"=== Testing the Newton-Raphson algorithm === \n"<<std::endl;

	// Instantiate a parser with the variables
	VariableFileParser parser;

	// Declare a ptr with the sail configuration
	// This is based on the variables that have been read in
	boost::shared_ptr<SailSet> pSails;

	// Declare a container for all the items that
	// constitute the VPP components (Wind, Resistance, RightingMoment...)
	boost::shared_ptr<VPPItemFactory> pVppItems;

	// Parse the variables file
	parser.parse("testFiles/variableFile_test.txt");

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
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.06918693946213,   x(0), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0125926166566537, x(1), 1.e-6);
	CPPUNIT_ASSERT_EQUAL( 10, static_cast<int>(solver.getNumIters()));
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

	// Note that the tolerance is relatively high on the values of x[0] and x[1]
	// This is to be expected because the ISRES algorithm attempts finding the
	// global minimum but it does not detail well the local minimum. The stopping
	// criterion can vary based on the initial guess, that is randomly generated.
	CPPUNIT_ASSERT_DOUBLES_EQUAL( xp[0], 14.09789016015, 1.e-1);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( xp[1], 0.845411, 1.e-1);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( minf, -6958.48271784916, 6958*0.01);

	// Use a local opt algorithm and verify the results up to a tight tolerance
	std::cout<<"\n=== Keep solving g06 using NLOpt COBYLA algorithm === "<<std::endl;

	// Instantiate a NLOpobject and set the COBYLA (Constrained Optimization BY Linear Approximations)
	// algorithm and the dimensionality.
	// This is a derivative-free local optimization with nonlinear inequality and equality constraints
	nlopt::opt locOpt(nlopt::LN_COBYLA,dimension);

	locOpt.set_lower_bounds(lb);

	// Set the objective function
	locOpt.set_min_objective(myfunc_g06, NULL);

	// Set the constraint equations
	locOpt.add_inequality_constraint(myconstraint_g06, &data[0], 1e-8);
	locOpt.add_inequality_constraint(myconstraint_g06, &data[1], 1e-8);

	// Set the relative tolerance
	locOpt.set_xtol_rel(1e-6);

	// Reset the nlOpt iteration counter to 0
	optIterations=0;

	// Launch the optimization; negative retVal implies failure
	nlopt::result locResult = locOpt.optimize(xp, minf);

	if (locResult < 0) {
		printf("nlopt failed while attempting local optimization!\n");
	}
	else {
		printf("found minimum after %d evaluations\n", optIterations);
		printf("found minimum at f(%g,%g) = %0.10g\n", xp[0], xp[1], minf);
	}

	CPPUNIT_ASSERT_DOUBLES_EQUAL( xp[0], 14.095, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( xp[1], 0.842960789215449, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( minf, -6961.81387558016, 1.e-6);

}

/// Test ipOpt -- from example HS071_NLP
void TVPPTest::ipOptTest() {

	// Create a new instance of the nlp problem
	SmartPtr<HS071_NLP> mynlp = new HS071_NLP();

	// Create a new instance of IpoptApplication
	//  (use a SmartPtr, not raw)
	// We are using the factory, since this allows us to compile this
	// example with an Ipopt Windows DLL
	SmartPtr<IpoptApplication> app = IpoptApplicationFactory();
	app->RethrowNonIpoptException(true);

	// Change some options
	// Note: The following choices are only examples, they might not be
	//       suitable for your optimization problem.
	app->Options()->SetNumericValue("tol", 1e-7);
	app->Options()->SetStringValue("mu_strategy", "adaptive");
	app->Options()->SetStringValue("output_file", "ipopt.out");

	// Do not request the hessian matrix
	app->Options()->SetStringValue("hessian_approximation", "limited-memory");

	// Leave ipOpt silent
	app->Options()->SetNumericValue("print_level",0);
	app->Options()->SetNumericValue("file_print_level", 12);

	// The following overwrites the default name (ipopt.opt) of the
	// options file
	app->Options()->SetStringValue("option_file_name", "hs071.opt");

	// Initialize the IpoptApplication and process the options
	ApplicationReturnStatus status;
	status = app->Initialize();
	if (status != Solve_Succeeded)
		CPPUNIT_FAIL("Error during initialization of ipOpt!");

	// Ask Ipopt to solve the problem
	status = app->OptimizeTNLP(mynlp);

	if (status == Solve_Succeeded) {
		std::cout << std::endl << std::endl << "*** The problem solved!" << std::endl;
	}
	else
		CPPUNIT_FAIL("ipOpt failed to find the solution!");

	// Get the value of the state vector and of the objective (minimized) function
	//  std::cout<<"-- ipOpt Res in autotests -- "<<std::endl;
	//  std::cout<<"Solution = "<< mynlp->getSolution() <<std::endl;
	//  std::cout<<"Residuals = "<< mynlp->getConstraints()<<std::endl;
	//  std::cout<<"minimized function = "<< mynlp->getObjectiveValue() <<std::endl;
	//  std::cout<<"-----------------------------"<<std::endl;

	CPPUNIT_ASSERT_DOUBLES_EQUAL( mynlp->getSolution()(0), 1, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( mynlp->getSolution()(1), 4.743, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( mynlp->getSolution()(2), 3.82115, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( mynlp->getSolution()(3), 1.37940829321546, 1.e-6);

	CPPUNIT_ASSERT_DOUBLES_EQUAL( mynlp->getConstraints()(0), 25, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( mynlp->getConstraints()(1), 40, 1.e-6);

	CPPUNIT_ASSERT_DOUBLES_EQUAL( mynlp->getObjectiveValue(), 17.0140171402241, 1.e-6);

}

// Test a run on a complete computation point : initial guess, NR and solution with NLOpt
void TVPPTest::vppPointTest() {

	// Set the precision for cout in order to visualize results that can be used
	// to reset the baselines
	std::cout.precision(15);

	std::cout<<"=== Testing one point computed by the vpp === \n"<<std::endl;

	// Instantiate a parser with the variables
	VariableFileParser parser;

	// Declare a ptr with the sail configuration
	// This is based on the variables that have been read in
	boost::shared_ptr<SailSet> pSails;

	// Declare a container for all the items that
	// constitute the VPP components (Wind, Resistance, RightingMoment...)
	boost::shared_ptr<VPPItemFactory> pVppItems;

	// Parse the variables file
	parser.parse("testFiles/variableFile_test.txt");

	// Instantiate the sailset
	pSails.reset( SailSet::SailSetFactory(parser) );

	// Instantiate the items
	pVppItems.reset( new VPPItemFactory(&parser,pSails) );

	// Set this run for wind angle 5
	size_t aTW=5;

	// -- Testing the Optimizer -- ///////////////////////////////////////////

	// Instantiate an optimizer
	Optim::NLOptSolverFactory solver(pVppItems);

	// Loop on the first 5 wind VELOCITIES.
	// Five is of course arbitrary
	for(size_t vTW=0; vTW<6; vTW++){

		try{
			// Run the optimizer for the current wind speed/angle
			solver.run(vTW,aTW);
		}
		catch(...){
			std::cout<<"Something went very wrong while running the solver..."<<std::endl;
		}
	}

	Eigen::VectorXd res( solver.get()->getResult(1,aTW) );
	//std::cout<<"RESULT: \n"<<res<<std::endl;

	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.768063025310529, res(0), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.00888463301260944, res(1), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 4.89947023590891e-07, res(2), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.97729159534302, res(3), 1.e-6);

	// ---

	// Set new velocity/angle
	res= solver.get()->getResult(5,aTW);
	//std::cout<<"RESULT: \n"<<res<<std::endl;

	CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.50491772482954, res(0),  1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0., res(1), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.640257525949011, res(2), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 1, res(3), 1.e-6);

	// -- Testing the SAOASolver -- ///////////////////////////////////////////

	// Now repeat the exercise with the SAOA
	Optim::SAOASolverFactory saSolver(pVppItems);

	// Loop on the first 5 wind VELOCITIES.
	// Five is of course arbitrary
	for(size_t vTW=0; vTW<6; vTW++){

		std::cout<<"vTW="<<vTW<<"  "<<"aTW="<<aTW<<std::endl;

		try{
			// Run the optimizer for the current wind speed/angle
			saSolver.run(vTW,aTW);
		}
		catch(...){
			std::cout<<"Something went very wrong while running the SAOA solver..."<<std::endl;
		}
	}

	res= saSolver.get()->getResult(1,aTW);
	//std::cout<<"RESULT: \n"<<res<<std::endl;

	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.772151955931057, res(0), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.00843341309907809, res(1), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0110665537937012, res(2), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 1., res(3), 1.e-6);

	// ----

	res= saSolver.get()->getResult(5,aTW);
	//std::cout<<"RESULT: \n"<<res<<std::endl;

	CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.5017222715628, res(0), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0325185520083958, res(1), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.123838135080945, res(2), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 1., res(3), 1.e-6);

	// -- Testing the VPPSolver -- ///////////////////////////////////////////

	// Now repeat the exercise with the vppSolver
	Optim::SolverFactory vppSolver(pVppItems);

	// Loop on the first 5 wind VELOCITIES.
	// Five is of course arbitrary
	for(size_t vTW=0; vTW<6; vTW++){

		try{
			// Run the optimizer for the current wind speed/angle
			vppSolver.run(vTW,aTW);
		}
		catch(...){
			std::cout<<"Something went very wrong while running the SAOA solver..."<<std::endl;
		}
	}

	res= vppSolver.get()->getResult(0,aTW);
	//std::cout<<"RESULT: \n"<<res<<std::endl;

	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.57909955000501, res(0), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.00469213389863513, res(1), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0., res(2), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 1., res(3), 1.e-6);

	// ---

	res= vppSolver.get()->getResult(5,aTW);
	//std::cout<<"RESULT: \n"<<res<<std::endl;

	CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.50084955365504, res(0), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0402998751229445, res(1), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0., res(2), 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 1., res(3), 1.e-6);

}


// Make a full run, and compare the full results with a baseline
void TVPPTest::ipOptFullRunTest() {


	// Set the precision for cout in order to visualize results that can be used
	// to reset the baselines
	std::cout.precision(15);

	std::cout<<"=== Testing a full run of IPOPT in the VPP usage === \n"<<std::endl;

	// Instantiate a parser with the variables
	VariableFileParser parser;

	// Declare a ptr with the sail configuration
	// This is based on the variables that have been read in
	boost::shared_ptr<SailSet> pSails;

	// Declare a container for all the items that
	// constitute the VPP components (Wind, Resistance, RightingMoment...)
	boost::shared_ptr<VPPItemFactory> pVppItems;

	// Parse the variables file
	parser.parse("testFiles/variableFile_ipOptFullTest.txt");

	// Instantiate the sailset
	pSails.reset( SailSet::SailSetFactory(parser) );

	// Instantiate the items
	pVppItems.reset( new VPPItemFactory(&parser,pSails) );

	Optim::IppOptSolverFactory solverFactory(pVppItems);

	// Loop on the wind ANGLES and VELOCITIES
	for(size_t aTW=0; aTW<parser.get("N_TWA"); aTW++)
		for(size_t vTW=0; vTW<parser.get("NTW"); vTW++){

			std::cout<<"vTW="<<vTW<<"  "<<"aTW="<<aTW<<std::endl;

			try{

				// Run the optimizer for the current wind speed/angle
				solverFactory.run(vTW,aTW);

			}
			catch(...){
				//do nothing and keep going
			}
		}

	// Save the results (useful for debugging)
	VPPResultIO writer(&parser, solverFactory.get()->getResults());
	writer.write("testFiles/vppRunTest_curResults.vpp");

	// Now import some baseline results
	ResultContainer baselineResults(pVppItems->getWind());

	// Read the results from file and push them back to the baselineResults container
	VPPResultIO reader(&parser, &baselineResults);
	reader.parse("testFiles/vppRunTest_baseline.vpp");

	// Get a ptr to the current results
	ResultContainer* pCurrentResults= solverFactory.get()->getResults();

	// Compare the size of the results with the baseline
	CPPUNIT_ASSERT_EQUAL( baselineResults.windVelocitySize(), pCurrentResults->windVelocitySize() );
	CPPUNIT_ASSERT_EQUAL( baselineResults.windAngleSize(), pCurrentResults->windAngleSize() );

	// Now compare each result using CPPUnit comparators
	for(size_t iWv=0; iWv<baselineResults.windVelocitySize(); iWv++)
		for(size_t iWa=0; iWa<baselineResults.windAngleSize(); iWa++) {

			for(size_t iCmp=0; iCmp<solverFactory.get()->getDimension(); iCmp++)
				CPPUNIT_ASSERT_DOUBLES_EQUAL(
						baselineResults.get(iWv,iWa).getX()->coeffRef(iCmp),
						pCurrentResults->get(iWv,iWa).getX()->coeffRef(iCmp),
						1.e-6
				);
		}
}


// Test the SmoothedStepFunction values
void TVPPTest::smoothedTestFunctionTest() {

	std::cout<<"=== Testing smoothedStep function === \n"<<std::endl;

	double xMin=0, xMax=0.5;

	// Instantiate a SmoothedStepFunction
	mathUtils::SmoothedStepFunction sdf(0,0.5);

	// Verify the values of the function at the constraint points
	CPPUNIT_ASSERT_DOUBLES_EQUAL( sdf.f(xMin), 1e-3, 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( sdf.f(xMax), 0.999, 1.e-6);

	// Verify that all values <xMin are approx 0 and all values >xMax are approx 1
	// Remember we have enforced a value of 1e-3 on xMin and xMax, so tolerances
	// cannot be too tight
	CPPUNIT_ASSERT_DOUBLES_EQUAL( sdf.f(xMin-100), 0., 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( sdf.f(xMin-10), 0., 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( sdf.f(xMin-1), 0., 1.e-5);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( sdf.f(xMin-0.5), 0., 1.e-5);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( sdf.f(xMax+0.5), 1., 1.e-5);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( sdf.f(xMax+1), 1., 1.e-5);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( sdf.f(xMax+10), 1., 1.e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( sdf.f(xMax+100), 1., 1.e-6);

}

// Test the VPPResultIO utility : instantiate results, write them to file,
// read them back and check if the values are unchanged
void TVPPTest::vppResultIOTest() {

	std::cout<<"=== Testing VPP Results IO === \n"<<std::endl;

	// Instantiate a parser with the variables
	VariableFileParser parser;

	// Declare a ptr with the sail configuration
	// This is based on the variables that have been read in
	boost::shared_ptr<SailSet> pSails;

	// Declare a container for all the items that
	// constitute the VPP components (Wind, Resistance, RightingMoment...)
	boost::shared_ptr<VPPItemFactory> pVppItems;

	// Parse the variables file
	parser.parse("testFiles/variableFile_small_test.txt");

	// Instantiate the sailset
	pSails.reset( SailSet::SailSetFactory(parser) );

	// Instantiate the wind
	boost::shared_ptr<WindItem> pWind(new WindItem(&parser,pSails));

	// Instantiate a result container
	ResultContainer resWriteContainer(pWind.get());

	// Push some baseline results to the resultContainer
	//                   iWv,iWa, v,  phi, b,    f,   dF,   dM
	resWriteContainer.push_back(0, 0, 0.2, 0.3, 3.2, 0.9, 0.001, 0.002 );
	resWriteContainer.push_back(0, 1, 0.44,0.1, 2.2, 0.88, 0.004, 0.003 );
	resWriteContainer.push_back(1, 0, 1.4 ,0.4, 2.5, 0.48, 0.001, 0.002 );
	resWriteContainer.push_back(1, 1, 3.4 ,0.12,1.5, 0.87, 0.004, 0.003 );
	resWriteContainer.push_back(2, 3, 0.4 ,0.,1.45, 0.1, 0.0005, 0.001 );
	resWriteContainer.push_back(3, 4, 3.4 ,0.33,6.45, 11.1, 0.0005, 0.001 );

	// Mark some results as to be discarded
	resWriteContainer.remove(1,0);
	resWriteContainer.remove(1,0);
	resWriteContainer.remove(1,1);
	resWriteContainer.remove(3,4);

	// Write the results to a file named testResult.vpp
	VPPResultIO writer(&parser, &resWriteContainer);
	writer.write("testFiles/testResult.vpp");

	// Instantiate a new result container to push the results the writer has
	// written to file
	ResultContainer resReadContainer(pWind.get());

	// Read the results from file and push them back to a new ResultContainer
	VPPResultIO reader(&parser, &resReadContainer);

	reader.parse("testFiles/testResult.vpp");

	// Compare the results with the baseline
	CPPUNIT_ASSERT(resReadContainer.get(0,0) == resWriteContainer.get(0,0));
	CPPUNIT_ASSERT(resReadContainer.get(0,1) == resWriteContainer.get(0,1));
	CPPUNIT_ASSERT(resReadContainer.get(1,0) == resWriteContainer.get(1,0));
	CPPUNIT_ASSERT(resReadContainer.get(1,1) == resWriteContainer.get(1,1));
	CPPUNIT_ASSERT(resReadContainer.get(2,3) == resWriteContainer.get(2,3));
	CPPUNIT_ASSERT(resReadContainer.get(3,4) == resWriteContainer.get(3,4));

}


} // namespace Test




