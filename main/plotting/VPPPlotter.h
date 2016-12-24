#ifndef VPP_PLOTTER_H
#define VPP_PLOTTER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

// This resolves a name collision between eigen and X11 that results in the compiler
// error: #error The preprocessor symbol 'Success' is defined, possibly by the X11 head...
#ifdef Success
	#undef Success
#endif

#include <Eigen/Core>
using namespace Eigen;

/// Directives for PLPLOT
#include "plc++demos.h"

/// Define colors -- see plplot_5.11 guide 18.13: plCol0
enum color{
	black,
	red,
	yellow,
	green,
	aquamarine,
	pink,
	wheat,
	grey,
	brown,
	blue,
	BlueViolet,
	cyan,
	turquoise,
	magenta,
	salmon,
	white
};

enum colorplot{
	colorMap,
	greyScale
};

class VPPPlotterBase {

	public:

		/// Constructor
		VPPPlotterBase();

		/// Destructor
		~VPPPlotterBase();

	protected:

		/// Reset the ranges to very big values
		void initRanges();

		/// Reset the ranges to the ranges of a point set - version for Eigen
		void resetRanges(Eigen::ArrayXd& x0, Eigen::ArrayXd& y0);

		/// Reset the ranges to the ranges of a point set - version for Eigen
		void resetRanges(Eigen::MatrixXd& x0, Eigen::MatrixXd& y0, bool axisEqual=false);

		/// Reset the ranges to the ranges of a point set -- version for vectors
		void resetRanges(std::vector<double>& x0, std::vector<double>& y0);

		/// Reset the ranges to the ranges of two point set -- version for Eigen
		void resetRanges(Eigen::ArrayXd& x0, Eigen::ArrayXd& y0,Eigen::ArrayXd& x1, Eigen::ArrayXd& y1);

		/// Reset the ranges to the ranges of two point set -- version for vectors
		void resetRanges(std::vector<double>& x0,std::vector<double>& y0,std::vector<double>& x1,std::vector<double>& y1);

		/// Find the min of the specified vector
		double min(std::vector<double>&);

		/// Find the max of the specified vector
		double max(std::vector<double>&);

		/// plot ranges
		double minX_, minY_, maxX_, maxY_;

};


/// Wrapper class for ploPlot
class VPPPlotter : public VPPPlotterBase {

	public:

		/// Constructor
		VPPPlotter();

		/// Destructor
		virtual ~VPPPlotter();

		/// Plot the points of some given arrays
		void plot( std::vector<double>& y, string title="Plot");

		/// Plot the points of some given arrays
		void plot(Eigen::ArrayXd& x, Eigen::ArrayXd& y, string title="Plot");

		/// Plot the points of two given array sets
		void plot(Eigen::ArrayXd& x0,
				Eigen::ArrayXd& y0,
				Eigen::ArrayXd& x1,
				Eigen::ArrayXd& y1,
				string title="Plot",
				string xLabel="x",
				string yLabel="y"
		);

		/// Plot the points of one given array set
		void plot(std::vector<double>& x0,
				std::vector<double>& y0,
				string title="Plot",
				string xLabel="x",
				string yLabel="y"
		);

		/// Plot the points of two given array sets
		void plot(std::vector<double>& x0,
				std::vector<double>& y0,
				std::vector<double>& x1,
				std::vector<double>& y1,
				string title="Plot",
				string xLabel="x",
				string yLabel="y"
		);

		/// Append a set of data. Allows for multi-curve plots
		void append(string curveLabel, Eigen::ArrayXd& xs, Eigen::ArrayXd& ys);

		/// Plot the data that have been previously appended to the buffer vectors
		void plot(string xLabel,string yLabel,string plotTitle);

	protected:

		/// Find the min of the specified c-style array
		double min(double*);

		/// Find the max of the specified c-style array
		double max(double*);

		/// Number of points the plot is made of
		int nValues_;

		/// Primitive arrays plplot is feed with
		double* x_, * y_;

