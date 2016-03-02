#ifndef RESULTS_H
#define RESULTS_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include "VPPItemFactory.h"

using namespace std;

/// Struct containing the results of the current
/// run. For each step (i.e. twv,twa it contains
/// the value of the state vector
class Result {

	public:

		/// Default constructor
		Result();

		/// Constructor
		Result(double twv, double twa, std::vector<double>& res, double dF, double dM);

		/// Destructor
		~Result();

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
class ResultContainer {

	public:

		/// Constructor using a windItem
		ResultContainer(WindItem*);

		/// Destructor
		~ResultContainer();

		/// push_back a result taking care of the allocation
		void push_back(size_t iWv, size_t iWa, std::vector<double>& res, double dF, double dM);

		/// Get the result for a given wind velocity/angle
		const Result& get(size_t iWv, size_t iWa) const;

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
		ResultContainer();

		/// Number of true wind velocities/ angles
		size_t nWv_, nWa_;

		/// Ptr to the wind item
		WindItem* pWind_;

		/// Result matrix for each wind velocity/angle
		vector<vector<Result> > resMat_;
};

#endif
