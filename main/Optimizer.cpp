#include "Optimizer.h"
#include "VPPException.h"
#include "Interpolator.h"
#include <fstream>
#include "mathUtils.h"
#include "VPPPlotSet.h"
#include "VPPResultIO.h"

using namespace mathUtils;

namespace Optim {

// Init static member
size_t Optimizer::maxIters_;
int optIterations=0;

//// Optimizer class  //////////////////////////////////////////////

// Constructor
Optimizer::Optimizer(boost::shared_ptr<VPPItemFactory> VPPItemFactory):
						VPPSolverBase(VPPItemFactory){

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
Optimizer::~Optimizer() {
	// make nothing
}

// Reset the Optimizer when reloading the initial data
void Optimizer::reset(boost::shared_ptr<VPPItemFactory> VPPItemFactory) {

	// Decorator for the mother class method reset
	VPPSolverBase::reset(VPPItemFactory);

	lowerBounds_[0] = pParser_->get("V_MIN");   // Lower velocity
	upperBounds_[0] = pParser_->get("V_MAX"); ;	// Upper velocity
	lowerBounds_[1] = pParser_->get("PHI_MIN"); // Lower PHI
	upperBounds_[1] = pParser_->get("PHI_MAX"); // Upper PHI
	lowerBounds_[2] = pParser_->get("B_MIN"); ;	// lower reef
	upperBounds_[2] = pParser_->get("B_MAX"); ;	// upper reef
	lowerBounds_[3] = pParser_->get("F_MIN"); ;	// lower FLAT
	upperBounds_[3] = pParser_->get("F_MAX"); ;	// upper FLAT

	// Set the bounds for the constraints
	opt_->set_lower_bounds(lowerBounds_);
	opt_->set_upper_bounds(upperBounds_);

}


// Returns the index of the previous velocity-wise (twv) result that is marked as
// converged (discard==false). It starts from 'current', so it can be used recursively
size_t Optimizer::getPreviousConverged(size_t idx, size_t TWA) {

	while(idx){
		idx--;
		if(!pResults_->get(idx,TWA).discard())
			return idx;
	}

	throw NoPreviousConvergedException(HERE,"No previous converged index found");
}


// Set the initial guess for the state variable vector
void Optimizer::resetInitialGuess(int TWV, int TWA) {

	// In it to something small to start the evals at each velocity
	if(TWV==0) {

		// This is the very first solution, so we must guess a solution
		// but we have nothing to establish our guess
		if(TWA==0){

			xp_(0)= .5;  	// V_0
			xp_(1)= 0.; 		// PHI_0
			xp_(2)= 0.0;		// b_0
			xp_(3)= 1.;		// f_0

		}
		else
			// In this case we have a solution at a previous angle we can use
			// to set the guess
			xp_ = *(pResults_->get(TWV,TWA-1).getX());

	}

	else if( TWV>1 ) {

		// For twv> 1 we can linearly predict the result of the state vector
		// we search for the last two converged results and we do discard the
		// diverged results, that would not serve our cause
		// Enclose by try-catch block in case getPreviousConverged does not find
		// a valid solution to be used for the linear guess
		try {

			size_t tmOne=getPreviousConverged(TWV-1,TWA);
			size_t tmTwo=getPreviousConverged(tmOne,TWA);

			Extrapolator extrapolator(
					pResults_->get(tmTwo,TWA).getTWV(),
					pResults_->get(tmTwo,TWA).getX(),
					pResults_->get(tmOne,TWA).getTWV(),
					pResults_->get(tmOne,TWA).getX()
			);

			// Extrapolate the state vector for the current wind
			// velocity. Note that the items have not been init yet
			Eigen::VectorXd xp= extrapolator.get( pWind_->getTWV(TWV) );

			// Do extrapolate ONLY if the velocity is increasing
			// This is beneficial to convergence
			if(xp(0)>xp_(0))
				xp_=xp;

		} catch( NoPreviousConvergedException& e){
			// do nothing
		}

		// Make sure the initial guess does not exceeds the bounds
		for(size_t i=0; i<dimension_; i++) {
			if(xp_[i]<lowerBounds_[i])
				xp_[i]=lowerBounds_[i];
			if(xp_[i]>upperBounds_[i])
				xp_[i]=upperBounds_[i];
		}
	}

	std::cout<<"-->> optimizer first guess: "<<xp_.transpose()<<std::endl;

}

// Ask the NRSolver to solve a sub-problem without the optimization variables
// this makes the initial guess an equilibrated solution
void Optimizer::solveInitialGuess(int TWV, int TWA) {

	// Get
	xp_.block(0,0,2,1)= nrSolver_->run(TWV,TWA,xp_).block(0,0,2,1);

	// Make sure the initial guess does not exceeds the bounds
	for(size_t i=0; i<subPbSize_; i++) {
		if(xp_[i]<lowerBounds_[i]){
			std::cout<<"WARNING: Modifying lower out-of-bounds initial guess for x["<<i<<"]"<<std::endl;
			xp_[i]=lowerBounds_[i];
		}
		if(xp_[i]>upperBounds_[i]){
			std::cout<<"WARNING: Modifying upper out-of-bounds initial guess for x["<<i<<"]"<<std::endl;
			xp_[i]=upperBounds_[i];
		}
	}

}

// Set the objective function for tutorial g13
double Optimizer::VPP_speed(unsigned n, const double* x, double *grad, void *my_func_data) {

	// Increment the number of iterations for each call of the objective function
	++optIterations;

	if(grad)
		throw VPPException(HERE,"VPP_speed can only be used for derivative-free algorithms!");

	if(mathUtils::isNotValid(x[0])) throw VPPException(HERE,"x[0] is NAN!");

	// Return x[0], or the velocity to be maximized
	return x[0];

}

// Set the constraint function for benchmark g13:
void Optimizer::VPPconstraint(unsigned m, double *result, unsigned n, const double* x, double* grad, void* loopData) {

	// Retrieve the loop data for this call with a c-style cast
	Loop_data* d = (Loop_data*)loopData;

	int twv= d-> twv_;
	int twa= d-> twa_;

	// Now call update on the VPPItem container
	vppItemsContainer_->update(twv,twa,x);

	// And compute the residuals for force and moment
	vppItemsContainer_->getResiduals(result[0],result[1]);

}

// Execute a VPP-like analysis
void Optimizer::run(int TWV, int TWA) {

	std::cout<<"    "<<pWind_->getTWV(TWV)<<"    "<<toDeg(pWind_->getTWA(TWA))<<std::endl;

	// Drive the loop info to the struct
	Loop_data loopData={TWV,TWA};

	// Reset the iteration counter
	optIterations=0;

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

	//while ( residuals.norm() > 0.00001 )
	for(size_t iRes=0; iRes<3; iRes++ ){

		try{

			// Launch the optimization; negative retVal implies failure
			std::cout<<"Entering the optimizer with: ";
			printf("%8.6f,%8.6f,%8.6f,%8.6f \n", xp_(0),xp_(1),xp_(2),xp_(3));
			// convert to standard vector
			std::vector<double> xp(xp_.rows());
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
		catch (std::exception& e) {

			// throw exceptions catched by NLOpt
			char msg[256];
			sprintf(msg,"%s\n",e.what());
			throw VPPException(HERE,msg);
		}
		catch (...) {
			throw VPPException(HERE,"nlopt unknown exception catched!\n");
		}


		printf("found maximum after %d evaluations\n", optIterations);
		printf("      at f(%g,%g,%g,%g)\n",
				xp_(0),xp_(1),xp_(2),xp_(3) );

		residuals= vppItemsContainer_->getResiduals();
		printf("      residuals: dF= %g, dM= %g\n\n",residuals(0),residuals(1) );

		if( residuals.norm() < 0.0001 )
			break;
	}

	// Push the result to the result container
	pResults_->push_back(TWV, TWA, xp_, residuals(0), residuals(1) );

	// Make sure the result does not exceeds the bounds
	for(size_t i=0; i<subPbSize_; i++)
		if(xp_[i]<lowerBounds_[i] || xp_[i]>upperBounds_[i]){
			std::cout<<"WARNING: Optimizer result for tWv="<<TWV<<" and tWa="<<TWA<<" is out-of-bounds for variable "<<i<<std::endl;
			pResults_->remove(TWV, TWA);
		}
}

} // End namespace Optim




