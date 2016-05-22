#ifndef SPARSESOLVER_H
#define SPARSESOLVER_H

#include <stdio.h>
#include <iostream>
#include <fstream>

#include <umfpack.h>

using namespace std;

/// Wrapper class around UMFPACK
class SparseSolver {

	public:

		/// Constructor
		SparseSolver();

		/// Destructor
		~SparseSolver();

		/// Execute UMFPACK example 1
		void run();

};

#endif
