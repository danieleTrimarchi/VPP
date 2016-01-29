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

		/// Constructor
		OptResult(double twv, double twa, std::vector<double>& res);

		/// Destructor
		~OptResult();

		/// PrintOut the values stored in this result
		void print();

		// get the twv for this result
		const double getTWV() const;

		// get the twa for this result
		const double getTWA() const;

		// get the state vector for this result
		const std::vector<double>* getX() const;

	private:

		/// Value of wind TRUE velocity/angle
		double twv_,twa_;

		/// State vector
		std::vector<double> result_;

} ;

/// Wrapper class around NLOPT non-linear
/// optimization library
class Optimizer {

	public:

		/// Constructor
		Optimizer(boost::shared_ptr<VPPItemFactory>);

		/// Destructor
		~Optimizer();

		/// Execute a VPP-like analysis
		void run(int TWV, int TWA);

		/// Make a printout of the results for this run
		void printResults();

		/// Make a printout of the results for this run
		void plotResults();

	private:

		/// Boat velocity objective function
		static double VPP_speed(unsigned n, const double *x, double *grad, void *my_func_data);

		/// Set the constraint: dF=0 and dM=0
		static void VPPconstraint(unsigned m, double *result, unsigned n, const double* x, double* grad, void* f_data);

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

		/// Vector of results, one result per Optimizer run
		std::vector<OptResult> results_;

		/// Ptr to the wind item, used to retrieve the current twv, twa
		WindItem* pWind_;

};

#endif
