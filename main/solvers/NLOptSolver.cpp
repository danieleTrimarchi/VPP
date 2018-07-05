#include "NLOptSolver.h"

#include "VPPException.h"
#include "Interpolator.h"
#include <fstream>
#include "mathUtils.h"
#include "VPPResultIO.h"

using namespace mathUtils;

namespace Optim {

// Init static member
size_t NLOptSolver::maxIters_;
int NLOptSolver::optIterations_;

//// Optimizer class  //////////////////////////////////////////////

// Constructor
NLOptSolver::NLOptSolver(std::shared_ptr<VPPItemFactory> VPPItemFactory):
								VPPSolverBase(VPPItemFactory){

	// Instantiate a NLOpobject and set the COBYLA algorithm for
	// nonlinearly-constrained local optimization
	// opt_.reset( new nlopt::opt(nlopt::GN_ISRES,dimension_) );
	opt_.reset( new nlopt::opt(nlopt::LN_COBYLA,dimension_) );

	// Set the bounds for the constraints
	opt_->set_lower_bounds(lowerBounds_);
	opt_->set_upper_bounds(upperBounds_);

	// Set the objective function to be maximized (using set_max_objective)
	opt_->set_max_objective(VPP_speed, NULL);

	// Set the absolute tolerance on the state variables
	//	opt_->set_xtol_abs(tol_);
	opt_->set_xtol_rel(tol_);

	// Set the absolute tolerance on the function value
	//	opt_->set_ftol_abs(tol_);
	opt_->set_ftol_rel(tol_);

	// Set the max number of evaluations for a single run
	maxIters_= 4000;
	opt_->set_maxeval(maxIters_);

}

// Destructor
NLOptSolver::~NLOptSolver() {
	// make nothing
}

// Reset the Optimizer when reloading the initial data
void NLOptSolver::reset(std::shared_ptr<VPPItemFactory> VPPItemFactory) {

	// Decorator for the mother class method reset
	VPPSolverBase::reset(VPPItemFactory);

}

// Set the objective function for tutorial g13
double NLOptSolver::VPP_speed(unsigned n, const double* x, double *grad, void *my_func_data) {

	// Increment the number of iterations for each call of the objective function
	++optIterations_;

	if(grad)
		throw VPPException(HERE,"VPP_speed can only be used for derivative-free algorithms!");

	if(mathUtils::isNotValid(x[0])) throw VPPException(HERE,"x[0] is NAN!");

	// Return x[0], or the velocity to be maximized
	return x[0];

}

// Set the constraint function for benchmark g13:
void NLOptSolver::VPPconstraint(unsigned m, double *result, unsigned n, const double* x, double* grad, void* loopData) {

	// Retrieve the loop data for this call with a c-style cast
	Loop_data* d = (Loop_data*)loopData;

	int twv= d-> twv_;
	int twa= d-> twa_;

	// Now call update on the VPPItem container
	pVppItemsContainer_->update(twv,twa,x);

	// And compute the residuals for force and moment
	pVppItemsContainer_->getResiduals(result[0],result[1]);

}

// Execute a VPP-like analysis
void NLOptSolver::run(int TWV, int TWA) {

	std::cout<<"    "<<pWind_->getTWV(TWV)<<"    "<<toDeg(pWind_->getTWA(TWA))<<std::endl;

	// Drive the loop info to the struct
	Loop_data loopData={TWV,TWA};

	// Reset the iteration counter
	optIterations_=0;

	// Note that the Number of constraints is determined by tol.size!!
	std::vector<double> tol(2);
	tol[0]=tol[1]=tol_;

	// Clear the optimizer
	opt_->remove_equality_constraints();
	opt_->remove_inequality_constraints();

	// For each wind velocity, reset the initial guess for the
	// state variable vector to zero
	resetInitialGuess(TWV,TWA);

	// Refine the initial guess solving a sub-problem with no optimization variables
	solveInitialGuess(TWV,TWA);

	// Make a ptr to the non static member function VPPconstraint
	opt_->add_equality_mconstraint(VPPconstraint, &loopData, tol);

	// Instantiate the maximum objective value, upon return
	double maxf;

	nlopt::result result;

	// Init an arbitrarily high residuals vector
	Eigen::VectorXd residuals(dimension_);
	for(size_t i=0; i<dimension_; i++) residuals(i)=100;

	// Declare a buffer vector xp. Declare it here so
	// that is available to the invalid_argument exception
	std::vector<double> xp(xp_.rows());

	try{

		// Launch the optimization; negative retVal implies failure
		std::cout<<"Entering the optimizer with: ";
		printf("%8.6f,%8.6f,%8.6f,%8.6f \n", xp_(0),xp_(1),xp_(2),xp_(3));
		// convert to standard vector
		for(size_t i=0; i<xp_.rows(); i++)
			xp[i]=xp_(i);

		// Launch the optimization
		result = opt_->optimize(xp, maxf);

		//store the results back to the member state vector
		for(size_t i=0; i<xp_.size(); i++)
			xp_(i)=xp[i];
	}
	catch( nlopt::roundoff_limited& e ){
		std::cout<<"Roundoff limited result"<<std::endl;
		// do nothing because the result of roundoff-limited exception
		// is meant to be still a meaningful result
	}
	catch(std::invalid_argument& e){
		std::cout<<"\nThe optimizer returned an invalid argument exception."<<std::endl;
		std::cout<<"This often happens if the specified initial guess exceeds the variable bounds."<<std::endl;
		std::cout<<"Initial guess: ";
		for(size_t i=0; i<xp.size(); i++) std::cout<<xp[i]<<", ";
		std::cout<<"\n\n";
		char msg[256];
		sprintf(msg,"%s",e.what());
		throw VPPException(HERE,msg);
	}
	catch (std::exception& e) {

		// throw exceptions catched by NLOpt
		char msg[256];
		sprintf(msg,"%s\n",e.what());
		throw VPPException(HERE,msg);
	}
	catch (...) {
		throw VPPException(HERE,"nlopt unknown exception catched!\n");
	}

	printf("found maximum after %d evaluations\n", optIterations_);
	printf("      at f(%g,%g,%g,%g)\n",
			xp_(0),xp_(1),xp_(2),xp_(3) );

	residuals= pVppItemsContainer_->getResiduals();
	printf("      residuals: dF= %g, dM= %g\n\n",residuals(0),residuals(1) );

	// Refine the solution from the optimizer with NR -> this is meant to fix the residuals
	solveInitialGuess(TWV,TWA);

	// Push the result to the result container and mark this as a converged result
	pResults_->push_back(TWV, TWA, xp_, residuals(0), residuals(1) );

	// Make sure the result does not exceeds the bounds
	for(size_t i=0; i<subPbSize_; i++)
		if(xp_[i]<lowerBounds_[i] || xp_[i]>upperBounds_[i] ||
				residuals.norm() > 100 ){
			std::cout<<"WARNING: Optimizer result for tWv="<<TWV<<" and tWa="<<TWA<<" is out-of-bounds for variable "<<i<<std::endl;
			pResults_->remove(TWV, TWA);
		}
}

} // End namespace Optim




