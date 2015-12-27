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
		Plotter(); 
		
		/// Destructor
		~Plotter(); 
		
		/// Produce a 2d plot from Eigen vectors
		void plotXY(); 

		/// Produce a 2d plot from Eigen vectors
		void plotPolar();

	private:

		/// Compute test values for the XY plot given the bounds
		void setTestParabolicFcn(double xmin, double xmax, double ymin, double ymax);

		/// Number of points the plot is made of
		int nValues_;

		/// Primitive arrays plplot is feed with
		double* x_, * y_;

};

#endif
