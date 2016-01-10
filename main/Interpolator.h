#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

// http://paulbourke.net/miscellaneous/interpolation/

#include "Eigen/Core"

class Interpolator {

	public:

		/// Constructor
		Interpolator();

		/// Destructor
		~Interpolator();

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


#endif
