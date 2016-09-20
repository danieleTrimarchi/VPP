#ifndef VPPPLOTSET_H
#define VPPPLOTSET_H

#include <stdio.h>
#include <iostream>
#include "VPPPlotter.h"
#include "Results.h"

using namespace std;

// The VPPPlotSet handles results and plots them in a
// standard format. Preparing the results includes making
// sure to manage only valid results. It instantiates the
// plotters required for a particular plotSet
class VPPPlotSet {

	public:

		/// Ctor
		VPPPlotSet( ResultContainer* );

		/// Dtor
		~VPPPlotSet();

		/// Make XY plots for a given angle
		void plotXY(size_t iWa);

	private:

		/// Ptr to the result container that stores the results to be plot
		ResultContainer* pResults_;

};

#endif
