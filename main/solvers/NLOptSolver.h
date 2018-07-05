#ifndef NLOPTSOLVER_H
#define NLOPTSOLVER_H

#include "nlopt.hpp"
#include "VPPSolverBase.h"

using namespace std;

namespace Optim {

// Forward declaration
class NLOptSolverFactory;

/// Wrapper class around NLOPT non-linear optimization library
class NLOptSolver : public VPPSolverBase {

	public:

		/// Destructor
		virtual ~NLOptSolver();

		/// Reset the Optimizer when reloading the initial data
		virtual void reset(std::shared_ptr<VPPItemFactory>);

		/// Execute a VPP-like analysis. Implements pure virtual defined
		/// in the abstract base class
		virtual void run(int TWV, int TWA);

	private:

		/// This class is to be instantiated using a NLOptSolverFactory.
		/// The friendship allows the factory to call the private constructor
		friend class NLOptSolverFactory;

		/// Private constructor - the class can only be instantiated using
		/// a VPPSolverFactory
		NLOptSolver(std::shared_ptr<VPPItemFactory>);

		/// Boat velocity objective function
		static double VPP_speed(unsigned n, const double *x, double *grad, void *my_func_data);

		/// Set the constraint: dF=0 and dM=0
		static void VPPconstraint(unsigned m, double *result, unsigned n, const double* x, double* grad, void* f_data);

		// Struct used to drive twv and twa into the update methods of the VPPItems
		typedef struct {
				int twv_, twa_;
		} Loop_data;

		/// Shared ptr holding the underlying optimizer
		std::shared_ptr<nlopt::opt> opt_;

		/// max iters allowed for the optimizer
		static size_t maxIters_;

		static int optIterations_;

};
};// namespace optimizer

#endif
