#include "VPPSolver.h"

#include "VPPException.h"
#include <fstream>
#include "mathUtils.h"
#include "VPPPlotSet.h"
#include "VPPResultIO.h"

using namespace mathUtils;

namespace VPPSolve {

//// VPPSolver class  //////////////////////////////////////////////

// Constructor
VPPSolver::VPPSolver(boost::shared_ptr<VPPItemFactory> VPPItemFactory):
				VPPSolverBase(VPPItemFactory),
				 fixedB_(0.),
				 fixedF_(1.){

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
VPPSolver::~VPPSolver() {
	// make nothing
}

// Reset the VPPSolver when reloading the initial data
void VPPSolver::reset(boost::shared_ptr<VPPItemFactory> VPPItemFactory) {

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

}


// Set the initial guess for the state variable vector
void VPPSolver::resetInitialGuess(int TWV, int TWA) {

	// In it to something small to start the evals at each velocity
	if(TWV==0) {

		xp_(0)= 1.0;  	// V_0
		xp_(1)= 0.1;		// PHI_0
		xp_(2)= fixedB_;		// b_0
		xp_(3)= fixedF_;		// f_0

	}

	else if( TWV>1 ) {

		// For twv> 1 we can linearly predict the result of the state vector
		Extrapolator extrapolator(
				pResults_->get(TWV-2,TWA).getTWV(),
				pResults_->get(TWV-2,TWA).getX(),
				pResults_->get(TWV-1,TWA).getTWV(),
				pResults_->get(TWV-1,TWA).getX()
		);

		// Extrapolate the state vector for the current wind
		// velocity. Note that the items have not been init yet
		Eigen::VectorXd xp= extrapolator.get( pWind_->getTWV(TWV) );

		// Do extrapolate ONLY if the velocity is increasing
		// This is beneficial to convergence
		if(xp(0)>xp_(0))
			xp_=xp;

		// Make sure the initial guess does not exceeds the bounds
		for(size_t i=0; i<subPbSize_; i++) {
			if(xp_[i]<lowerBounds_[i])
				xp_[i]=lowerBounds_[i];
			if(xp_[i]>upperBounds_[i])
				xp_[i]=upperBounds_[i];
		}

		xp_[2]= fixedB_;
		xp_[3]= fixedF_;
	}

	std::cout<<"-->> VPPSolver first guess: "<<xp_.transpose()<<std::endl;

}

// Ask the NRVPPSolver to solve a sub-problem without the optimization variables
// this makes the initial guess an equilibrated solution
void VPPSolver::solveInitialGuess(int TWV, int TWA) {

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


// Execute a VPP-like analysis
void VPPSolver::run(int TWV, int TWA) {

	std::cout<<"    "<<pWind_->getTWV(TWV)<<"    "<<toDeg(pWind_->getTWA(TWA))<<std::endl;

	// For each wind velocity, reset the initial guess for the
	// state variable vector to zero
	resetInitialGuess(TWV,TWA);

	// Refine the initial guess solving a sub-problem with no optimization variables
	solveInitialGuess(TWV,TWA);

	// Do nothing else, the solution is already found
	Eigen::VectorXd residuals= vppItemsContainer_->getResiduals();
	printf("      residuals: dF= %g, dM= %g\n\n",residuals(0),residuals(1) );

	// Push the result to the result container
	pResults_->push_back(TWV, TWA, xp_, residuals(0), residuals(1) );

}


} // End namespace VPPSolver
