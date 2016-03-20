#ifndef PLOTTER_H
#define PLOTTER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include <Eigen/Core>
using namespace Eigen;

/// Directives for PLPLOT
#include "plcdemos.h"

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

class PlotterBase {

	public:

		/// Constructor
		PlotterBase();

		/// Destructor
		~PlotterBase();

	protected:

		/// Reset the ranges to very big values
		void initRanges();

		/// Reset the ranges to the ranges of a point set - version for Eigen
		void resetRanges(Eigen::ArrayXd& x0, Eigen::ArrayXd& y0);

		/// Reset the ranges to the ranges of a point set - version for Eigen
		void resetRanges(Eigen::MatrixXd& x0, Eigen::MatrixXd& y0);

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
class Plotter : public PlotterBase {

	public:

		/// Constructor
		Plotter();

		/// Destructor
		virtual ~Plotter();

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

class VectorPlotter : public PlotterBase {

	public:

		/// Constructor
		VectorPlotter();

		/// Destructor
		~VectorPlotter();

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
				string title="Plot",
				string xLabel="x",
				string yLabel="y"
		);

	private:

		/// Number of points the underlying grid is made of
		int nX_, nY_;

};

////////////////////////////////////////////////////////////////////////////////

class PolarPlotter {

	public:

		/// Constructor
		PolarPlotter( string title );

		/// Destructor
		~PolarPlotter();

		/// Append a set of polar data
		void append(string curveLabel, Eigen::ArrayXd& alpha, Eigen::ArrayXd& vals);

		/// Plot the data appended to the plotter
		void plot();

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

#endif
