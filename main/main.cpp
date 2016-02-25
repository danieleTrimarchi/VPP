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

		// Get the variables
		VariableFileParser parser("variableFile.txt");
		parser.parse();
		parser.check();
		parser.printVariables();

		// Compute the sail configuration based on the variables that have been read in
		boost::shared_ptr<SailSet> pSails( SailSet::SailSetFactory(parser) );
		pSails->printVariables();

		// Instantiate a container for all the quantities function of the state variables
		boost::shared_ptr<VPPItemFactory> pVppItems( new VPPItemFactory(&parser,pSails) );

		// Instantiate an optimizer
		Optimizer optimizer(pVppItems);

		string s;
		while(cin >> s){

			// Exit the program on demand
			if(s==string("exit"))
				break;

			// Parse other options
			if(s == string("run") )
				run(parser,optimizer);

			else if( s == string("print"))
				optimizer.printResults();

			else if( s == string("plot"))
				optimizer.plotResults();

			else
				std::cout<<"Option not recognized\n";

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
