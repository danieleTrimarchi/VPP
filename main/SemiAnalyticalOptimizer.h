#ifndef SEMIANALYTICAL_OPTIMIZER_H
#define SEMIANALYTICAL_OPTIMIZER_H

#include "VPPSolverBase.h"
#include "nlopt.hpp"

using namespace std;

namespace SAOA {

/// This class implements the SAOA (Semi-Analytical Optimization Approach)
/// which is used to improve the performance of the underlying optimizer NLOpt.
/// The idea is to use the NRSolver to find a series of solutions in the
/// optimization space (crew, flat) and feed a parabolic regression with these
/// solutions. In the end we will be building a polynomial that approximates the
/// optimization space. Given this polynomial, not only we can feed the optimizer
/// with an analytical formulation, but we can compute derivatives which will ease
/// the convergence and the accuracy of the solution.
class SemiAnalyticalOptimizer : public VPPSolverBase {

	public:

		/// Constructor
		SemiAnalyticalOptimizer(boost::shared_ptr<VPPItemFactory>);

		/// Destructor
		~SemiAnalyticalOptimizer();

		/// Reset the SemiAnalyticalOptimizer when reloading the initial data
		virtual void reset(boost::shared_ptr<VPPItemFactory>);

		/// Execute a VPP-like analysis - implements the pure virtual method
		virtual void run(int TWV, int TWA);

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	private:

		/// Boat velocity objective function
		static double VPP_speed(unsigned n, const double *x, double *grad, void *my_func_data);

		/// Ask the NRSolver to solve a sub-problem without the optimization variables
		/// this makes the initial guess an equilibrated solution
		void solveInitialGuess(int TWV, int TWA);

		// Struct used to drive twv and twa into the update methods of the VPPItems
		typedef struct {
				int twv_, twa_;
		} Loop_data;

		/// Size of the sub-problem to be pre-solved with the NRSolver
		size_t saPbSize_; // --> v, phi

		/// Shared ptr holding the underlying SemiAnalyticalOptimizer
		boost::shared_ptr<nlopt::opt> opt_;

		/// max iters allowed for the SemiAnalyticalOptimizer
		static size_t maxIters_;

};
};// namespace SemiAnalyticalOptimizer

#endif
