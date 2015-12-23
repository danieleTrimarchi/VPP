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
		void plotPolar(VectorXd& theta, VectorXd val);

	private:

		void testParabolicFcn(double xmin, double xmax, double ymin, double ymax);

};

#endif
