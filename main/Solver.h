#ifndef SOLVER_H
#define SOLVER_H

#include <stdio.h>
#include <iostream>
#include <fstream>

#include <umfpack.h>

using namespace std;

/// Wrapper class around UMFPACK
class Solver {

	public:

		/// Constructor
		Solver();

		/// Destructor
		~Solver();

		/// Execute UMFPACK example 1
		void run();


};

#endif
