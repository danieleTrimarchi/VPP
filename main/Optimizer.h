#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include "nlopt.hpp"

#include "VPPItemFactory.h"
#include "Results.h"
#include "Plotter.h"

using namespace std;

/// Wrapper class around NLOPT non-linear
/// optimization library
class Optimizer {

	public:

		/// Constructor
		Optimizer(boost::shared_ptr<VPPItemFactory>);

		/// Destructor
		~Optimizer();

		/// Reset the optimizer when reloading the initial data
		void reset(boost::shared_ptr<VPPItemFactory>);

		/// Execute a VPP-like analysis. Returns 0 on success,
		/// -1 if the residuals are found to exceed a tolerance
		void run(int TWV, int TWA);

		/// Make a printout of the results for this run
		void printResults();

		/// Plot the polar plots for the state variables
		void plotPolars();

		/// Plot the XY plots for the state variables
		void plotXY( size_t index );

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	private:

		/// Boat velocity objective function
		static double VPP_speed(unsigned n, const double *x, double *grad, void *my_func_data);

		/// Set the constraint: dF=0 and dM=0
		static void VPPconstraint(unsigned m, double *result, unsigned n, const double* x, double* grad, void* f_data);

		/// Set the initial guess for the state variable vector
		void resetInitialGuess(int TWV, int TWA);

		// Struct used to drive twv and twa into the update methods of the VPPItems
		typedef struct {
				int twv_, twa_;
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

		/// Vector with the initial guess/optimizer results
		Eigen::VectorXd xp_;

		/// Matrix of results, one result per wind velocity/angle
		boost::shared_ptr<ResultContainer> pResults_;

		/// Ptr to the wind item, used to retrieve the current twv, twa
		WindItem* pWind_;

		/// tolerance
		double tol_;

		/// max iters allowed for the optimizer
		static size_t maxIters_;

};

#endif
