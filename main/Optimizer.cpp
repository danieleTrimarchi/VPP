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
					d->s1 * std::pow(x[0]+d->a,2) +
					d->s2 * std::pow(x[1]+d->b,2) +
					d->c
				);
}


void Optimizer::run_g06() {

	// g06:
	// Test function: f(x) = (x1 − 10)^3 + (x2 − 20)^3
	// Constraints: g1(x) : −(x1 −5)^2 − (x2 −5)^2 + 100 ≤ 0
	//					 		g2(x) :  (x1 −6)^2 + (x2 −5)^2 − 82.81 ≤ 0
	// where 13 ≤ x1 ≤ 100 and 0 ≤ x2 ≤ 100.
	// The optimum solution is x = (14.095, 0.84296)
	// where f(x) = −6961.81388

	// Set the dimension of this problem (result vector size)
	size_t dimension=2;

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

   // Set some initial guess. Make sure it is within the
   // bounds that have been set
   std::vector<double> xp(dimension);
   xp[0]= 13.5;
   xp[1]= 0.1;

   // Instantiate the minimum objective value, upon return
   double minf;

   // Launch the optimization; negative retVal implies failure
   nlopt::result result = opt.optimize(xp, minf);

   std::cout<<"ResVec= "<<std::endl;
   for(size_t i=0; i<xp.size(); i++){
  	 std::cout<<xp[i]<<" , ";
   }
   std::cout<<"\n";

   if (result < 0) {
       printf("nlopt failed!\n");
   }
   else {
   		printf("found minimum after %d evaluations\n", optIterations);
       printf("found minimum at f(%g,%g) = %0.10g\n", xp[0], xp[1], minf);
   }
}

// Set the objective function for tutorial g13
double Optimizer::myfunc_g13(unsigned n, const double *x, double *grad, void *my_func_data) {

		// Increment the number of iterations for each call of the objective function
		++optIterations;

		// f(x) = e^(x1 x2 x3 x4 x5 )
    return ( std::exp( (x[0]*x[1]*x[2]*x[3]*x[4]) ) );

}

// Set the constraint function for benchmark g13:
double Optimizer::myconstraint_h1_g13(unsigned n, const double *x, double *grad, void *data) {

	//	h1(x)= (x1)^2 +(x2)^2 +(x3)^2 +(x4)^2 +(x5)^2 −10 = 0
	return x[0]*x[0] + x[1]*x[1] + x[2]*x[2] + x[3]*x[3] + x[4]*x[4] - 10;
}

// Set the constraint function for benchmark g13:
double Optimizer::myconstraint_h2_g13(unsigned n, const double *x, double *grad, void *data) {

	//  h2(x) = x2 x3 − 5 x4 x5 = 0
	return x[1]*x[2] - 5 * x[3] * x[4];
}

// Set the constraint function for benchmark g13:
double Optimizer::myconstraint_h3_g13(unsigned n, const double *x, double *grad, void *data) {

	// h3(x) = (x1)^3 + (x2)^3 + 1 = 0
	return x[0]*x[0]*x[0] + x[1]*x[1]*x[1] + 1;
}