	private:

		/// Copy the values into plplot compatible containers -- version for Eigen
		void setValues(Eigen::ArrayXd& x, Eigen::ArrayXd& y);

		/// Copy the values into plplot compatible containers -- version for vectors
		void setValues(std::vector<double>& x, std::vector<double>& y);

		/// Values that constitute the plot. The points of each curve in the
		/// plot are appended to these vectors
		std::vector<Eigen::ArrayXd> xs_, ys_;

		/// index of the position where we aim to place the curve label for this curve
		std::vector<size_t> idx_;

		/// Title of each curve to be plot
		std::vector<string> curveLabels_;

};

////////////////////////////////////////////////////////////////////////////////
// Plotter class used to plot vectorFields
//
// Usage Example:
//size_t nx=20, ny=1;
//Eigen::MatrixXd x(nx,ny);
//Eigen::MatrixXd y(nx,ny);
//for(size_t i=0; i<nx; i++) {
//	for(size_t j=0; j<ny; j++) {
//		 x(i,j)=i;
//		 y(i,j)=3;
//	}
//}
//
//Eigen::MatrixXd du(nx,ny), dv(nx,ny);
//for(size_t i=0; i<nx; i++) {
//	for(size_t j=0; j<ny; j++) {
//		du(i,j) = .2;
//		dv(i,j) = 0.0001 * i*i;
//	}
//}
//
//// instantiate a vectorPlotter and quit
//VectorPlotter vecplot;
//vecplot.plot(x,y,du,dv);

class VPPVectorPlotter : public VPPPlotterBase {

	public:

		/// Constructor
		VPPVectorPlotter();

		/// Destructor
		~VPPVectorPlotter();

		/// Vector plot for a grid of m points,
		/// the coordinates of which are x,y
		/// for each couple x,y the arrays du,dv
		/// store the isoparametric coordinates
		/// of the vector plot. Note that the magnitudes
		/// are rescaled to unity
		void plot(Eigen::MatrixXd& x,
				Eigen::MatrixXd& y,
				Eigen::MatrixXd& du,
				Eigen::MatrixXd& dv,
				double scale=1.,
				string title="Plot",
				string xLabel="x",
				string yLabel="y"
		);

	private:

		/// Number of points the underlying grid is made of
		int nX_, nY_;

};

////////////////////////////////////////////////////////////////////////////////

class VPPPolarPlotter {

	public:

		/// Constructor
		VPPPolarPlotter( string title );

		/// Destructor
		~VPPPolarPlotter();

		/// Append a set of polar data
		void append(string curveLabel, Eigen::ArrayXd& alpha, Eigen::ArrayXd& vals);

		/// Append a set of polar data
		void append(string curveLabel, std::vector<double>& alpha, std::vector<double>& vals);

		/// Plot the data appended to the plotter
		void plot(size_t skipCircles=1);

	private:

		/// Copy the values from the ArrayXd to the c-style containers plPlot is fed with
		void setValues(Eigen::ArrayXd& x, Eigen::ArrayXd& y);

		/// Pi in radians
		double pi_;

		/// Ranges of the alpha and vals arrays
		double minAlphaRange_, maxAlphaRange_, maxValRange_;

		/// Title of the plot
		string title_;

		/// Values that constitute the plot
		std::vector<Eigen::ArrayXd> alphas_, vals_;

		/// index of the position where we aim to place the curve label for this curve
		std::vector<size_t> idx_;

		/// Title of each curve to be plot
		std::vector<string> curveLabels_;

		/// Primitive arrays plplot is feed with
		double* x_, * y_;

};

////////////////////////////////////////////////////////////////////////////////

class VPPPlotter3d {

	public:

		/// Destructor
		virtual ~VPPPlotter3d();

		/// Init method
		void cmap1_init( int );

	protected:

		/// Protected constructor
		VPPPlotter3d(ArrayXd& x, ArrayXd& y, MatrixXd& z,
				string title, string xLabel, string yLabel );

