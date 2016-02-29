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

		// Instantiate an optimizer
		Optimizer optimizer(pVppItems);

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

			else if(s == string("plotViscousRes_Keel") )
				pVppItems->getViscousResistanceKeelItem()->plot(); //-> this does not plot

			else if(s == string("plotViscousRes_Rudder") )
				pVppItems->getViscousResistanceRudderItem()->plot(); //-> this does not plot

			else if(s == string("plotDelta_FrictRes_Heel") )
				pVppItems->getDelta_FrictionalResistance_HeelItem()->plot();

			else if(s == string("plotDelta_ResidRes_Heel") )
				pVppItems->getDelta_ResiduaryResistance_HeelItem()->plot();

			//---

			else if(s == string("reload") )
				load(parser,pSails,pVppItems);

			else if(s == string("run") )
				run(parser,optimizer);

			//---

			else if( s == string("print"))
				optimizer.printResults();

			else if( s == string("plotPolars"))
				optimizer.plotPolars();

			else if( s == string("plotXY"))
				optimizer.plotXY();

			//---

			else if( s == string("help")){

				std::cout<<"\n== AVAILABLE OPTIONS =============================================== \n";
				std::cout<<"   printVars              : print the variables read from file \n";
				std::cout<<" \n";
				std::cout<<"   plotSailCoeffs         : plot the aerodynamic coeffs for the current sails \n";
				std::cout<<" \n";
				std::cout<<"   plotResidRes           : plot the Residuary Resistance \n";
				std::cout<<"   plotResidRes_Keel      : plot the Residuary Resistance of the Keel \n";
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