void Optimizer::run_g13() {

	// Benchmark case 13:
	// f(x) = e^(x1 x2 x3 x4 x5 )
	// Subjected to :
	//	h1(x)= (x1)^2 +(x2)^2 +(x3)^2 +(x4)^2 +(x5)^2 −10 = 0
	//  h2(x) = x2 x3 − 5 x4 x5 = 0
	//  h3(x) = (x1)^3 + (x2)^3 + 1 = 0

	//where −2.3 ≤ xi ≤ 2.3 (i = 1,2) and −3.2 ≤ xi ≤ 3.2 (i = 3,4,5).
	//The optimum solution is
	// x = (−1.717143, 1.595709, 1.827247, −0.7636413, −0.763645) where f(x) = 0.0539498

	// Set the dimension of this problem (result vector size)
	size_t dimension=5;

  // Instantiate a NLOpobject and set the ISRES "Improved Stochastic Ranking Evolution Strategy"
	// algorithm for nonlinearly-constrained global optimization
	///GN_ISRES
	nlopt::opt opt(nlopt::GN_ISRES,dimension);

 	// Set the and apply the lower and the upper bounds
	// -> make sure the bounds are larger than the initial
	// 		guess!
  std::vector<double> lb(dimension),ub(dimension);
  lb[0] = -2.31;
  ub[0] =  2.31;
  lb[1] = -2.31;
  ub[1] =  2.31;
  lb[2] = -3.21;
  ub[2] =  3.21;
  lb[3] = -3.21;
  ub[3] =  3.21;
  lb[4] = -3.21;
  ub[4] =  3.21;

   // Set the bounds for the constraints
   opt.set_lower_bounds(lb);
   opt.set_upper_bounds(ub);

   // Set the objective function to be minimized (or maximized, using set_max_objective)
   opt.set_min_objective(myfunc_g13, NULL);

   // Set the constraint equations
   opt.add_equality_constraint(myconstraint_h1_g13, NULL, 1e-5);
   opt.add_equality_constraint(myconstraint_h2_g13, NULL, 1e-5);
   opt.add_equality_constraint(myconstraint_h3_g13, NULL, 1e-5);

   // Set the relative tolerance
   opt.set_xtol_rel(1e-4);

   // Set the max number of evaluations
   opt.set_maxeval(100000);

   // Set some initial guess. Make sure it is within the
   // bounds that have been set
   std::vector<double> xp(dimension);
   double eps=.0;
   xp[0]= -1.717143+3*eps;
   xp[1]=  1.595709-eps;
   xp[2]=  1.827247-4*eps;
   xp[3]= -0.7636413+eps;
   xp[4]= -0.763645-2*eps;

   // Instantiate the minimum objective value, upon return
   double minf;

   // Launch the optimization; negative retVal implies failure
   nlopt::result result = opt.optimize(xp, minf);

   if (result < 0) {
       printf("nlopt failed!\n");
   }
   else {
   		printf("found minimum after %d evaluations\n", optIterations);
       printf("found minimum at f(%g,%g,%g,%g,%g) = %0.10g\n",
      		 xp[0],xp[1],xp[2],xp[3],xp[4],minf);
   }

}

// Set the objective function for tutorial g13
double Optimizer::myfunc_g11(unsigned n, const double *x, double *grad, void *my_func_data) {

		// Increment the number of iterations for each call of the objective function
		++optIterations;

		// f(x) = (x1)^2 + ( x2 - 1 )^2
    return ( x[0]*x[0] + (x[1]-1)*(x[1]-1) );

}

// Set the constraint function for benchmark g13:
double Optimizer::myconstraint_g11(unsigned n, const double *x, double *grad, void *data) {

	//	h(x)= x2 - (x1)^2 = 0
	return x[1] - x[0] * x[0];
}

void Optimizer::run() {

	// Benchmark case 11:
	// f(x) = (x1)^2 + ( x2 - 1 )^2
	// Subjected to :
	//	h(x)= x2 - (x1)^2 = 0

	//where −1 ≤ x1 ≤ 1 and −1 ≤ x2 ≤ 1
	//The optimum solution is
	// x = (±1/sqrt(2),1/2) where f(x) = 0.75

	// Set the dimension of this problem (result vector size)
	size_t dimension=2;

  // Instantiate a NLOpobject and set the ISRES "Improved Stochastic Ranking Evolution Strategy"
	// algorithm for nonlinearly-constrained global optimization
	///GN_ISRES
	nlopt::opt opt(nlopt::GN_ISRES,dimension);

 	// Set the and apply the lower and the upper bounds
	// -> make sure the bounds are larger than the initial
	// 		guess!
  std::vector<double> lb(dimension),ub(dimension);
  lb[0] = 0;
  ub[0] = 1;
  lb[1] = 0;
  ub[1] = 1;

   // Set the bounds for the constraints
   opt.set_lower_bounds(lb);
   opt.set_upper_bounds(ub);

   // Set the objective function to be minimized (or maximized, using set_max_objective)
   opt.set_min_objective(myfunc_g11, NULL);

   my_constraint_data data = {2,0};

   // Set the constraint equations
   opt.add_equality_constraint(myconstraint_g11, NULL, 1e-1);

   // Set the relative tolerance
   opt.set_xtol_rel(1e-1);

   // Set the max number of evaluations
   opt.set_maxeval(500000);

   // Set some initial guess. Make sure it is within the
   // bounds that have been set
   std::vector<double> xp(dimension);
   double eps=.0;
   xp[0]= 0.7;
   xp[1]= 0.5;

   // Instantiate the minimum objective value, upon return
   double minf;

   // Launch the optimization; negative retVal implies failure
   nlopt::result result = opt.optimize(xp, minf);

   if (result < 0) {
       printf("nlopt failed!\n");
   }
   else {
   		printf("found minimum after %d evaluations\n", optIterations);
   		printf("found minimum at f(%g,%g) = %0.10g\n",
      		 xp[0],xp[1],minf);
   }


}










