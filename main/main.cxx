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
#include "Regression.h"
#include "Interpolator.h"
#include "VPPException.h"
#include "mathUtils.h"
#include "Version.h"
#include "VPPResultIO.h"
#include "VPPSolverFactoryBase.h"

#include <QApplication>
#include "MainWindow.h"

// MAIN
int main(int argc, char** argv) {

	// Instantiate the utilities to get the external resources such
	// as the icons
	Q_INIT_RESOURCE(VPP);

	try {

		// Instantiate a Qt application that will be executed at the end of main
		QApplication app(argc, argv);

		// Instantiate the MainWindow of the VPP application
		MainWindow mainWin;
		mainWin.resize(800, 600);
		mainWin.show();

		// Execute the application
		return app.exec();

	} catch(std::exception& e) {
		std::cout<<"\n-----------------------------------------"<<std::endl;
		std::cout<<" Exception caught in Main:  "<<std::endl;
		std::cout<<" --> "<<e.what()<<std::endl;
		std::cout<<" The program is terminated. "<<std::endl;
		std::cout<<"-----------------------------------------\n"<<std::endl;
	}	catch(...) {
		cout << "Unknown Exception occurred\n";
	}

}

// ALLOW FOR I/O of QUANTITIES IN THE PREFERRED UNIT...
// todo dtrimarchi
//			else if(s == string("convertVelocityToFn")) {
//				IOUtils io(pVppItems->getWind());
//				std::cout<<"Fn= "<<
//						pVppItems->getFrictionalResistanceItem()->convertToFn(
//								io.askUserDouble("Please enter the value of the velocity...")
//						)<<std::endl;
//			}
//
// ALLOW FOR I/O of QUANTITIES IN THE PREFERRED UNIT...
// todo dtrimarchi
//			else if(s == string("convertFnToVelocity")) {
//				IOUtils io(pVppItems->getWind());
//				std::cout<<"Velocity= "<<
//						pVppItems->getFrictionalResistanceItem()->convertToVelocity(
//								io.askUserDouble("Please enter the value of the Fn...")
//						)<<std::endl;
//			}
//
// --
//
//			else if( s == string("plotPolars"))
//				solverFactory.get()->plotPolars();
//
//			else if( s == string("plotXY")) {
//				std::cout<<"Please enter the index of the wind angle: \n";
//				int idx;
//				cin >> idx;
//				solverFactory.get()->plotXY(idx);
//			}
//
// --
//
//			else if(s == string("reload") ){
//				load(parser,pSails,pVppItems);
//				solverFactory.get()->reset(pVppItems);
//			}
//
//			else if(s == string("import") )
//				solverFactory.get()->importResults();
//
//			//---
//
// 			This is substituted by the spreadsheet view
//			else if( s == string("print"))
//				solverFactory.get()->printResults();
//
//			else if( s == string("bounds"))
//				solverFactory.get()->printResultBounds();
//
//			else if (s == "buildInfo" ){
//				std::cout<<"-------------------------"<<std::endl;
//				std::cout<<" Branch: "<<currentBranch<<std::endl;
//				std::cout<<" Revision number: "<<currentRevNumber<<std::endl;
//				std::cout<<" Commit hash: "<<currentHash<<std::endl;
//				std::cout<<" Build on: "<<buildDate<<std::endl;
//				std::cout<<"-------------------------"<<std::endl;
//			}
//
//			//---
//
//			else if( s == string("help") || s == string("h") ){
//
//				std::cout<<"\n== AVAILABLE OPTIONS ================================================== \n";
//				std::cout<<"   printVars                   : print the variables read from file \n";
//				std::cout<<" \n";
//				std::cout<<"   convertVelocityToFn         : print out the Fn, given a velocity: Fn = v/sqrt(gL)\n";
//				std::cout<<"   convertFnToVelocity         : print out the velocity, given a Fn: v = Fn*sqrt(gL) \n";
//				std::cout<<" \n";
//				std::cout<<"   plotSailCoeffs              : plot the aerodynamic coeffs for the current sails \n";
//				std::cout<<"   plot_D_SailCoeffs           : plot the first derivative of the aerodynamic coeffs for the current sails \n";
//				std::cout<<"   plot_D2_SailCoeffs          : plot the second derivative of the aerodynamic coeffs for the current sails \n";
//				std::cout<<" \n";
//				std::cout<<"   plotSailForceMoment         : plot the drive force and the heeling moment for fixed wind/heel ranges\n";
//				std::cout<<"   plotJacobian                : plot the Jacobian derivative components for fixed wind/heel ranges\n";
//				std::cout<<"   plotGradient                : plot the Gradient derivative components for fixed wind/heel ranges\n";
//				std::cout<<" \n";
//				std::cout<<"   plot_deltaWettedArea_heel   : plot the change in wetted area due to the heel (DSYHS99 p116) \n";
//				std::cout<<" \n";
//				std::cout<<"   plotTotalResistance         : plot the Total Resistance for a fixed range\n";
//				std::cout<<"   plotResidRes                : plot the Residuary Resistance for a fixed range\n";
//				std::cout<<"   plotInducedRes              : plot the Induced Resistance for a fixed range\n";
//				std::cout<<"   plotResidRes_Keel           : plot the Residuary Resistance of the Keel for a fixed range\n";
//				std::cout<<"   plotFrictionalRes           : plot the Viscous Resistance for a fixed range\n";
//				std::cout<<"   plotFrictionalRes_Keel      : plot the Viscous Resistance of the Keel for a fixed range\n";
//				std::cout<<"   plotFrictionalRes_Rudder    : plot the Viscous Resistance of the Rudder for a fixed range\n";
//				std::cout<<"   plotDelta_FrictRes_Heel     : plot the Delta Frictional Resistance due to heel for a fixed range\n";
//				std::cout<<"   plotDelta_ResidRes_Heel     : plot the Delta Residuary Resistance due to heel for a fixed range\n";
//				std::cout<<"   plotDelta_ResidResKeel_Heel : plot the Delta Residuary Resistance of the keel due to heel for a fixed range\n";
//				std::cout<<" \n";
//				std::cout<<"   plotNegativeResistance      : plot the Negative Resistance for a fixed Fn range\n";
//				std::cout<<" \n";
//				std::cout<<"   plotOptimizationSpace       : plot the optimization 2d space starting at a given configuration\n";
//				std::cout<<" \n";
//				std::cout<<"   reload                      : reload the variables from file \n";
//				std::cout<<"   run                         : launches the computations \n";
//				std::cout<<"   import                      : import the results from a file named results.vpp \n";
//				std::cout<<" \n";
//				std::cout<<"   print                       : print results to screen \n";
//				std::cout<<"   save                        : save results to a file named results.vpp \n";
//				std::cout<<"   bounds                      : print result bounds to screen \n";
//				std::cout<<" \n";
//				std::cout<<"   plotPolars                  : plot the polar result graphs \n";
//				std::cout<<"   plotXY                      : plot the XY velocity-wise result graphs \n";
//				std::cout<<" \n";
//				std::cout<<"   buildInfo                   : plot build info such as the date, the branch and the commit\n";
//				std::cout<<"   exit / q                    : terminates the program \n";
//				std::cout<<"=========================================================================\n\n";
//
//			}
//			else
//				std::cout<<"Option not recognized, type -help- "
//				"for a list of available options \n";
//
//			std::cout<<"\nPlease enter a command or type -help-\n";
//		}
//
