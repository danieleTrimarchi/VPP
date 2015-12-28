#include "Optimizer.h"

// Constructor
Optimizer::Optimizer() {
	// make nothing
}

// Function required for example1. Set the objective function
double Optimizer::myfunc(unsigned n, const double *x, double *grad, void *my_func_data) {

    if (grad) {
        grad[0] = 0.0;
        grad[1] = 0.5 / sqrt(x[1]);
    }
    return sqrt(x[1]);
}

// Function requested for example1. Set the constraint function
double Optimizer::myconstraint(unsigned n, const double *x, double *grad, void *data) {

	my_constraint_data *d = (my_constraint_data *) data;
    double a = d->a, b = d->b;
    if (grad) {
        grad[0] = 3 * a * (a*x[0] + b) * (a*x[0] + b);
        grad[1] = -1.0;
    }
    return ((a*x[0] + b) * (a*x[0] + b) * (a*x[0] + b) - x[1]);
}

// Destructor
Optimizer::~Optimizer() {
	// make nothing
}

// Execute example 1
void Optimizer::run() {

  double lb[2] = { -HUGE_VAL, 0 }; /* lower bounds */
  nlopt_opt opt;
  opt = nlopt_create(NLOPT_LD_MMA, 2); /* algorithm and dimensionality */
  nlopt_set_lower_bounds(opt, lb);
  nlopt_set_min_objective(opt, myfunc, NULL);
  my_constraint_data data[2] = { {2,0}, {-1,1} };
  nlopt_add_inequality_constraint(opt, myconstraint, &data[0], 1e-8);
  nlopt_add_inequality_constraint(opt, myconstraint, &data[1], 1e-8);
  nlopt_set_xtol_rel(opt, 1e-4);
  double xp[2] = { 1.234, 5.678 };  /* some initial guess */
  double minf; /* the minimum objective value, upon return */
  if (nlopt_optimize(opt, xp, &minf) < 0) {
      printf("nlopt failed!\n");
  }
  else {
      printf("found minimum at f(%g,%g) = %0.10g\n", xp[0], xp[1], minf);
  }
  nlopt_destroy(opt);

}
