#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include <nlopt.hpp>

#include "VPPItemFactory.h"

using namespace std;

/// Struct containing the results of the current
/// run. For each step (i.e. twv,twa it contains
/// the value of the state vector
class OptResult {

	public:

		/// Constructor
		OptResult(int twv, int twa, std::vector<double>& res);

		/// Destructor
		~OptResult();

	private:

		/// Value of wind TRUE velocity/angle
		double twv_,twa_;

		/// State vector
		std::vector<double> result_;

} ;

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

		/// Boat velocity objective function
		static double VPP_speed(unsigned n, const double *x, double *grad, void *my_func_data);

		/// Set the constraint: dF=0 and dM=0
		static void VPPconstraint(unsigned m, double *result, unsigned n, const double* x, double* grad, void* f_data);

		// Struct used to drive twv and twa into the update methods of the VPPItems
		typedef struct {
				double twv, twa;
		} Loop_data;

		/// Size of the problem this Optimizer is handling
		size_t dimension_; // --> v, phi, reef, flat

		/// Shared ptr holding the underlying optimizer
		boost::shared_ptr<nlopt::opt> opt_;

		/// lower and upper bounds for the state variables
	  std::vector<double> lowerBounds_,upperBounds_;

		/// Ptr to the VPPItemFactory that contains all of the ingredients
		/// required to compute the optimization constraints
		static boost::shared_ptr<VPPItemFactory> vppItemsContainer_;

		/// Ptr to the variableFileParser
		VariableFileParser* pParser_;

		/// Vector of results, one result per Optimizer run
		std::vector<OptResult> results_;

};

#endif
