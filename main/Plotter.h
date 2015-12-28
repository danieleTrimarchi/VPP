#ifndef PLOTTER_H
#define PLOTTER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

#include <Eigen/Core>
using namespace Eigen;

/// Directives for PLPLOT
#include "plcdemos.h"

/// Wrapper class for ploPlot
class Plotter {

	public:
		
		/// Constructor
		Plotter(int nValues=101);
		
		/// Destructor
		virtual ~Plotter();
		
		/// Produce a 2d plot from Eigen vectors
		virtual void plot();

	private:

		/// Compute test values for the XY plot given the bounds
		void setTestParabolicFcn(double xmin, double xmax, double ymin, double ymax);

	protected:

		/// Find the min of the specified c-style array
		double min(double*);

		/// Find the max of the specified c-style array
		double max(double*);

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

};

class PolarPlotter : public Plotter {

	public:

		/// Constructor
		PolarPlotter();

		/// Destructor
		~PolarPlotter();

		/// Implement the abstract method plot
		virtual void plot();

	private:

		/// Pi in radians
		double pi_;

};

#endif
