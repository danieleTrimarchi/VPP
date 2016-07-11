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

		/// Test Constructor. Underneath calls compute, so it is standalone
		Regression();

		/// Constructor with point arrays. Requires calling compute()
		Regression(Eigen::MatrixXd&, Eigen::MatrixXd&, Eigen::MatrixXd& );

		/// Destructor
		~Regression();

		/// Compute the coefficient of the regression given the point array
		Eigen::VectorXd compute();

	private:

		/// Run a test : define the points using a polynomial, then reconstruct
		/// the regression and verify the coefficients are the initial ones. This
		/// will be embedded into an autotest
		void runTest();

		/// Point array to be used to compute the regression
		Eigen::MatrixXd x_, y_, z_;
};

#endif
