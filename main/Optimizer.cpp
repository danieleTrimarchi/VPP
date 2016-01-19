#include "Optimizer.h"
#include "VPPException.h"

// Init static member
boost::shared_ptr<VPPItemFactory> Optimizer::vppItemsContainer_;
int optIterations=0;

// Constructor
Optimizer::Optimizer(boost::shared_ptr<VPPItemFactory> VPPItemFactory):
		dimension_(4) {

	// Init the static member vppItemsContainer
	vppItemsContainer_= VPPItemFactory;

	// Set the parser
	pParser_= vppItemsContainer_->getParser();

 	// Set the and apply the lower and the upper bounds
	// -> make sure the bounds are larger than the initial
	// 		guess!
	lowerBounds_.resize(dimension_);
	upperBounds_.resize(dimension_);

	lowerBounds_[0] = pParser_->get("V_MIN");   // Lower velocity
	upperBounds_[0] = pParser_->get("V_MAX"); ;	// Upper velocity
  lowerBounds_[1] = pParser_->get("PHI_MIN"); // Lower PHI
  upperBounds_[1] = pParser_->get("PHI_MAX"); // Upper PHI
  lowerBounds_[2] = pParser_->get("B_MIN"); ;	// lower reef
  upperBounds_[2] = pParser_->get("B_MAX"); ;	// upper reef
  lowerBounds_[3] = pParser_->get("F_MIN"); ;	// lower FLAT
  upperBounds_[3] = pParser_->get("F_MAX"); ;	// upper FLAT

}

// Destructor
Optimizer::~Optimizer() {
	// make nothing
}

// Set the objective function for tutorial g13
double Optimizer::VPP_speed(unsigned n, const double* x, double *grad, void *my_func_data) {

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

	// And compute the residuals for force and moment
	vppItemsContainer_->computeResiduals(result[0],result[1]);

}

void Optimizer::run(int TWV, int TWA) {

  // Instantiate a NLOpobject and set the ISRES "Improved Stochastic Ranking Evolution Strategy"
	// algorithm for nonlinearly-constrained global optimization
	//nlopt::opt opt(nlopt::GN_ISRES,dimension_);
	nlopt::opt opt(nlopt::LN_COBYLA,dimension_);

  // Set the bounds for the constraints
  opt.set_lower_bounds(lowerBounds_);
  opt.set_upper_bounds(upperBounds_);

  std::vector<double> tol(dimension_);
  tol[0]=tol[1]=1.e-8;

  // Drive the loop info to the struct
  Loop_data loopData={TWV,TWA};

  // Set the objective function to be minimized (or maximized, using set_max_objective)
  //opt.set_max_objective(VPP_speed, NULL);
  opt.set_max_objective(VPP_speed, NULL);

  // Make a ptr to the non static member function VPPconstraint
  opt.add_equality_mconstraint(VPPconstraint, &loopData, tol);

  // Set the relative tolerance
  opt.set_xtol_rel(1e-8);

  // Set the max number of evaluations
  opt.set_maxeval(5000);

  // Set some initial guess. Make sure it is within the
  // bounds that have been set
  std::vector<double> xp(dimension_);
  xp[0]= 1.;
  xp[1]= 0.;
  xp[2]= 0.5;
  xp[3]= 0.5;

  // Instantiate the maximum objective value, upon return
  double maxf;

  // Set an initial population of 1000 points
  opt.set_population(1000);

  // Launch the optimization; negative retVal implies failure
  nlopt::result result = opt.optimize(xp, maxf);

  if (result < 0) {
      printf("nlopt failed!\n");
  }
  else {
  		printf("found maximum after %d evaluations\n", optIterations);
  		printf("found maximum at f(%g,%g,%g,%g) = %0.10g\n",
     		 xp[0],xp[1],xp[2],xp[3],maxf);
  }
}






