#include <stdio.h>
#include <iostream>
#include <fstream>
#include <getopt.h>

using namespace std;

// ------------------------
// Directives for EIGEN
#include <Eigen/Core>
using namespace Eigen;

#include "boost/shared_ptr.hpp"

#include "VariableFileParser.h"
#include "SailSet.h"
#include "VPPItem.h"
#include "Optimizer.h"
#include "NRSolver.h"
#include "Interpolator.h"
#include "VPPException.h"

/// Reload the variable file and update the items accordingly
void load(VariableFileParser& parser,
		boost::shared_ptr<SailSet>& pSailSet,
		boost::shared_ptr<VPPItemFactory>& pVppItems){

	// Parse the variables file
	parser.parse();

	// Instantiate the sailset
	pSailSet.reset( SailSet::SailSetFactory(parser) );

	// Reload the items
	pVppItems.reset( new VPPItemFactory(&parser,pSailSet) );

}

/// RUN
void run(VariableFileParser& parser, Optimizer& optimizer){

	// Loop on the wind ANGLES and VELOCITIES
	for(size_t aTW=0; aTW<parser.get("N_ALPHA_TW"); aTW++)
		for(size_t vTW=0; vTW<parser.get("N_TWV"); vTW++){

			std::cout<<"vTW="<<vTW<<"  "<<"aTW="<<aTW<<std::endl;

			// Run the optimizer for the current wind speed/angle
			optimizer.run(vTW,aTW);

		}
}

/// RUN
void run(VariableFileParser& parser, NRSolver& solver){

	// Loop on the wind ANGLES and VELOCITIES
	for(size_t aTW=0; aTW<parser.get("N_ALPHA_TW"); aTW++)
		for(size_t vTW=0; vTW<parser.get("N_TWV"); vTW++){

			std::cout<<"vTW="<<vTW<<"  "<<"aTW="<<aTW<<std::endl;

			// Run the optimizer for the current wind speed/angle
			solver.run(vTW,aTW);

		}
}

#include "math.h"
#include "mathUtils.h"

double f(double x){
	return sin(x);
}

// WARNING, UNDOCUMENTED OPTION!
// TODO: we do not test for derivatives close to zero
void testNR1() {

	// select a starting point
	double x = M_PI / 4 + 0.01;

	size_t maxIter=100;
	for( size_t iter=0; iter<maxIter+1; iter++){

		if(iter==maxIter)
			throw VPPException(HERE,"Could not converge!");

		std::cout<<"Iteration "<<iter<<endl;
		std::cout<<" x="<<x<<"; f= "<<f(x)<<std::endl;

		// Compute the optimum eps for this variable
		double eps= x * std::sqrt( std::numeric_limits<double>::epsilon() );

		// compute f'(x_i)= [ f(x+eps) - f(x-eps) ] / ( 2 * eps )
		double df= ( f(x+eps)-f(x-eps))/(2*eps);

		// compute x_(i+1) = x_i - f(x_i) / f'(x_i)
		x = x - f(x) / df;

		if( fabs(f(x)) < 0.001 ){
			cout<<"Solution found! \n";
			std::cout<<"x="<<x<<
					"; f= "<<f(x)<<std::endl;
			break;
		}
	}
}

double f2(Eigen::VectorXd& x){
	return sin(x(0))*x(1)*x(1);
}

double g2(Eigen::VectorXd& x){
	return x(0)*x(0)-x(1);
}

#include <Eigen/Dense>

