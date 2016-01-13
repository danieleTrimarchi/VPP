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
		virtual double interpolate(double val,Eigen::ArrayXd& X,Eigen::ArrayXd& Y);

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

		/// Constructor
		SplineInterpolator();

		/// Destructor
		virtual ~SplineInterpolator();

		/// Interpolate the function X-Y for the value val
		virtual double interpolate(double val,Eigen::ArrayXd& X,Eigen::ArrayXd& Y);

		// Interpolate the function X-Y for the value val
		double interpolate(double val,std::vector<double>& x, std::vector<double>& y );

		// Interpolate on an already compute spline
		double get(double val);

		/// Test the spline header from http://kluge.in-chemnitz.de/opensource/spline/
		void testSpline();

	private:

		tk::spline s_;

};

#endif
