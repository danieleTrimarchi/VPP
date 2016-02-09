#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

// http://paulbourke.net/miscellaneous/interpolation/

#include "Eigen/Core"
#include "Spline.h"
#include "Plotter.h"
#include <vector>

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

		/// Plot the spline and its underlying source points
		void plot(double minVal,double maxVal,int nVals,
				string title, string xLabel="x", string yLabel="y");

	private:

		/// Generate the underlying spline
		void generate(std::vector<double>&, std::vector<double>&);

		/// Underlying spline
		tk::spline s_;

};

#endif
