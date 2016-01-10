#include "Optimizer.h"

// Init static member
boost::shared_ptr<VPPItemFactory> Optimizer::vppItemsContainer_;
int optIterations=0;

// Constructor
Optimizer::Optimizer(boost::shared_ptr<VPPItemFactory> VPPItemFactory) {

	// Init the static member vppItemsContainer
	vppItemsContainer_=VPPItemFactory;
}

// Destructor
Optimizer::~Optimizer() {
	// make nothing
}

// Set the objective function for tutorial g13
double Optimizer::VPP_speed(unsigned n, const double *x, double *grad, void *my_func_data) {

		// Increment the number of iterations for each call of the objective function
		++optIterations;

		// Return x[0], or the velocity to be maximized
    return x[0];

}

// Set the constraint function for benchmark g13:
void Optimizer::VPPconstraint(unsigned m, double *result, unsigned n, const double* x, double* grad, void* loopData) {

	// Retrieve the loop data for this call with a c-style cast
	Loop_data* d = (Loop_data*)loopData;

	int twv= d-> twv;
	int twa= d-> twa;

	// Now call update on the VPPItem container
	vppItemsContainer_->update(twv,twa,x);

	// Compute the residuals for force and moment
	//	dF e dM devono essere iniettati in result,  array di taglia m (num of constraints, in questo caso 2)
	//

}

// Declare a ptr to a function
typedef void ( Optimizer::*FUNC ) (unsigned,double*,unsigned,const double*,double*,void*);

void Optimizer::run(int TWV, int TWA) {

	// Set the dimension of this problem (result vector size)
	size_t dimension=4; //  v, phi, reef, flat

  // Instantiate a NLOpobject and set the ISRES "Improved Stochastic Ranking Evolution Strategy"
	// algorithm for nonlinearly-constrained global optimization
	///GN_ISRES
	nlopt::opt opt(nlopt::GN_ISRES,dimension);

 	// Set the and apply the lower and the upper bounds
	// -> make sure the bounds are larger than the initial
	// 		guess!
  std::vector<double> lb(dimension),ub(dimension);
  lb[0] = 1;
  ub[0] = 10;
  lb[1] = 0;
  ub[1] = 80;
  lb[2] = 0;
  ub[2] = 0.3;
  lb[3] = 0.4;
  ub[3] = 1;

  // Set the bounds for the constraints
  opt.set_lower_bounds(lb);
  opt.set_upper_bounds(ub);

  std::vector<double> tol(dimension);
  tol[0]=tol[1]=1e-1;

  // Drive the loop info to the struct
  Loop_data loopData={TWV,TWA};

  // Set the objective function to be minimized (or maximized, using set_max_objective)
  //opt.set_max_objective(VPP_speed, NULL);
  opt.set_max_objective(VPP_speed, NULL);

  // Make a ptr to the non static member function VPPconstraint
  opt.add_equality_mconstraint(VPPconstraint, &loopData, tol);

  // Set the relative tolerance
  opt.set_xtol_rel(1e-1);

  // Set the max number of evaluations
  opt.set_maxeval(500000);

  // Set some initial guess. Make sure it is within the
  // bounds that have been set
  std::vector<double> xp(dimension);
  xp[0]= 0.7;
  xp[1]= 0.5;
  xp[2]= 0.5;
  xp[3]= 0.5;

  // Instantiate the minimum objective value, upon return
  double minf;

  // Set an initial population of 1000 points
  opt.set_population(1000);

  // Launch the optimization; negative retVal implies failure
  nlopt::result result = opt.optimize(xp, minf);

  if (result < 0) {
      printf("nlopt failed!\n");
  }
  else {
  		printf("found minimum after %d evaluations\n", optIterations);
  		printf("found minimum at f(%g,%g,%g,%g) = %0.10g\n",
     		 xp[0],xp[1],xp[2],xp[3],minf);
  }
}








