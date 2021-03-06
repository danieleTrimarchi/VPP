#ifndef VPP_SolverBase_H
#define VPP_SolverBase_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include "VPPItemFactory.h"
#include "Results.h"
#include "NRSolver.h"
#include "VPPGradient.h"

using namespace std;

/// Abstract class base for all solvers and optimizers
class VPPSolverBase {

	public:

		/// Destructor
		virtual ~VPPSolverBase();

		/// Reset the optimizer when reloading the initial data
		virtual void reset(std::shared_ptr<VPPItemFactory>);

		/// Pure virtual used to execute a VPP-like analysis
		virtual void run(int TWV, int TWA) =0;

		/// Returns the state vector for a given wind configuration
		const Eigen::VectorXd getResult(int TWV, int TWA);

		/// Make a printout of the results for this run
		void printResults();

		/// Save the current results to file
		void saveResults(string fileName);

		/// Read results from file and places them in the current results
		void importResults(string fileName);

		/// Make a printout of the result bounds for this run
		void printResultBounds();

		/// Plot the polar plots for the state variables
		void plotPolars(MultiplePlotWidget*);

		/// Plot the XY results
		void plotXY(MultiplePlotWidget*, WindIndicesDialog&);

		/// Returns the dimensionality of this problem (the size of the state vector)
		size_t getDimension() const;

		/// Return a ptr to the results.
		ResultContainer* getResults();

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	protected:

		/// Protected constructor
		VPPSolverBase(std::shared_ptr<VPPItemFactory>);

		/// Set the initial guess for the state variable vector
		virtual void resetInitialGuess(int TWV, int TWA);

		/// Returns the index of the previous velocity-wise (twv) result that is marked as
		/// converged (discarde==false). It starts from 'current', so it can be used recursively
		size_t getPreviousConverged(size_t current, size_t TWA);

		/// Ask the NRSolver to solve a sub-problem without the optimization variables
		/// this makes the initial guess an equilibrated solution
		virtual void solveInitialGuess(int TWV, int TWA);

		/// Disallow default constructor
		VPPSolverBase();

		/// Size of the problem this Optimizer is handling
		size_t dimension_; // --> v, phi, crew, flat

		/// Size of the sub-problem to be pre-solved with the NRSolver
		size_t subPbSize_; // --> v, phi

		/// Shared ptr holding the underlying NRSolver used to refine the
		/// initial guess to be handed to the optimizer
		std::shared_ptr<NRSolver> nrSolver_;

		/// VPPGradient used to compute the gradient by finite differences
		std::shared_ptr<VPPGradient> pGradient_;

		/// lower and upper bounds for the state variables
		std::vector<double> lowerBounds_,upperBounds_;

		/// Ptr to the VPPItemFactory that contains all of the ingredients
		/// required to compute the optimization constraints
		static std::shared_ptr<VPPItemFactory> pVppItemsContainer_;

		/// Ptr to the variableFileParser
		VariableFileParser* pParser_;

		/// Vector with the initial guess/optimizer results
		Eigen::VectorXd xp_;

		/// Matrix of results, one result per wind velocity/angle
		std::shared_ptr<ResultContainer> pResults_;

		/// Ptr to the wind item, used to retrieve the current twv, twa
		WindItem* pWind_;

		/// tolerance
		double tol_;

	private:

		/// Declare a static const initial guess state vector
		static const Eigen::VectorXd xp0_;


};

#endif
