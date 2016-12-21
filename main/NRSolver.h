#ifndef NRSOLVER_H
#define NRSOLVER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include "IOUtils.h"
#include "VPPItemFactory.h"
#include "Results.h"
#include "VPPPlotter.h"

using namespace std;

/// Newton-Raphson solver class.
/// TODO dtrimarchi: this class is to be inserted in
/// a class hierarchy with its brother class Optimizer
class NRSolver {

	public:

		/// Constructor
		NRSolver(VPPItemFactory*, size_t size, size_t subPbSize);

		/// Destructor
		~NRSolver();

		/// Reset the NRSolver when reloading the initial data
		void reset(VPPItemFactory*);

		/// This is similar to a reset, but it is used to change the subPbSize only.
		/// Used - for example - when computing the derivatives of the objective function
		/// in VPP_NLP::computederivative. In that case, the subPbSize is one for du/dphi
		/// but 2 for the other derivatives
		void setSubPbSize(size_t subPbSize);

		/// Run the solver
		void run(int TWV, int TWA);

		/// Run the solver with an external initial guess
		Eigen::VectorXd run(int twv, int twa, Eigen::VectorXd& xp );

		/// Returns the current number of iterations for the last run
		size_t getNumIters();

		/// Make a printout of the results for this run
		void printResults();

		/// Make a printout of the result bounds for this run
		void printResultBounds();

		/// Plot the polar plots for the state variables
		/// It is really not the solver to be responsible
		/// for plotting...
		void plotPolars();

		/// Plot the XY plots for the state variables
		/// It is really not the solver to be responsible
		/// for plotting...
		void plotXY( size_t index );

		/// Plot the Jacobian derivatives on a fixed interval of linearisation
		/// points and for given awv, awa
		void plotJacobian();

		/// Print the result to screen and save it to the result container
		void printAndSave(int twv, int twa);

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	private:

		// Struct used to drive twv and twa into the update methods of the VPPItems
		typedef struct {
				int twv_, twa_;
		} Loop_data;

		/// Size of the problem this NRSolver is handling
		size_t dimension_; // --> v, phi, reef, flat

		/// Size of the subProblem we aim to solve with the help of the NRSolver
		size_t subPbSize_; // --> v, phi

		/// lower and upper bounds for the state variables
		std::vector<double> lowerBounds_,upperBounds_;

		/// Ptr to the VPPItemFactory that contains all of the ingredients
		/// required to compute the optimization constraints
		VPPItemFactory* pVppItemsContainer_;

		/// Ptr to the variableFileParser
		VariableFileParser* pParser_;

		/// Vector with the initial guess/NRSolver subset results
		Eigen::VectorXd xp_, xpBuf_;

		/// Matrix of results, one result per wind velocity/angle
		boost::shared_ptr<ResultContainer> pResults_;

		/// Ptr to the wind item, used to retrieve the current twv, twa
		WindItem* pWind_;

		/// tolerance
		double tol_;

		/// Current number of iterations -- number of iters the last
		/// run required to converge
		size_t it_;

		/// max iterations allowed for the Newton loop
		size_t maxIters_;
};

#endif
