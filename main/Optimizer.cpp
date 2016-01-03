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
void Optimizer::runExample2() {

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

// Try with ISRES global optimization
void Optimizer::runISRES() {

	// Set the dimension of this problem
	size_t dimension=2;

  // Instantiate a NLOpobject and set the ISRES "Improved Stochastic Ranking Evolution Strategy"
	// algorithm for nonlinearly-constrained global optimization
	///GN_ISRES
	nlopt::opt opt(nlopt::GN_ISRES,dimension);

 	// Set the and apply the lower and the upper bounds for the constraints
	// -> make sure the bounds are larger than the initial
	// 		guess!
   std::vector<double> lb(2),ub(2);
   for(size_t i=0; i<2; i++){
  	 lb[i] = 0;
  	 ub[i] = 6.;
   }

   // Set the bounds for the constraints
   opt.set_lower_bounds(lb);
   opt.set_upper_bounds(ub);

   // Set the objective function to be minimized (or maximized, using set_max_objective)
   opt.set_min_objective(myfunc, NULL);

   // Set the coefficients for the constraint equations :
   // a1 = 2, b1=0, a2=-1, b2=1
   my_constraint_data data[2] = { {2,0}, {-1,1} };

   // Set the constraint equations
   opt.add_inequality_constraint(myconstraint, &data[0], 1e-8);
   opt.add_inequality_constraint(myconstraint, &data[1], 1e-8);

   // Set the relative tolerance
   opt.set_xtol_rel(1e-4);

   // Set some initial guess. Make sure it is within the
   // bounds that have been set
   std::vector<double> xp(dimension);
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

// Set the objective function and gradient for example1:
// Find min : sqrt(x2)
// Subjected to y>0, y>=(a1*x + b1)^3 , y>=(a2*x +b2)^3
double Optimizer::myfunc_g06(unsigned n, const double *x, double *grad, void *my_func_data) {

		// Increment the number of iterations for each call of the objective function
		++optIterations;

    return (std::pow((x[0]-10),3) + std::pow((x[1]-20),3) );

}

// Set the constraint function and gradient for example1:
// Set the inequality in the shape: (-1)^s1 (x1 + a)^2 + (-1)^s2 (x2 + b)^2 + c ≤ 0
double Optimizer::myconstraint_g06(unsigned n, const double *x, double *grad, void *data) {

	g06_constraint_data *d = (g06_constraint_data *) data;

	return (
					std::pow((-1),d->s1) * std::pow(x[0]+d->a,2) +
					std::pow((-1),d->s2) * std::pow(x[1]+d->b,2) +
					d->c
				);
}


void Optimizer::run() {

	// g06:
	// Test function: f(⃗x) = (x1 − 10)^3 + (x2 − 20)^3
	// Constraints: g1(x) : −(x1 −5)^2 − (x2 −5)^2 + 100 ≤ 0
	//					 		g2(x) :  (x1 −6)^2 + (x2 −5)^2 − 82.81 ≤ 0
	// where 13 ≤ x1 ≤ 100 and 0 ≤ x2 ≤ 100.
	// The optimum solution is x = (14.095, 0.84296)
	// where f(x) = −6961.81388

	// Set the dimension of this problem
	size_t dimension=3;

  // Instantiate a NLOpobject and set the ISRES "Improved Stochastic Ranking Evolution Strategy"
	// algorithm for nonlinearly-constrained global optimization
	///GN_ISRES
	nlopt::opt opt(nlopt::GN_ISRES,dimension);

 	// Set the and apply the lower and the upper bounds
	// -> make sure the bounds are larger than the initial
	// 		guess!

   std::vector<double> lb(dimension),ub(dimension);
  	 lb[0] = 13;
  	 ub[0] = 100;
  	 lb[1] = 0;
  	 ub[1] = 100;

   // Set the bounds for the constraints
   opt.set_lower_bounds(lb);
   opt.set_upper_bounds(ub);

   // Set the objective function to be minimized (or maximized, using set_max_objective)
   opt.set_min_objective(myfunc_g06, NULL);

   // Set the constraint equations
   g06_constraint_data data[2] = { {-1,-5,-1,-5,+100}, {1,-6,1,-5,-82.81} };

   opt.add_inequality_constraint(myconstraint_g06, &data[0], 1e-8);
   opt.add_inequality_constraint(myconstraint_g06, &data[1], 1e-8);

   // Set the relative tolerance
   opt.set_xtol_rel(1e-4);

   opt.set_maxeval(500000);

   // Set some initial guess. Make sure it is within the
   // bounds that have been set
   std::vector<double> xp(dimension);
   xp[0]= 14;
   xp[1]= 0.8;

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


	/////////////////////////////////////////
	// g04:
	// Test function : f(x) = 5.3578547 (x3)^2 + 0.8356891 x1 x5 +  37.293239 x1 - 40792.141
	// Subjected to :
	// g1(x) = 85.334407 + 0.0056858 x2 x5 + 0.0006262 x1 x4 − 0.0022053 x3 x5 − 92 ≤ 0
	// g2(x) = −85.334407 − 0.0056858 x2 x5 − 0.0006262 x1 x4 + 0.0022053 x3 x5 ≤ 0
	// g3(x) = 80.51249 + 0.0071317 x2 x5 + 0.0029955 x1 x2 + 0.0021813 (x3)^2 − 110 ≤ 0
	// g4(x) = −80.51249 − 0.0071317 x2 x5 − 0.0029955 x1 x2 − 0.0021813 (x3)^2 + 90 ≤ 0
	// g5(x) = 9.300961 + 0.0047026 x3 x5 + 0.0012547 x1 x3 + 0.0019085 x3 x4 − 25 ≤ 0
	// g6(x) = −9.300961 − 0.0047026 x3 x5 − 0.0012547 x1 x3 − 0.0019085 x3 x4 + 20 ≤ 0
	// where 78≤x1≤102 , 33≤x2≤45 and 27≤xi≤45 (i=3,4,5).
	// The optimum solution is x =(78, 33, 29.995256025682, 45, 36.775812905788)
	// where f(x) = −30665.539.

}


