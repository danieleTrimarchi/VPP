#ifndef SEMIANALYTICAL_OPTIMIZER_H
#define SEMIANALYTICAL_OPTIMIZER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include "nlopt.hpp"
#include "NRSolver.h"

#include "VPPItemFactory.h"
#include "Results.h"

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
class SemiAnalyticalOptimizer {

	public:

		/// Constructor
		SemiAnalyticalOptimizer(boost::shared_ptr<VPPItemFactory>);

		/// Destructor
		~SemiAnalyticalOptimizer();

		/// Reset the SemiAnalyticalOptimizer when reloading the initial data
		void reset(boost::shared_ptr<VPPItemFactory>);

		/// Execute a VPP-like analysis
		void run(int TWV, int TWA);

		/// Returns the state vector for a given wind configuration
		const Eigen::VectorXd getResult(int TWV, int TWA);

		/// Make a printout of the results for this run
		/// TODO dtrimarchi : shift this to a mother class
		void printResults();

		/// Save the current results to file
		/// TODO dtrimarchi : shift this to a mother class
		void saveResults();

		/// Read results from file and places them in the current results
		/// TODO dtrimarchi : shift this to a mother class
		void importResults();

		/// Make a printout of the result bounds for this run
		/// TODO dtrimarchi : shift this to a mother class
		void printResultBounds();

		/// Plot the polar plots for the state variables
		void plotPolars();

		/// Plot the XY plots for the state variables
		void plotXY( size_t index );

		/// Ask the underlying NRSolver to plot its Jacobian
		void plotJacobian();

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	private:

		/// Boat velocity objective function
		static double VPP_speed(unsigned n, const double *x, double *grad, void *my_func_data);

		/// Set the initial guess for the state variable vector
		void resetInitialGuess(int TWV, int TWA);

		/// Ask the NRSolver to solve a sub-problem without the optimization variables
		/// this makes the initial guess an equilibrated solution
		void solveInitialGuess(int TWV, int TWA);

		// Struct used to drive twv and twa into the update methods of the VPPItems
		typedef struct {
				int twv_, twa_;
		} Loop_data;

		/// Size of the problem this SemiAnalyticalOptimizer is handling
		size_t dimension_; // --> v, phi, crew, flat

		/// Size of the sub-problem to be pre-solved with the NRSolver
		size_t subPbSize_, saPbSize_; // --> v, phi

		/// Shared ptr holding the underlying SemiAnalyticalOptimizer
		boost::shared_ptr<nlopt::opt> opt_;

		/// Shared ptr holding the underlying NRSolver used to refine the
		/// initial guess to be handed to the SemiAnalyticalOptimizer
		boost::shared_ptr<NRSolver> nrSolver_;

		/// lower and upper bounds for the state variables
		std::vector<double> lowerBounds_,upperBounds_;

		/// Ptr to the VPPItemFactory that contains all of the ingredients
		/// required to compute the optimization constraints
		static boost::shared_ptr<VPPItemFactory> vppItemsContainer_;

		/// Ptr to the variableFileParser
		VariableFileParser* pParser_;

		/// Vector with the initial guess/SemiAnalyticalOptimizer results
		Eigen::VectorXd xp_;

		/// Matrix of results, one result per wind velocity/angle
		boost::shared_ptr<ResultContainer> pResults_;

		/// Ptr to the wind item, used to retrieve the current twv, twa
		WindItem* pWind_;

		/// tolerance
		double tol_;

		/// max iters allowed for the SemiAnalyticalOptimizer
		static size_t maxIters_;

};
};// namespace SemiAnalyticalOptimizer

#endif
