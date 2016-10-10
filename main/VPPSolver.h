#ifndef VPP_SOLVER_H
#define VPP_SOLVER_H

#include "VPPSolverBase.h"

using namespace std;

namespace VPPSolve {

/// VPP solver, wrapper around the NRSolver used to solve
/// the VPP problem without optimization variables. The
/// optimization variables are fixed to default values.
class VPPSolver : public VPPSolverBase {

	public:

		/// Constructor
		VPPSolver(boost::shared_ptr<VPPItemFactory>);

		/// Destructor
		virtual ~VPPSolver();

		/// Reset the VPPSolver when reloading the initial data
		virtual void reset(boost::shared_ptr<VPPItemFactory>);

		/// Execute a VPP-like analysis. Implements pure virtual defined
		/// in the abstract base class
		virtual void run(int TWV, int TWA);

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	private:

		/// Set the initial guess for the state variable vector
		virtual void resetInitialGuess(int TWV, int TWA);

		/// Ask the NRSolver to solve a sub-problem without the optimization variables
		/// this makes the initial guess an equilibrated solution
		virtual void solveInitialGuess(int TWV, int TWA);

		double fixedB_, fixedF_;

};
};// namespace VPPSolve

#endif
