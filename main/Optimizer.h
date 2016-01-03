#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include <nlopt.hpp>

using namespace std;

/// Wrapper class around NLOPT non-linear
/// optimization library
class Optimizer {

	public:

		/// Constructor
		Optimizer();

		/// Destructor
		~Optimizer();

		/// Execute example 1
		void runExample1();

		/// Execute example 2
		void runExample2();

		/// Execute
		void run();

	private:

		/// Function required for example1. Set the objective function
		static double myfunc(unsigned n, const double *x, double *grad, void *my_func_data);

		/// Function requested for example1. Set the constraint function
		static double myconstraint(unsigned n, const double *x, double *grad, void *data);

		/// Struct requested by example1. Coefficient for each constraint in the
		/// shape : y >= (ax+b)^3
		typedef struct {
		    double a, b;
		} my_constraint_data;

};

#endif
