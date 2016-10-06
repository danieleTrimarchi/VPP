#ifndef VPP_OPTIMIZER_H
#define VPP_OPTIMIZER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include "nlopt.hpp"
#include "VPPSolverBase.h"

using namespace std;

namespace Optim {

/// Wrapper class around NLOPT non-linear optimization library
class Optimizer : public VPPSolverBase {

	public:

		/// Constructor
		Optimizer(boost::shared_ptr<VPPItemFactory>);

		/// Destructor
		virtual ~Optimizer();

		/// Reset the Optimizer when reloading the initial data
		virtual void reset(boost::shared_ptr<VPPItemFactory>);

		/// Execute a VPP-like analysis. Implements pure virtual defined
		/// in the abstract base class
		virtual void run(int TWV, int TWA);

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	private:

		/// Boat velocity objective function
		static double VPP_speed(unsigned n, const double *x, double *grad, void *my_func_data);

		/// Set the constraint: dF=0 and dM=0
		static void VPPconstraint(unsigned m, double *result, unsigned n, const double* x, double* grad, void* f_data);

		/// Set the initial guess for the state variable vector
		virtual void resetInitialGuess(int TWV, int TWA);

		/// Ask the NRSolver to solve a sub-problem without the optimization variables
		/// this makes the initial guess an equilibrated solution
		virtual void solveInitialGuess(int TWV, int TWA);

		// Struct used to drive twv and twa into the update methods of the VPPItems
		typedef struct {
				int twv_, twa_;
		} Loop_data;

		/// Shared ptr holding the underlying optimizer
		boost::shared_ptr<nlopt::opt> opt_;

		/// max iters allowed for the optimizer
		static size_t maxIters_;

};
};// namespace optimizer

#endif
