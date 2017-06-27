#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

// http://paulbourke.net/miscellaneous/interpolation/

#include "Eigen/Core"
#include "Spline.h"
#include <vector>

/// Forward declarations
class VppXYCustomPlotWidget;

class Interpolator {

	public:

		/// Constructor
		Interpolator();

		/// Destructor
		virtual ~Interpolator();

		/// Interpolate the function X-Y for the value val
		double interpolate(double val,Eigen::ArrayXd& X,Eigen::ArrayXd& Y);

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

		/// Destructor
		virtual ~SplineInterpolator();

		/// How many points are used to build this spline?
		size_t getNumPoints() const;

		/// Interpolate the function X-Y using the underlying spline for the value val
		double interpolate(double);

		/// Plot the spline and its underlying source points.
		/// Hand the points to a QCustomPlot
		void plot(VppXYCustomPlotWidget* chart, double minVal,double maxVal,int nVals);

		/// Plot the first derivative of the spline and its underlying source points
		void plotD1(VppXYCustomPlotWidget* plot, double minVal,double maxVal,int nVals );

		/// Plot the second derivative of the spline and its underlying source points
		void plotD2(VppXYCustomPlotWidget* plot, double minVal,double maxVal,int nVals );

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	private:

		/// Generate the underlying spline
		void generate();

		/// Underlying spline
		tk::spline s_;

		// Underlying value vectors
		std::vector<double> X_, Y_;

};

////////////////////////////////////////////////////////////////////////////////////

class Extrapolator {

	public:

		/// Constructor
		Extrapolator(double xm2, const Eigen::VectorXd* vm2, double xm1, const Eigen::VectorXd* vm1);

		/// Get the vector with the value extrapolated for the abscissa x
		Eigen::VectorXd get(double x);

	private:

		/// values of the abscissas
		double xm2_, xm1_;

		/// Ptrs to the vectors with the vals to extrapolate
		const Eigen::VectorXd* pVm2_, *pVm1_;

		/// Vector with the extrapolated values to be filled
		std::vector<double> v_;

};

#endif
