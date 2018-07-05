#ifndef SEMIANALYTICAL_OPTIMIZER_H
#define SEMIANALYTICAL_OPTIMIZER_H

#include "VPPSolverBase.h"
#include "nlopt.hpp"

using namespace std;

namespace Optim {

// forward declaration
class SAOASolverFactory;

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

		/// Destructor
		~SemiAnalyticalOptimizer();

		/// Reset the SemiAnalyticalOptimizer when reloading the initial data
		virtual void reset(std::shared_ptr<VPPItemFactory>);

		/// Execute a VPP-like analysis - implements the pure virtual method
		virtual void run(int TWV, int TWA);

	private:

		/// This class is to be instantiated using a NLOptSolverFactory.
		/// The friendship allows the factory to call the private constructor
		friend class SAOASolverFactory;

		/// Private constructor - the class can only be instantiated using
		/// a VPPSolverFactory
		SemiAnalyticalOptimizer(std::shared_ptr<VPPItemFactory>);

		/// Boat velocity objective function
		static double VPP_speed(unsigned n, const double *x, double *grad, void *my_func_data);

		// Struct used to drive twv and twa into the update methods of the VPPItems
		typedef struct {
				int twv_, twa_;
		} Loop_data;

		/// Size of the sub-problem to be pre-solved with the NRSolver
		size_t saPbSize_; // --> v, phi

		/// Shared ptr holding the underlying SemiAnalyticalOptimizer
		std::shared_ptr<nlopt::opt> opt_;

		/// max iters allowed for the SemiAnalyticalOptimizer
		static size_t maxIters_;

		static int optIterations_;

};
};// namespace SemiAnalyticalOptimizer

#endif
