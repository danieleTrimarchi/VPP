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
#include "VPPItem.h"
#include "Optimizer.h"
#include "Interpolator.h"

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
		boost::shared_ptr<SailSet> pSails( SailSet::SailSetFactory(parser) );
		pSails->printVariables();

		// Instantiate a container for all the quantities function of the state variables
		boost::shared_ptr<VPPItemFactory> pVppItems( new VPPItemFactory(&parser,pSails) );

//		// Instantiate an optimizer
//		Optimizer optimizer(pVppItems);
//
//		// Loop on the wind VELOCITIES and ANGLES
//		//for(size_t vTW=0; vTW<parser.get("N_TWV"); vTW++)
//		//	for(size_t aTW=0; aTW<parser.get("N_ALPHA_TW"); aTW++){
//		for(size_t vTW=0; vTW<2; vTW++)
//			for(size_t aTW=0; aTW<2; aTW++){
//
//				// Run the optimizer for the current wind speed/angle
//				optimizer.test(vTW,aTW);
//
//			}

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
