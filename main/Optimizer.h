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

		/// Execute an example running ISRES "Improved Stochastic Ranking Evolution Strategy"
		/// this is the global optimization algorithm with non-linear equality constraints we
		/// can use for the VPP
		void runISRES();

		/// Execute benchmark case g06
		void run_g06();

		/// Execute benchmark case g13 -> FAILS!
		void run_g13();

		void run();

	private:

		/// Function required for example1. Set the objective function
		static double myfunc(unsigned n, const double *x, double *grad, void *my_func_data);

		// Same but for benchmark g06
		static double myfunc_g06(unsigned n, const double *x, double *grad, void *my_func_data);

		// Same but for benchmark g11
		static double myfunc_g11(unsigned n, const double *x, double *grad, void *my_func_data);

		// Same but for benchmark g13
		static double myfunc_g13(unsigned n, const double *x, double *grad, void *my_func_data);

		/// Function requested for example1. Set the constraint function
		static double myconstraint(unsigned n, const double *x, double *grad, void *data);

		/// Function requested for benchmark g06
		static double myconstraint_g06(unsigned n, const double *x, double *grad, void *data);

		/// Function requested for benchmark g11
		static double myconstraint_g11(unsigned n, const double *x, double *grad, void *data);

		/// Function requested for benchmark g13
		static double myconstraint_h1_g13(unsigned n, const double *x, double *grad, void *data);
		static double myconstraint_h2_g13(unsigned n, const double *x, double *grad, void *data);
		static double myconstraint_h3_g13(unsigned n, const double *x, double *grad, void *data);

		/// Struct requested by example1. Coefficient for each constraint in the
		/// shape : y >= (ax+b)^3
		typedef struct {
		    double a, b;
		} my_constraint_data;

		/// Struct requested by benchmark g06
		typedef struct {
				int s1;
		    double a;
				int s2;
				double b, c;
		} g06_constraint_data;

};

#endif
