#ifndef VPP_SOLVER_H
#define VPP_SOLVER_H

#include "VPPSolverBase.h"

using namespace std;

namespace Optim {

// forward declaration
class SolverFactory;

/// VPP solver, wrapper around the NRSolver used to solve
/// the VPP problem without optimization variables. The
/// optimization variables are fixed to default values.
class VPPSolver : public VPPSolverBase {

	public:

		/// Destructor
		virtual ~VPPSolver();

		/// Reset the VPPSolver when reloading the initial data
		virtual void reset(std::shared_ptr<VPPItemFactory>);

		/// Execute a VPP-like analysis. Implements pure virtual defined
		/// in the abstract base class
		virtual void run(int TWV, int TWA);

	private:

		/// This class is to be instantiated using a NLOptSolverFactory.
		/// The friendship allows the factory to call the private constructor
		friend class SolverFactory;

		/// Private constructor - the class can only be instantiated using
		/// a VPPSolverFactory
		VPPSolver(std::shared_ptr<VPPItemFactory>);

		/// Set the initial guess for the state variable vector
		virtual void resetInitialGuess(int TWV, int TWA);

		double fixedB_, fixedF_;

};
};// namespace VPPSolve

#endif
