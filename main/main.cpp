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
#include "VPPSolver.h"
#include "Optimizer.h"
#include "SemiAnalyticalOptimizer.h"
#include "NRSolver.h"
#include "Regression.h"
#include "Interpolator.h"
#include "VPPException.h"
#include "mathUtils.h"
#include "Version.h"
#include "VPPResultIO.h"

#include "IpIpoptApplication.hpp"
#include "VPP_nlp.h"

using namespace VPPSolve;
using namespace Optim;
using namespace SAOA;
using namespace Ipopt;


/// Reload the variable file and update the items accordingly
void load(VariableFileParser& parser,
		boost::shared_ptr<SailSet>& pSailSet,
		boost::shared_ptr<VPPItemFactory>& pVppItems){

	// Parse the variables file
	parser.parse();

	// Instantiate the sailset
	pSailSet.reset( SailSet::SailSetFactory(parser) );

	// Instantiate the items
	pVppItems.reset( new VPPItemFactory(&parser,pSailSet) );

}

/// Run the solver/Optimizer
void run(VariableFileParser& parser, VPPSolverBase* pSolver){

	// Loop on the wind ANGLES and VELOCITIES
	for(size_t aTW=0; aTW<parser.get("N_ALPHA_TW"); aTW++)
		for(size_t vTW=0; vTW<parser.get("N_TWV"); vTW++){

			std::cout<<"vTW="<<vTW<<"  "<<"aTW="<<aTW<<std::endl;

			try{
				// Run the optimizer for the current wind speed/angle
				pSolver->run(vTW,aTW);
			}
			catch(...){
				//do nothing and keep going
			}

		}
}

/// Run the solver/Optimizer
void run(VariableFileParser& parser, SmartPtr<IpoptApplication>& pApp, SmartPtr<VPP_NLP>& pMyNlp ){

	// Loop on the wind ANGLES and VELOCITIES
	for(size_t aTW=0; aTW<parser.get("N_ALPHA_TW"); aTW++)
		for(size_t vTW=0; vTW<parser.get("N_TWV"); vTW++){

			std::cout<<"vTW="<<vTW<<"  "<<"aTW="<<aTW<<std::endl;

			try{

				// Set the wind indexes
				pMyNlp->setWind(vTW,aTW);

				// Run the optimizer for the current wind speed/angle
				ApplicationReturnStatus status = pApp->OptimizeTNLP(pMyNlp);

			  if (status == Solve_Succeeded) {
			    std::cout << std::endl << std::endl << "*** The problem solved!" << std::endl;
			  }
			  else
			    throw VPPException(HERE,"ipOpt failed to find the solution!");

			}
			catch(...){
				//do nothing and keep going
			}

		}
}

