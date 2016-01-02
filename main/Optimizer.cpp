#include "Optimizer.h"

// Constructor
Optimizer::Optimizer() {
	// make nothing
}

// number of iterations required for this run
int optIterations = 0;

// Set the objective function and gradient for example1:
// Find min : sqrt(x2)
// Subjected to y>0, y>=(a1*x + b1)^3 , y>=(a2*x +b2)^3
double Optimizer::myfunc(unsigned n, const double *x, double *grad, void *my_func_data) {

		// Increment the number of iterations for each call of the objective function
		++optIterations;

    if (grad) {
        grad[0] = 0.0;
        grad[1] = 0.5 / sqrt(x[1]);
    }
    return sqrt(x[1]);
}

// Set the constraint function and gradient for example1:
// Find min : sqrt(x2)
// Subjected to y>0, y>=(a1*x + b1)^3 , y>=(a2*x +b2)^3
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
void Optimizer::runExample1() {

  // Instantiate a NLOpobject and set the MMA (Method of Moving Asymptotes)
	// algorithm and the dimensionality.
  // This is a gradient-based local optimization including nonlinear inequality
  // constraints (but not equality constraints)
  nlopt::opt opt(nlopt::LD_MMA,2);

	// Set the and apply the lower lower bounds ( no need to apply the upper bounds,
  // which are inf by default
  std::vector<double> lb(2);
  lb[0] = -HUGE_VAL;
  lb[1] = 0;
  opt.set_lower_bounds(lb);

  // Set the objective function
  opt.set_min_objective(myfunc, NULL);

  // Set the coefficients for the constraint equations :
  // a1 = 2, b1=0, a2=-1, b2=1
  my_constraint_data data[2] = { {2,0}, {-1,1} };

  // Set the constraint equations
  opt.add_inequality_constraint(myconstraint, &data[0], 1e-8);
  opt.add_inequality_constraint(myconstraint, &data[1], 1e-8);

  // Set the relative tolerance
  opt.set_xtol_rel(1e-4);

  // Set some initial guess
  std::vector<double> xp(2);
  xp[0]= 1.234;
  xp[1]= 5.678;

  // Instantiate the minimum objective value, upon return
  double minf;

  // Launch the optimization; negative retVal implies failure
  nlopt::result result = opt.optimize(xp, minf);

  if (result < 0) {
      printf("nlopt failed!\n");
  }
  else {
  		printf("found minimum after %d evaluations\n", optIterations);
      printf("found minimum at f(%g,%g) = %0.10g\n", xp[0], xp[1], minf);
  }

}

// Execute example 2: COBYLA derivative-free algorithm
void Optimizer::run() {

  // Instantiate a NLOpobject and set the COBYLA (Constrained Optimization BY Linear Approximations)
 	// algorithm and the dimensionality.
  // This is a derivative-free local optimization with nonlinear inequality and equality constraints
	nlopt::opt opt(nlopt::LN_COBYLA,2);

 	// Set the and apply the lower lower bounds ( no need to apply the upper bounds,
   // which are inf by default
   std::vector<double> lb(2);
   lb[0] = -HUGE_VAL;
   lb[1] = 0;
   opt.set_lower_bounds(lb);

   // Set the objective function
   opt.set_min_objective(myfunc, NULL);

   // Set the coefficients for the constraint equations :
   // a1 = 2, b1=0, a2=-1, b2=1
   my_constraint_data data[2] = { {2,0}, {-1,1} };

   // Set the constraint equations
   opt.add_inequality_constraint(myconstraint, &data[0], 1e-8);
   opt.add_inequality_constraint(myconstraint, &data[1], 1e-8);

   // Set the relative tolerance
   opt.set_xtol_rel(1e-4);

   // Set some initial guess
   std::vector<double> xp(2);
   xp[0]= 1.234;
   xp[1]= 5.678;

   // Instantiate the minimum objective value, upon return
   double minf;

   // Launch the optimization; negative retVal implies failure
   nlopt::result result = opt.optimize(xp, minf);

   if (result < 0) {
       printf("nlopt failed!\n");
   }
   else {
   		printf("found minimum after %d evaluations\n", optIterations);
       printf("found minimum at f(%g,%g) = %0.10g\n", xp[0], xp[1], minf);
   }

}
