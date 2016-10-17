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
#include "VPPCommand.h"
#include "VPPResultIO.h"

using namespace VPPSolve;
using namespace Optim;
using namespace SAOA;

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
				std::cout<<"Something went very wrong while running the solver..."<<std::endl;
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
		Optimizer solver(pVppItems);
		// SemiAnalyticalOptimizer solver(pVppItems);

		// Instantiate a vector of commands that will have the role of undo/redo stack
		std::vector<VPPCommand> vppCommandStack;

		std::cout<<"Please enter a command or type -help-\n";

		string s;
		while(cin >> s){

			// Exit the program on demand
			if(s==string("exit") || s==string("q") || s==string("Q") )
				break;

			// Parse other options
			if(s == string("printVars") )
				vppCommandStack.push_back( PrintVariablesCommand(&parser,pSails.get() ) );

			else if( s == string("plotSailCoeffs"))
				vppCommandStack.push_back( PlotInterpolatedCoefficientsCommand(pVppItems.get()) );

			else if( s == string("plot_D_SailCoeffs"))
				vppCommandStack.push_back( plot_D_InterpolatedCoefficientsCommand(pVppItems.get()) );

			else if( s == string("plot_D2_SailCoeffs"))
				vppCommandStack.push_back( plot_D2_InterpolatedCoefficientsCommand(pVppItems.get()) );


			// ---

			else if( s == string("plotSailForceMoment"))
				vppCommandStack.push_back( plotSailForceMomentCommand(pVppItems.get()) );

			else if( s == string("plotJacobian"))
				vppCommandStack.push_back( PlotJacobianCommand(pVppItems.get(),&solver) );

			//---

			else if(s == string("plotTotalResistance") )
				vppCommandStack.push_back( plotResistanceCommand(
						plotResistanceCommand::totalResistance,
						pVppItems.get()) );

			else if(s == string("plotFrictionalRes") )
				vppCommandStack.push_back( plotResistanceCommand(
						plotResistanceCommand::frictionalResistance,
						pVppItems.get()) );

			else if( s == string("plotResidRes"))
				vppCommandStack.push_back( plotResistanceCommand(
						plotResistanceCommand::residuaryResistance,
						pVppItems.get()) );

			else if(s == string("plotInducedRes") )
				vppCommandStack.push_back( plotResistanceCommand(
						plotResistanceCommand::inducedResistance,
						pVppItems.get()) );

			else if(s == string("plotDelta_FrictRes_Heel") )
				vppCommandStack.push_back( plotResistanceCommand(
						plotResistanceCommand::delta_frictionalResistance_heel,
						pVppItems.get()) );

			else if(s == string("plotDelta_ResidRes_Heel") )
				vppCommandStack.push_back( plotResistanceCommand(
						plotResistanceCommand::delta_residualResistance_heel,
						pVppItems.get()) );

			else if(s == string("plotFrictionalRes_Keel") )
				vppCommandStack.push_back( plotResistanceCommand(
						plotResistanceCommand::frictionalResistance_keel,
						pVppItems.get()) );

			else if(s == string("plotFrictionalRes_Rudder") )
				vppCommandStack.push_back( plotResistanceCommand(
						plotResistanceCommand::frictionalResistance_rudder,
						pVppItems.get()) );

			else if(s == string("plotResidRes_Keel") )
				vppCommandStack.push_back( plotResistanceCommand(
						plotResistanceCommand::residuaryResistance_keel,
						pVppItems.get()) );

			else if(s == string("plotNegativeResistance") )
				vppCommandStack.push_back( plotResistanceCommand(
						plotResistanceCommand::negativeResistance,
						pVppItems.get()) );

			else if(s == string("plotOptimizationSpace") )
				vppCommandStack.push_back( plotOptimizationSpaceCommand(pVppItems.get()) );

			else if( s == string("plotPolars"))
				vppCommandStack.push_back( PlotJacobianCommand(pVppItems.get(),&solver) );

			else if( s == string("plotXY"))
				vppCommandStack.push_back( plotXYCommand(pVppItems.get(),&solver) );

			//--- not undoable commands

			else if(s == string("reload") ){
				load(parser,pSails,pVppItems);
				solver.reset(pVppItems);
			}

			else if(s == string("run") )
				run(parser,&solver);

			else if(s == string("import") )
				solver.importResults();

			//---

			else if( s == string("print"))
				solver.printResults();

			else if( s == string("save"))
				solver.saveResults();

			else if( s == string("bounds"))
				solver.printResultBounds();

			else if ( s == string ("redo")){

				// this is a command to be applied to the previous plot
				// 1_ reject because the previous command (sPrevious) is not a 2d plotting command
				// 2_ ask the user to prompt two doubles xMin and xMax
				// 3_ call the plotting cmd, each with a new signature with bounds xMin, xMax
				*(vppCommandStack.end()).

			}
			//---

			else if( s == string("help") || s == string("h") ){

				std::cout<<"\n== AVAILABLE OPTIONS =============================================== \n";
				std::cout<<"   printVars                : print the variables read from file \n";
				std::cout<<" \n";
				std::cout<<"   plotSailCoeffs           : plot the aerodynamic coeffs for the current sails \n";
				std::cout<<"   plot_D_SailCoeffs        : plot the first derivative of the aerodynamic coeffs for the current sails \n";
				std::cout<<"   plot_D2_SailCoeffs       : plot the second derivative of the aerodynamic coeffs for the current sails \n";
				std::cout<<" \n";
				std::cout<<"   plotSailForceMoment      : plot the drive force and the heeling moment for fixed wind/heel ranges\n";
				std::cout<<"   plotJacobian             : plot the Jacobian derivative components for fixed wind/heel ranges\n";
				std::cout<<" \n";
				std::cout<<"   plotTotalResistance      : plot the Total Resistance for a fixed range\n";
				std::cout<<"   plotResidRes             : plot the Residuary Resistance for a fixed range\n";
				std::cout<<"   plotInducedRes           : plot the Induced Resistance for a fixed range\n";
				std::cout<<"   plotResidRes_Keel        : plot the Residuary Resistance of the Keel for a fixed range\n";
				std::cout<<"   plotFrictionalRes        : plot the Viscous Resistance for a fixed range\n";
				std::cout<<"   plotFrictionalRes_Keel   : plot the Viscous Resistance of the Keel for a fixed range\n";
				std::cout<<"   plotFrictionalRes_Rudder : plot the Viscous Resistance of the Rudder for a fixed range\n";
				std::cout<<"   plotDelta_FrictRes_Heel  : plot the Delta Frictional Resistance due to heel for a fixed range\n";
				std::cout<<"   plotDelta_ResidRes_Heel  : plot the Delta Residuary Resistance due to heel for a fixed range\n";
				std::cout<<"   plotNegativeResistance   : plot the Negative Resistance for a fixed Fn range\n";
				std::cout<<" \n";
				std::cout<<"   replot                   : repeat the previous plot but redefine the bounds of the plot\n";
				std::cout<<" \n";
				std::cout<<"   plotOptimizationSpace    : plot the optimization 2d space starting at a given configuration\n";
				std::cout<<" \n";
				std::cout<<"   reload                   : reload the variables from file \n";
				std::cout<<"   run                      : launches the computations \n";
				std::cout<<"   import                   : import the results from a file named results.vpp \n";
				std::cout<<" \n";
				std::cout<<"   print                    : print results to screen \n";
				std::cout<<"   save                     : save results to a file named results.vpp \n";
				std::cout<<"   bounds                   : print result bounds to screen \n";
				std::cout<<" \n";
				std::cout<<"   plotPolars               : plot the polar result graphs \n";
				std::cout<<"   plotXY                   : plot the XY velocity-wise result graphs \n";
				std::cout<<" \n";
				std::cout<<"   exit / q                 : terminates the program \n";
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