// MAIN
int main(int argc, char** argv) {

	try{

		printf("\n=======================\n");
		printf("===  V++ PROGRAM  =====\n");
		printf("=======================\n");

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

		// Instantiate a solver. This can be an optimizer (with opt vars)
		// or a simple solver that will keep fixed the values of the optimization
		// vars
		//VPPSolver solver(pVppItems);
		//Optimizer solver(pVppItems);
		// SemiAnalyticalOptimizer solver(pVppItems);
		// ---
		SmartPtr<VPP_NLP> mynlp = new VPP_NLP(pVppItems);
	  SmartPtr<IpoptApplication> app = IpoptApplicationFactory();
	  app->RethrowNonIpoptException(true);
	  app->Options()->SetNumericValue("tol", 1e-7);
	  app->Options()->SetStringValue("mu_strategy", "adaptive");
	  app->Options()->SetStringValue("output_file", "ipopt.out");
	  app->Options()->SetStringValue("hessian_approximation", "limited-memory");
//	  // Leave ipOpt silent
//	  app->Options()->SetNumericValue("print_level",0);
//	  app->Options()->SetNumericValue("file_print_level", 12);
	  ApplicationReturnStatus status;
	  status = app->Initialize();
	  if (status != Solve_Succeeded)
	  	throw VPPException(HERE,"Error during initialization of ipOpt!");

		// ---

		std::cout<<"Please enter a command or type -help-\n";

		string s;
		while(cin >> s){

			// Exit the program on demand
			if(s==string("exit") || s==string("q") || s==string("Q") )
				break;

			// Parse other options
			if(s == string("printVars") ){
				parser.printVariables();
				pSails->printVariables();
			}

			//---

			else if(s == string("convertVelocityToFn")) {
				IOUtils io(pVppItems->getWind());
				std::cout<<"Fn= "<<
				pVppItems->getFrictionalResistanceItem()->convertToFn(
						io.askUserDouble("Please enter the value of the velocity...")
				)<<std::endl;
			}

			else if(s == string("convertFnToVelocity")) {
				IOUtils io(pVppItems->getWind());
				std::cout<<"Velocity= "<<
				pVppItems->getFrictionalResistanceItem()->convertToVelocity(
						io.askUserDouble("Please enter the value of the Fn...")
				)<<std::endl;
			}

			//---

			else if( s == string("plotSailCoeffs"))
				pVppItems->getSailCoefficientItem()->plotInterpolatedCoefficients();

			else if( s == string("plot_D_SailCoeffs"))
				pVppItems->getSailCoefficientItem()->plot_D_InterpolatedCoefficients();

			else if( s == string("plot_D2_SailCoeffs"))
				pVppItems->getSailCoefficientItem()->plot_D2_InterpolatedCoefficients();

			// ---

			else if( s == string("plotSailForceMoment"))
				pVppItems->getAeroForcesItem()->plot();

			else if( s == string("plotJacobian"))
				throw VPPException(HERE,"not implemented");
				// solver.plotJacobian();

			//---

			else if(s == string("plotTotalResistance") )
				pVppItems->plotTotalResistance();

			else if(s == string("plotFrictionalRes") )
				pVppItems->getFrictionalResistanceItem()->plot();

			else if( s == string("plotResidRes"))
				pVppItems->getResiduaryResistanceItem()->plot(pVppItems->getWind());

			else if(s == string("plotInducedRes") )
				pVppItems->getInducedResistanceItem()->plot();

			else if(s == string("plotDelta_FrictRes_Heel") )
				pVppItems->getDelta_FrictionalResistance_HeelItem()->plot(pVppItems->getWind());

			else if(s == string("plotDelta_ResidRes_Heel") )
				pVppItems->getDelta_ResiduaryResistance_HeelItem()->plot(pVppItems->getWind());

			else if(s == string("plotDelta_ResidResKeel_Heel") )
				pVppItems->getDelta_ResiduaryResistanceKeel_HeelItem()->plot(pVppItems->getWind());

			else if(s == string("plotFrictionalRes_Keel") )
				pVppItems->getViscousResistanceKeelItem()->plot(); //-> this does not plot

			else if(s == string("plotFrictionalRes_Rudder") )
				pVppItems->getViscousResistanceRudderItem()->plot(); //-> this does not plot

			else if(s == string("plotResidRes_Keel") )
				pVppItems->getResiduaryResistanceKeelItem()->plot();

			else if(s == string("plotNegativeResistance") )
				pVppItems->getNegativeResistanceItem()->plot();

			else if(s == string("plotOptimizationSpace") )
				pVppItems->plotOptimizationSpace();

			//---

			else if( s == string("plotPolars"))
				throw VPPException(HERE,"not implemented");
			//	solver.plotPolars();

			else if( s == string("plotXY")) {
				throw VPPException(HERE,"not implemented");
				std::cout<<"Please enter the index of the wind angle: \n";
				int idx;
				cin >> idx;
				//solver.plotXY(idx);
			}

			//---

			else if(s == string("reload") ){
				throw VPPException(HERE,"not implemented");
				load(parser,pSails,pVppItems);
				//solver.reset(pVppItems);
			}

			else if(s == string("run") )
				//run(parser,&solver);
				run(parser,app,mynlp);

			else if(s == string("import") )
				throw VPPException(HERE,"not implemented");
				//solver.importResults();

			//---

			else if( s == string("print"))
				//solver.printResults();
				mynlp->printResults();

			else if( s == string("save"))
				throw VPPException(HERE,"not implemented");
			 //solver.saveResults();

			else if( s == string("bounds"))
				throw VPPException(HERE,"not implemented");
				//solver.printResultBounds();

			else if (s == "buildInfo" ){
				std::cout<<"-------------------------"<<std::endl;
				std::cout<<" Branch: "<<currentBranch<<std::endl;
				std::cout<<" Revision number: "<<currentRevNumber<<std::endl;
				std::cout<<" Commit hash: "<<currentHash<<std::endl;
				std::cout<<" Build on: "<<buildDate<<std::endl;
				std::cout<<"-------------------------"<<std::endl;
			}

			//---

			else if( s == string("help") || s == string("h") ){

				std::cout<<"\n== AVAILABLE OPTIONS ================================================== \n";
				std::cout<<"   printVars                   : print the variables read from file \n";
				std::cout<<" \n";
				std::cout<<"   convertVelocityToFn         : print out the Fn, given a velocity: Fn = v/sqrt(gL)\n";
				std::cout<<"   convertFnToVelocity         : print out the velocity, given a Fn: v = Fn*sqrt(gL) \n";
				std::cout<<" \n";
				std::cout<<"   plotSailCoeffs              : plot the aerodynamic coeffs for the current sails \n";
				std::cout<<"   plot_D_SailCoeffs           : plot the first derivative of the aerodynamic coeffs for the current sails \n";
				std::cout<<"   plot_D2_SailCoeffs          : plot the second derivative of the aerodynamic coeffs for the current sails \n";
				std::cout<<" \n";
				std::cout<<"   plotSailForceMoment         : plot the drive force and the heeling moment for fixed wind/heel ranges\n";
				std::cout<<"   plotJacobian                : plot the Jacobian derivative components for fixed wind/heel ranges\n";
				std::cout<<" \n";
				std::cout<<"   plotTotalResistance         : plot the Total Resistance for a fixed range\n";
				std::cout<<"   plotResidRes                : plot the Residuary Resistance for a fixed range\n";
				std::cout<<"   plotInducedRes              : plot the Induced Resistance for a fixed range\n";
				std::cout<<"   plotResidRes_Keel           : plot the Residuary Resistance of the Keel for a fixed range\n";
				std::cout<<"   plotFrictionalRes           : plot the Viscous Resistance for a fixed range\n";
				std::cout<<"   plotFrictionalRes_Keel      : plot the Viscous Resistance of the Keel for a fixed range\n";
				std::cout<<"   plotFrictionalRes_Rudder    : plot the Viscous Resistance of the Rudder for a fixed range\n";
				std::cout<<"   plotDelta_FrictRes_Heel     : plot the Delta Frictional Resistance due to heel for a fixed range\n";
				std::cout<<"   plotDelta_ResidRes_Heel     : plot the Delta Residuary Resistance due to heel for a fixed range\n";
				std::cout<<"   plotDelta_ResidResKeel_Heel : plot the Delta Residuary Resistance of the keel due to heel for a fixed range\n";
				std::cout<<" \n";
				std::cout<<"   plotNegativeResistance      : plot the Negative Resistance for a fixed Fn range\n";
				std::cout<<" \n";
				std::cout<<"   plotOptimizationSpace       : plot the optimization 2d space starting at a given configuration\n";
				std::cout<<" \n";
				std::cout<<"   reload                      : reload the variables from file \n";
				std::cout<<"   run                         : launches the computations \n";
				std::cout<<"   import                      : import the results from a file named results.vpp \n";
				std::cout<<" \n";
				std::cout<<"   print                       : print results to screen \n";
				std::cout<<"   save                        : save results to a file named results.vpp \n";
				std::cout<<"   bounds                      : print result bounds to screen \n";
				std::cout<<" \n";
				std::cout<<"   plotPolars                  : plot the polar result graphs \n";
				std::cout<<"   plotXY                      : plot the XY velocity-wise result graphs \n";
				std::cout<<" \n";
				std::cout<<"   buildInfo                   : plot build info such as the date, the branch and the commit\n";
				std::cout<<"   exit / q                    : terminates the program \n";
				std::cout<<"=========================================================================\n\n";

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
