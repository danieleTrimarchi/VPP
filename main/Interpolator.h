#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

// http://paulbourke.net/miscellaneous/interpolation/

#include "Eigen/Core"
#include "Spline.h"
#include <vector>

/// Utility class used to sort a vector based on the ordering of the second.

class Sorter {

	public:

		// Constructor
		Sorter(std::vector<double>*px,std::vector<double>*py);

		/// Define operator () used by std::sort
		bool operator() (int i,int j);

		// Destructor
		~Sorter();

	private:

		/// Ptrs to the arrays to be sorted
		std::vector<double>* px_, * py_;

};

/////////////////////////////////////////////////////////////////

class Interpolator {

	public:

		/// Constructor
		Interpolator();

		/// Destructor
		virtual ~Interpolator();

		/// Interpolate the function X-Y for the value val
		double interpolate(double val,Eigen::ArrayXd& X,Eigen::ArrayXd& Y);

		/// Test the interpolator on some simple function
		void test();

	private:

		/// Linearly interpolate
		double LinearInterpolate(double y1,double y2,double mu);

		/// Interpolate using cosines
		double CosineInterpolate(double y1,double y2,double mu);

};

////////////////////////////////////////////////////////////////////////////////////

class SplineInterpolator {

	public:

		/// Default constructor used to test
		SplineInterpolator();

		/// Constructor for Eigen::ArrayXd
		SplineInterpolator(Eigen::ArrayXd&,Eigen::ArrayXd&);

		/// Constructor for std::vector
		SplineInterpolator(std::vector<double>&, std::vector<double>&);

		/// Destructor
		virtual ~SplineInterpolator();

		/// Interpolate the function X-Y using the underlying spline for the value val
		double interpolate(double);

	private:

		/// Generate the underlying spline
		void generate(std::vector<double>&, std::vector<double>&);

		/// Underlying spline
		tk::spline s_;

};

#endif
