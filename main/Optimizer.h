#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include <nlopt.hpp>

#include "VPPItemFactory.h"

using namespace std;

/// Wrapper class around NLOPT non-linear
/// optimization library
class Optimizer {

	public:

		/// Constructor
		Optimizer(boost::shared_ptr<VPPItemFactory>);

		/// Destructor
		~Optimizer();

		/// Execute a VPP-like analysis
		void run(int TWV, int TWA);

	private:

		/// Size of the problem this Optimizer is handling
		size_t dimension_; // --> v, phi, reef, flat

		/// Ptr to the VPPItemFactory that contains all of the ingredients
		/// required to compute the optimization constraints
		static boost::shared_ptr<VPPItemFactory> vppItemsContainer_;

		/// Ptr to the variableFileParser
		VariableFileParser* pParser_;

		// Struct used to drive twv and twa into the update methods of the VPPItems
		typedef struct {
				double twv, twa;
		} Loop_data;

		/// Boat velocity objective function
		static double VPP_speed(unsigned n, const double *x, double *grad, void *my_func_data);

		/// Set the constraint: dF=0 and dM=0
		static void VPPconstraint(unsigned m, double *result, unsigned n, const double* x, double* grad, void* f_data);

};

#endif
