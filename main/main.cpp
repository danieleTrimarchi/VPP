#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

// ------------------------
// Directives for EIGEN
#include <Eigen/Core>
using namespace Eigen;

#include "boost/shared_ptr.hpp"

#include "VariableFileParser.h"
#include "SailSet.h"

// MAIN
int main(int argc, const char *argv[]) {

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
		boost::shared_ptr<SailSet> pSails( SailSet::SailSetFactory(parser ) );
		pSails->printVariables();

		// Instantiate a container for all the quantities used in the equations
		// to be solved. This is an ITEM, all the quantities derive from. It has
		// a pure virtual update(vTW,aTW) that recursively updates all the children
		// for the current location in the polar plot

		// Loop on the wind VELOCITIES and ANGLES
		for(size_t vTW=0; vTW<parser.get("N_TWV"); vTW++)
			for(size_t aTW=0; aTW<parser.get("N_ALPHA_TW"); aTW++){

				// instantiate a container for the physical quantities
				// able to be passed to the optimizer that will compute the
				// max speed.
				// this contains
				// - access to variables
				// - wind
				// - sailSet (sail geometry, coefficients and interpolator)
				// - forces: aero and Resistance (-> hence all the resistance components)
				// - Moments : heeling and righting moments

				// this must take the
				//Optimizer optimizer;
				//optimizer.run();

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
