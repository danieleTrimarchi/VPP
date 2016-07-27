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
		Result(	double twv, double twa,
						std::vector<double>& res,
						double dF, double dM,
						bool discarde=false);

		/// Constructor with residual array
		Result(	double twv, double twa,
						Eigen::VectorXd& res,
						Eigen::VectorXd& residuals,
						bool discarde=false );

		/// Destructor
		~Result();

		/// PrintOut the values stored in this result
		void print();

		/// Get the twv for this result
		const double getTWV() const;

		/// Get the twa for this result
		const double getTWA() const;

		/// Get the force residuals for this result
		const double getdF() const;

		/// Get the moment residuals for this result
		const double getdM() const;

		/// Get the state vector for this result
		const Eigen::VectorXd* getX() const;

		/// Discard this solution: do not plot it
		const bool discard() const;


		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	private:

		/// Value of wind TRUE velocity/angle
		double twv_,twa_;

		/// State vector
		Eigen::VectorXd result_;

		/// Force and moment, c1 residuals
		Eigen::VectorXd residuals_;

		/// Flag used to mark a result that must be discarded -> not plotted
		bool discard_;

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

		/// Alternative signature for push_back
		void push_back(size_t iWv, size_t iWa,
										Eigen::VectorXd& results,
										double dF, double dM );

		/// push_back a result taking care of the allocation
		void push_back(size_t iWv, size_t iWa,
										Eigen::VectorXd& results,
										Eigen::VectorXd& residuals,
										bool discard=false );

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

		/// Printout the bounds of the state variables for the whole run
		void printBounds();

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