void testNR2() {

	size_t dimension=2;
	// Select a starting point
	Eigen::VectorXd x(dimension);
	x << 10, 13;

	size_t maxIter=100;
	for( size_t iter=0; iter<maxIter+1; iter++){

		if(iter==maxIter)
			throw VPPException(HERE,"Could not converge!");

		std::cout<<"Iteration "<<iter<<endl;
		std::cout<<" x="<<x<<"; \n"<<std::endl;
		std::cout<<"f= "<<f2(x)<<" g= "<<g2(x)<<std::endl;

		// Instantiate and compute the Jacobian matrix
		// J = |df/dx1 df/dx2|
		//		 |dg/dx1 dg/dx2|
		Eigen::MatrixXd J(dimension,dimension);

		for(size_t j=0; j<dimension; j++){

			// compute eps(xj)
			double eps= x(j) * std::sqrt( std::numeric_limits<double>::epsilon() );

			// buffer the state vector
			Eigen::VectorXd xbuf=x;

			// set x(j)=x(j)+eps
			xbuf(j) = x(j) + eps;

			J.col(j)(0)=f2(xbuf);
			J.col(j)(1)=g2(xbuf);

			// set x(j)=x(j)-eps
			xbuf(j) = x(j) - eps;

			// J.col(j)-=f- , g-
			J.col(j)(0)-=f2(xbuf);
			J.col(j)(1)-=g2(xbuf);

			//J.col(j)/=2eps
			J.col(j) /= 2*eps;

		}

		// Compute the new solution x = x - J^-1 res
		Eigen::VectorXd residuals(2);
		residuals << f2(x), g2(x);

		// A * x = residuals --  J * deltas = residuals
		// where deltas are also equal to f(x_i) / f'(x_i)
		VectorXd deltas = J.colPivHouseholderQr().solve(residuals);

		x -= deltas;

		if( fabs(f2(x)) < 0.001 && fabs(g2(x)) < 0.001 ){
			cout<<"\n Solution found! \n";
			std::cout<<"x="<<x.transpose()<<
					"; f= "<<f2(x)<<", g= "<<g2(x)<<std::endl;
			break;
		}

	}

}

double fDrive(double V, double phi, boost::shared_ptr<SailSet>& pSails){

	// set the wind velocity at 5 m/s
	double wv=6;

	// Fdrive = pho S (V^2 + vw^2) cos(phi)
	double fDrive= ( V*V + wv*wv ) * cos(phi);
	if(isnan(fDrive))
		throw VPPException(HERE,"fDrive is NAN!");

	return fDrive;
}

double R(double V, VariableFileParser& parser){

	if(V<0.1)
		return 0.01;

	// compute Rn
	double rN= V * parser.get("LWL") * 0.7 / Physic::ni_w;

	// Compute the Frictional coefficient
	double cf=  0.075 / std::pow( (std::log10(rN) - 2), 2);

	// Compute the Frictional resistance of the bare hull
	double rfh = 0.5 * Physic::rho_w * parser.get("SC") * V * V * cf;

	double r= rfh * parser.get("HULLFF");
	if(isnan(r))
		throw VPPException(HERE,"r is NAN!");

	return r;
}

double Mw(double fDrive, double phi, boost::shared_ptr<SailSet>& pSails) {

	double mw= fDrive * pSails->get("ZCE") * cos(phi);
	if(isnan(mw))
		throw VPPException(HERE,"mw is NAN!");

	return mw;

}

double Rm(double phi, VariableFileParser& parser) {

	double rm= Physic::rho_w * Physic::g *
			(parser.get("KM") - parser.get("KG")) *
			(parser.get("DIVCAN") + parser.get("DVK")) *
			std::sin( phi ) ;
	if(isnan(rm))
		throw VPPException(HERE,"rm is NAN!");

	return rm;
}

void computeJacobian(	boost::shared_ptr<SailSet>& pSails,
											VariableFileParser& parser,
											Eigen::VectorXd x,Eigen::MatrixXd& J){

	for(size_t j=0; j<J.cols(); j++){

		// compute eps(xj)
		double eps= x(j) * std::sqrt( std::numeric_limits<double>::epsilon() );

		// buffer the state vector
		Eigen::VectorXd xbuf=x;

		// set x(j)=x(j)+eps
		xbuf(j) = x(j) + eps;

		double f_drive= fDrive( xbuf(0), xbuf(1), pSails );
		double Resistance= R( xbuf(0), parser );
		double HeelMoment= Mw( f_drive, xbuf(1), pSails);
		double rightMoment= Rm( xbuf(1),parser );

		J.col(j)(0) = f_drive-Resistance;
		J.col(j)(1) = HeelMoment-rightMoment;

		// set x(j)=x(j)-eps
		xbuf(j) = x(j) - eps;

		f_drive= fDrive( xbuf(0), xbuf(1), pSails );
		Resistance= R( xbuf(0), parser );
		HeelMoment= Mw( f_drive, xbuf(1), pSails);
		rightMoment= Rm( xbuf(1),parser );

		// J.col(j)-=f- , g-
		J.col(j)(0) -= f_drive-Resistance;
		J.col(j)(1) -= HeelMoment-rightMoment;

		//J.col(j)/=2eps
		J.col(j) /= ( 2 * eps );

	}
}