		/// Plot!
		virtual void plot();

		/// Number of points this plot is made of in both directions
		size_t nPtsX_, nPtsY_;

		/// -x and -y wise axis value containers
		double *x_, *y_;

		/// function values array
		double **z_;

		/// bounds of the functions to be plotted in -z
		double xMin_, xMax_, yMin_, yMax_, zMin_, zMax_;

		double MIN( double x, double y ) { return ( x < y ? x : y ); };
		double MAX( double x, double y ) { return ( x > y ? x : y ); };

	private:

		/// Alt and azimuth, perspective view observation point
		double alt_, az_;

		/// Title of the plot
		string title_, xLabel_, yLabel_;

};

class VPPDiffuseLightSurfacePlotter3d : public VPPPlotter3d {

	public:

		/// Constructor
		VPPDiffuseLightSurfacePlotter3d(ArrayXd& x, ArrayXd& y, MatrixXd& z,
				string title, string xLabel, string yLabel );

		/// Destructor
		virtual ~VPPDiffuseLightSurfacePlotter3d();

		/// Plot!
		virtual void plot();

	private:

};

class VPPMagnitudeColoredPlotter3d : public VPPPlotter3d {

	public:

		/// Constructor
		VPPMagnitudeColoredPlotter3d(ArrayXd& x, ArrayXd& y, MatrixXd& z,
				string title, string xLabel, string yLabel );

		/// Constructor to plot surface and points
		VPPMagnitudeColoredPlotter3d(
				ArrayXd& x, ArrayXd& y, MatrixXd& z,
				ArrayXd& xp, ArrayXd& yp, ArrayXd& zp,
				string title, string xLabel, string yLabel );

		/// Destructor
		virtual ~VPPMagnitudeColoredPlotter3d();

		/// Plot!
		virtual void plot();

	private:

		ArrayXd* pXp_, *pYp_, *pZp_;

};

class VPPMagnitudeColoredFacetedPlotter3d : public VPPPlotter3d {

	public:

		/// Constructor
		VPPMagnitudeColoredFacetedPlotter3d(ArrayXd& x, ArrayXd& y, MatrixXd& z,
				string title, string xLabel, string yLabel );

		/// Destructor
		virtual ~VPPMagnitudeColoredFacetedPlotter3d();

		/// Plot!
		virtual void plot();

};

/// Base class for 3d plots featuring contours
class VPPCountourPlotter3d : public VPPPlotter3d {

	public:

	protected:

		// Constructor
		VPPCountourPlotter3d(ArrayXd& x, ArrayXd& y, MatrixXd& z,
				string title, string xLabel, string yLabel );

		// Destructor
		~VPPCountourPlotter3d();

		/// Plot!
		virtual void plot();

		/// Number of contour levels
		size_t nLevels_;

		/// C-style array with the values of
		/// the contour levels
		double* cLevel_;

	private:

};


class VPPMagnitudeColoredCountourPlotter3d : public VPPCountourPlotter3d {

	public:

		/// Constructor
		VPPMagnitudeColoredCountourPlotter3d(ArrayXd& x, ArrayXd& y, MatrixXd& z,
																			string title, string xLabel, string yLabel );

		/// Destructor
		virtual ~VPPMagnitudeColoredCountourPlotter3d();

		/// Plot!
		virtual void plot();

	private:

};

class VPPMagnitudeColoredCountourLimitedPlotter3d : public VPPCountourPlotter3d {

	public:

		/// Constructor
		VPPMagnitudeColoredCountourLimitedPlotter3d(ArrayXd& x, ArrayXd& y, MatrixXd& z,
				string title, string xLabel, string yLabel );

		/// Destructor
		virtual ~VPPMagnitudeColoredCountourLimitedPlotter3d();

		/// Plot!
		virtual void plot();

	private:

		double** zLimited_;

		int* indexymin_, *indexymax_;

};


#endif
