#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

// http://paulbourke.net/miscellaneous/interpolation/

#include "Eigen/Core"
#include "Spline.h"
#include <vector>

#include "VPPPlotter.h"

/// Forward declarations
class VPPXYChart;
class VppXYCustomPlotWidget;

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

		/// Destructor
		virtual ~SplineInterpolator();

		/// How many points are used to build this spline?
		size_t getNumPoints() const;

		/// Returns the max y-value of the underlying control points
		double getMax_Ycp_Value() const;

		/// Returns the min y-value of the underlying control points
		double getMin_Ycp_Value() const;

		/// Interpolate the function X-Y using the underlying spline for the value val
		double interpolate(double);

		/// Plot the spline and its underlying source points
		void plot(double minVal,double maxVal,int nVals,
				string title, string xLabel="x", string yLabel="y");

		/// Plot the spline and its underlying source points.
		/// Hand the points to a Qt XY plot
		void plot(VPPXYChart& chart, double minVal,double maxVal,int nVals);

		/// Plot the spline and its underlying source points.
		/// Hand the points to a QCustomPlot
		void plot(VppXYCustomPlotWidget* chart, double minVal,double maxVal,int nVals);

		/// Plot the first derivative of the spline
		void plotD1(double minVal,double maxVal,int nVals,
				string title, string xLabel="x", string yLabel="y");

		/// Plot the second derivative of the spline
		void plotD2(double minVal,double maxVal,int nVals,
				string title, string xLabel="x", string yLabel="y");

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	private:

		/// Generate the underlying spline
		void generate();

		/// Underlying spline
		tk::spline s_;

		// Underlying value vectors
		std::vector<double> X_, Y_;

		// Max and min values of the generator points. To be used
		// for example to define the bounds of a plot
		double max_Ycp_Value_ , min_Ycp_Value_;

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