void solve(	boost::shared_ptr<SailSet>& pSails,
						VariableFileParser& parser,Eigen::VectorXd& x){

	size_t maxIter=20;
	for( size_t iter=0; iter<maxIter+1; iter++){

		if(iter==maxIter)
			throw VPPException(HERE,"Could not converge!");

		double f_drive= fDrive( x(0), x(1), pSails );
		double Resistance= R( x(0), parser );

		double HeelMoment= Mw( f_drive, x(1), pSails);
		double rightMoment= Rm( x(1),parser );

		// Compute the residuals for the current solution x
		Eigen::VectorXd residuals(x.size());
		residuals << f_drive-Resistance, HeelMoment-rightMoment;

		std::cout<<"Iteration "<<iter<<endl;
		std::cout<<" x="<<x.transpose()<<"; \n"<<std::endl;
		std::cout<<"dF= "<<residuals(0)<<" g= "<<residuals(1)<<std::endl;

		// Instantiate and compute the Jacobian matrix
		// J = |df/dx1 df/dx2|
		//	   |dg/dx1 dg/dx2|
		Eigen::MatrixXd J(x.size(),x.size());
		computeJacobian(pSails,parser,x,J);
		std::cout<<"J= "<<J<<std::endl;
        
		// A * x = residuals --  J * deltas = residuals
		// where deltas are also equal to f(x_i) / f'(x_i)
		VectorXd deltas = J.colPivHouseholderQr().solve(residuals);

		std::cout<<"deltas   = "<<deltas.transpose()<<std::endl;
		x -= deltas;
		std::cout<<"        x= "<<x.transpose()<<std::endl;

//        // limit the solution. 0<x<100 ; 0<phi<85
//		if(x(0)<0) x(0)=0.1;
//		if(x(0)>100) x(0)=100;
//		if(x(1)<0) x(1)=0.1;
//		if(x(1)>85) x(1)=85;
 
		std::cout<<"limited x= "<<x.transpose()<<std::endl;

		f_drive= fDrive( x(0), x(1), pSails );
		Resistance= R( x(0), parser );
		HeelMoment= Mw( f_drive, x(1), pSails);
		rightMoment= Rm( x(1),parser );

		if( fabs(f_drive-Resistance) < 0.001 && fabs(HeelMoment-rightMoment) < 0.001 ){
			cout<<"\n Solution found! \n";
			std::cout<<"x="<<x.transpose()<<
					"; f= "<<f_drive-Resistance<<", g= "<<HeelMoment-rightMoment<<std::endl;
			break;
		}
	}
}

void testFakeVPP(	boost::shared_ptr<SailSet>& pSails,
									VariableFileParser& parser){

	size_t dimension=2;
	// Select a starting point
	Eigen::VectorXd x(dimension);
	x << 1.5, mathUtils::toRad(10);

	solve( pSails, parser, x);

}

