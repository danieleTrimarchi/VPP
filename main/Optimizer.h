#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include "nlopt.hpp"

#include "VPPItemFactory.h"
#include "Plotter.h"

using namespace std;

/// Struct containing the results of the current
/// run. For each step (i.e. twv,twa it contains
/// the value of the state vector
class OptResult {

	public:

		/// Default constructor
		OptResult();

		/// Constructor
		OptResult(double twv, double twa, std::vector<double>& res, double dF, double dM);

		/// Destructor
		~OptResult();

		/// PrintOut the values stored in this result
		void print();

		// get the twv for this result
		const double getTWV() const;

		// get the twa for this result
		const double getTWA() const;

		// get the force residuals for this result
		const double getdF() const;

		// get the moment residuals for this result
		const double getdM() const;

		// get the state vector for this result
		const std::vector<double>* getX() const;

	private:

		/// Value of wind TRUE velocity/angle
		double twv_,twa_;

		/// State vector
		std::vector<double> result_;

		// Force and moment residuals
		double dF_, dM_;

} ;

/// Container for OptResult, which is a wrapper around
/// vector<vector<OptResults? > >
/// This utility class is used to store a result for each
/// twv and twa
class OptResultContainer {

	public:

		/// Constructor using a windItem
		OptResultContainer(WindItem*);

		/// Destructor
		~OptResultContainer();

		/// push_back a result taking care of the allocation
		void push_back(size_t iWv, size_t iWa, std::vector<double>& res, double dF, double dM);

		/// Get the result for a given wind velocity/angle
		const OptResult& get(size_t iWv, size_t iWa) const;

		/// How many results have been stored?
		const size_t size() const;

		/// How many wind velocities?
		const size_t windVelocitySize() const;

		/// How many wind angles?
		const size_t windAngleSize() const;

		/// Printout the list of Opt Results, arranged by twv-twa
		void print();

	private:

		/// Default constructor
		OptResultContainer();

		/// Number of true wind velocities/ angles
		size_t nWv_, nWa_;

		/// Ptr to the wind item
		WindItem* pWind_;

		/// Result matrix for each wind velocity/angle
		vector<vector<OptResult> > resMat_;
};

/// Wrapper class around NLOPT non-linear
/// optimization library
class Optimizer {

	public:

		/// Constructor
		Optimizer(boost::shared_ptr<VPPItemFactory>);

		/// Destructor
		~Optimizer();

		/// Reset the optimizer when reloading the initial data
		void reset(boost::shared_ptr<VPPItemFactory>);

		/// Execute a VPP-like analysis. Returns 0 on success,
		/// -1 if the residuals are found to exceed a tolerance
		void run(int TWV, int TWA);

		/// Make a printout of the results for this run
		void printResults();

		/// Plot the polar plots for the state variables
		void plotPolars();

		/// Plot the XY plots for the state variables
		void plotXY( size_t index );

	private:

		/// Boat velocity objective function
		static double VPP_speed(unsigned n, const double *x, double *grad, void *my_func_data);

		/// Set the constraint: dF=0 and dM=0
		static void VPPconstraint(unsigned m, double *result, unsigned n, const double* x, double* grad, void* f_data);

		/// Set the initial guess for the state variable vector
		void resetInitialGuess(int TWV, int TWA);

		// Struct used to drive twv and twa into the update methods of the VPPItems
		typedef struct {
				int twv_, twa_;
		} Loop_data;

		/// Size of the problem this Optimizer is handling
		size_t dimension_; // --> v, phi, reef, flat

		/// Shared ptr holding the underlying optimizer
		boost::shared_ptr<nlopt::opt> opt_;

		/// lower and upper bounds for the state variables
		std::vector<double> lowerBounds_,upperBounds_;

		/// Ptr to the VPPItemFactory that contains all of the ingredients
		/// required to compute the optimization constraints
		static boost::shared_ptr<VPPItemFactory> vppItemsContainer_;

		/// Ptr to the variableFileParser
		VariableFileParser* pParser_;

		/// Vector with the initial guess/optimizer results
		std::vector<double> xp_;

		/// Matrix of results, one result per wind velocity/angle
		boost::shared_ptr<OptResultContainer> pResults_;

		/// Ptr to the wind item, used to retrieve the current twv, twa
		WindItem* pWind_;

		/// tolerance
		double tol_;

		/// max iters allowed for the optimizer
		static size_t maxIters_;

};

#endif
