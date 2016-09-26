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

		/// Constructor -- only doubles
		Result(	size_t itwv, double twv,
						size_t itwa, double twa,
						double  v, double phi,
						double b, double f,
						double dF, double dM,
						bool discarde=false);

		/// Constructor
		Result(	size_t itwv, double twv,
						size_t itwa, double twa,
						std::vector<double>& res,
						double dF, double dM,
						bool discarde=false);

		/// Constructor with residual array
		Result(	size_t itwv, double twv,
						size_t itwa, double twa,
						Eigen::VectorXd& res,
						Eigen::VectorXd& residuals,
						bool discarde=false );

		/// Destructor
		~Result();

		/// PrintOut the values stored in this result. Use stdout as default stream
		void print(FILE* outStream=stdout) const;

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
		void setDiscard( const bool discard );

		/// Discard this solution: do not plot it
		const bool discard() const;

		/// Comparison operator
		bool operator == (const Result& ) const;

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	private:

		/// Indices of this result in the result array
		size_t itwv_, itwa_;

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

		/// Alternative signature for push_back (compatibility)
		void push_back(size_t iWv, size_t iWa,
										double v, double phi, double b, double f,
										double dF, double dM );

		/// Alternative signature for push_back (compatibility)
		void push_back(size_t iWv, size_t iWa,
										Eigen::VectorXd& results,
										double dF, double dM );

		/// push_back a result taking care of the allocation
		void push_back(size_t iWv, size_t iWa,
										Eigen::VectorXd& results,
										Eigen::VectorXd& residuals,
										bool discard=false );

		/// Push a trivial result marked as to be discarded -> not plot
		void remove(size_t iWv, size_t iWa);

		/// Get the result for a given wind velocity/angle
		const Result& get(size_t iWv, size_t iWa) const;

		/// How many results have been stored?
		const size_t size() const;

		/// Count the number of results that must not be plotted for
		/// a given wind angle. Note that the method is brute force,
		/// but it has the advantage of assuring the sync
		const size_t getNumDiscardedResultsForAngle(size_t iWa) const;

		/// Count the number of results that must not be plotted for
		/// a given wind velocity. Note that the method is brute force,
		/// but it has the advantage of assuring the sync
		const size_t getNumDiscardedResultsForVelocity(size_t iWv) const;

		/// Count the number of results that must not be plotted for a
		/// given angle. Note that the method is brute force, but it has
		/// the advantage of assuring the sync
		const size_t getNumDiscardedResults() const;

		/// How many wind velocities?
		const size_t windVelocitySize() const;

		/// How many wind angles?
		const size_t windAngleSize() const;

		/// Return the total number of valid results: the results that have not
		/// been discarded
		const size_t getNumValidResults() const;

		/// Return the number of valid velocity-wise results for a given angle
		const size_t getNumValidResultsForAngle( size_t iWa ) const;

		/// Get a ptr to the wind item
		const WindItem* getWind() const;

		/// Printout the list of Opt Results, arranged by twv-twa
		void print(FILE* outStream=stdout);

		/// Printout the bounds of the state variables for the whole run
		void printBounds();

		/// CLear the result vector
		void clear();

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
