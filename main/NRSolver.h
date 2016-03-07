#ifndef NRSOLVER_H
#define NRSOLVER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include "VPPItemFactory.h"
#include "Plotter.h"
#include "Results.h"

using namespace std;

/// Newton-Raphson solver class.
/// todo dtrimarchi: this class is to be inserted in
/// a class hierarchy with its brother class Optimizer
class NRSolver {

	public:

		/// Constructor
		NRSolver(boost::shared_ptr<VPPItemFactory>);

		/// Destructor
		~NRSolver();

		/// Reset the NRSolver when reloading the initial data
		void reset(boost::shared_ptr<VPPItemFactory>);

		/// Run the solver
		void run(int TWV, int TWA);

		/// Make a printout of the results for this run
		void printResults();

		/// Plot the polar plots for the state variables
		/// It is really not the solver to be responsible
		/// for plotting...
		void plotPolars();

		/// Plot the XY plots for the state variables
		/// It is really not the solver to be responsible
		/// for plotting...
		void plotXY( size_t index );

	private:

		/// Set the initial guess for the state variable vector
		void resetInitialGuess(int TWV, int TWA);

		// Struct used to drive twv and twa into the update methods of the VPPItems
		typedef struct {
				int twv_, twa_;
		} Loop_data;

		/// Size of the problem this NRSolver is handling
		size_t dimension_; // --> v, phi, reef, flat

		/// lower and upper bounds for the state variables
		std::vector<double> lowerBounds_,upperBounds_;

		/// Ptr to the VPPItemFactory that contains all of the ingredients
		/// required to compute the optimization constraints
		boost::shared_ptr<VPPItemFactory> vppItemsContainer_;

		/// Ptr to the variableFileParser
		VariableFileParser* pParser_;

		/// Vector with the initial guess/NRSolver results
		Eigen::Vector4d xp_;

		/// Matrix of results, one result per wind velocity/angle
		boost::shared_ptr<ResultContainer> pResults_;

		/// Ptr to the wind item, used to retrieve the current twv, twa
		WindItem* pWind_;

		/// tolerance
		double tol_;

		/// max iterations allowed for the Newton loop
		size_t maxIters_;
};

#endif
