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

/// Wrapper class for ploPlot
class Plotter {

	public:
		
		/// Constructor
		Plotter();
		
		/// Destructor
		virtual ~Plotter();
		
		/// Plot the points of some given arrays
		void plot(Eigen::ArrayXd& x, Eigen::ArrayXd& y, string title="Plot");

		/// Plot the points of two given array sets
		void plot(Eigen::ArrayXd& x0,
							Eigen::ArrayXd& y0,
							Eigen::ArrayXd& x1,
							Eigen::ArrayXd& y1
							);

		/// Plot the points of two given array sets
		void plot(std::vector<double>& x0,
							std::vector<double>& y0,
							std::vector<double>& x1,
							std::vector<double>& y1,
							std::string title
							);

	protected:

		/// Find the min of the specified c-style array
		double min(double*);

		/// Find the min of the specified vector
		double min(std::vector<double>&);

		/// Find the max of the specified c-style array
		double max(double*);

		/// Find the max of the specified vector
		double max(std::vector<double>&);

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

		/// Number of points the plot is made of
		int nValues_;

		/// Primitive arrays plplot is feed with
		double* x_, * y_;

	private:

		/// Copy the values into plplot compatible containers -- version for Eigen
		void setValues(Eigen::ArrayXd& x, Eigen::ArrayXd& y);

		/// Copy the values into plplot compatible containers -- version for vectors
		void setValues(std::vector<double>& x, std::vector<double>& y);

		/// Reset the ranges to very big values
		void initRanges();

		/// Reset the ranges to the ranges of a point set - version for Eigen
		void resetRanges(Eigen::ArrayXd& x0, Eigen::ArrayXd& y0);

		/// Reset the ranges to the ranges of a point set -- version for vectors
		void resetRanges(std::vector<double>& x0, std::vector<double>& y0);

		/// Reset the ranges to the ranges of two point set -- version for Eigen
		void resetRanges(Eigen::ArrayXd& x0, Eigen::ArrayXd& y0,Eigen::ArrayXd& x1, Eigen::ArrayXd& y1);

		/// Reset the ranges to the ranges of two point set -- version for vectors
		void resetRanges(std::vector<double>& x0,std::vector<double>& y0,std::vector<double>& x1,std::vector<double>& y1);

		/// plot ranges
		double minX_, minY_, maxX_, maxY_;

};

class PolarPlotter : public Plotter {

	public:

		/// Constructor
		PolarPlotter();

		/// Destructor
		~PolarPlotter();

		// make a test plot
		void testPlot();

	private:

		/// Pi in radians
		double pi_;

};

#endif