// MAIN
int main(int argc, char** argv) {

	try{

		printf("\n=======================\n");
		printf("===  V++ PROGRAM  =====\n");
		printf("=======================\n");

		// instantiate a vectorPlotter and quit
		VectorPlotter testplot;

		// Instantiate a parser with the variables
		VariableFileParser parser("variableFile.txt");

		// Declare a ptr with the sail configuration
		// This is based on the variables that have been read in
		boost::shared_ptr<SailSet> pSails;

		// Declare a container for all the items that
		// constitute the VPP components (Wind, Resistance, RightingMoment...)
		boost::shared_ptr<VPPItemFactory> pVppItems;

		// Load variables and items
		load(parser,pSails,pVppItems);

		// Instantiate a Newton Raphson solver
		NRSolver solver(pVppItems);

		std::cout<<"Please enter a command or type -help-\n";

		string s;
		while(cin >> s){

			// Exit the program on demand
			if(s==string("exit") || s==string("q") )
				break;

			// Parse other options
			if(s == string("printVars") )
				parser.printVariables();

			else if( s == string("plotSailCoeffs"))
				pVppItems->getSailCoefficientItem()->plotInterpolatedCoefficients();

			else if( s == string("plotResidRes"))
				pVppItems->getResiduaryResistanceItem()->plot();

			//---

			else if(s == string("plotResidRes_Keel") )
				pVppItems->getResiduaryResistanceKeelItem()->plot();

			//add plot FrictionalResistanceItem
			else if(s == string("plotViscousRes") )
				pVppItems->getFrictionalResistanceItem()->plot();

			else if(s == string("plotViscousRes_Keel") )
				pVppItems->getViscousResistanceKeelItem()->plot(); //-> this does not plot

			else if(s == string("plotViscousRes_Rudder") )
				pVppItems->getViscousResistanceRudderItem()->plot(); //-> this does not plot

			else if(s == string("plotDelta_FrictRes_Heel") )
				pVppItems->getDelta_FrictionalResistance_HeelItem()->plot();

			else if(s == string("plotDelta_ResidRes_Heel") )
				pVppItems->getDelta_ResiduaryResistance_HeelItem()->plot();

			//---

			else if(s == string("reload") ){
				load(parser,pSails,pVppItems);
				solver.reset(pVppItems);
			}

			else if(s == string("run") )
				run(parser,solver);

			else if(s == string("testNR1") )
				testNR1(); // WARNING, UNDOCUMENTED OPTION!

			else if(s == string("testNR2") )
				testNR2(); // WARNING, UNDOCUMENTED OPTION!

			else if(s == string("testFakeVPP") )
				testFakeVPP(pSails,parser);

			//---

			else if( s == string("print"))
				solver.printResults();

			else if( s == string("plotPolars"))
				solver.plotPolars();

			else if( s == string("plotXY")) {
				std::cout<<"Please enter the index of the wind angle: \n";
				int idx;
				cin >> idx;
				solver.plotXY(idx);
			}

			//---

			else if( s == string("help")){

				std::cout<<"\n== AVAILABLE OPTIONS =============================================== \n";
				std::cout<<"   printVars              : print the variables read from file \n";
				std::cout<<" \n";
				std::cout<<"   plotSailCoeffs         : plot the aerodynamic coeffs for the current sails \n";
				std::cout<<" \n";
				std::cout<<"   plotResidRes           : plot the Residuary Resistance \n";
				std::cout<<"   plotResidRes_Keel      : plot the Residuary Resistance of the Keel \n";
				std::cout<<"   plotViscousRes         : plot the Viscous Resistance \n";
				std::cout<<"   plotViscousRes_Keel    : plot the Viscous Resistance of the Keel \n";
				std::cout<<"   plotViscousRes_Rudder  : plot the Viscous Resistance of the Rudder \n";
				std::cout<<"   plotDelta_FrictRes_Heel: plot the Delta Frictional Resistance due to heel \n";
				std::cout<<"   plotDelta_ResidRes_Heel: plot the Delta Residuary Resistance due to heel \n";
				std::cout<<" \n";
				std::cout<<"   reload                 : reload the variables from file \n";
				std::cout<<"   run                    : launches the computations \n";
				std::cout<<" \n";
				std::cout<<"   print                  : print results to screen \n";
				std::cout<<" \n";
				std::cout<<"   plotPolars             : plot the polar result graphs \n";
				std::cout<<"   plotXY                 : plot the XY velocity-wise result graphs \n";
				std::cout<<" \n";
				std::cout<<"   exit                   : terminates the program \n";
				std::cout<<"======================================================================\n\n";

			}
			else
				std::cout<<"Option not recognized, type -help- "
						"for a list of available options \n";

			std::cout<<"\nPlease enter a command or type -help-\n";
		}

	} catch(std::exception& e) {
		std::cout<<"\n-----------------------------------------"<<std::endl;
		std::cout<<" Exception caught in Main:  "<<std::endl;
		std::cout<<" --> "<<e.what()<<std::endl;
		std::cout<<" The program is terminated. "<<std::endl;
		std::cout<<"-----------------------------------------\n"<<std::endl;
	}	catch(...) {
		cout << "Unknown Exception occurred\n";
	}

	return (0);

}
