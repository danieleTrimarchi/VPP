#ifndef REGRESSION_H
#define REGRESSION_H

#include <Eigen/Core>
#include <stdio.h>
#include <iostream>

/// Aim of this class is to return a size-6 vector with the coefficients of a
/// paraboloid that minimizes the distance from a given array of points.
/// It is a regression in 3d.

class Regression {

	public:

		/// Constructor with point arrays. Requires calling compute()
		Regression(Eigen::MatrixXd&, Eigen::MatrixXd&, Eigen::MatrixXd& );

		/// Destructor
		~Regression();

		/// Compute the coefficient of the regression given the point array
		Eigen::VectorXd compute();

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	private:

		/// Point array to be used to compute the regression
		Eigen::MatrixXd xp_, yp_, zp_;
};

#endif
