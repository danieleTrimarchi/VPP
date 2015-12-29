#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

// ------------------------
// Directives for EIGEN
#include <Eigen/Core>
using namespace Eigen;

// ------------------------
// Solver utility
#include "Solver.h"

// Optimizer utility
#include "Optimizer.h"

// ------------------------
// Directives for BOOST
#include "boost/shared_ptr.hpp"

// ------------------------
// Plotting utility
#include "Plotter.h"

#include "VariableFileParser.h"

// MAIN
int main(int argc, const char *argv[]) {

	try{

		// TRIVIAL TEST
		printf("\nVPP PROGRAM\n");

		printf("\n-- EIGEN TEST -----------\n");
		//    int rows=5, cols=5;
		//    MatrixXf m(rows,cols);
		//    m << (Matrix3f() << 1, 2, 3, 4, 5, 6, 7, 8, 9).finished(),
		//    MatrixXf::Zero(3.,cols-3),
		//    MatrixXf::Zero(rows-3,3),
		//    MatrixXf::Identity(rows-3,cols-3);
		//    cout << m << endl;
		printf("-------------\n");


		printf("\n-- UMFPACK TESTS -----------\n");
		//    Solver solver;
		//    solver.run();
		printf("-------------\n");


		printf("\n-- BOOST TESTS -----------\n");
		boost::shared_ptr<int> p(new int);


		printf("\n-- PLOTTER TESTS -----------\n");
//		// Instantiate a plotter
//		Plotter plotter;
//
//		// plot a XY test plot
//		plotter.plot();
//
//		PolarPlotter polarPlotter;
//
//		// plot a polar test plot
//		polarPlotter.plot();

		//    printf("\n-- NLOPT TESTS -----------\n");
		//    Optimizer optimizer;
		//    optimizer.run();

		printf("\n-- VariableFileParser TESTS -----------\n");

		VariableFileParser variableFileParser("variableFile.txt");
		variableFileParser.parse();




	} catch(...) {
		cout << "Exception occurred\n";
	}

	return (0);

}
