#include "VPPSolver.h"

#include "VPPException.h"
#include <fstream>
#include "mathUtils.h"
#include "VPPPlotSet.h"
#include "VPPResultIO.h"

using namespace mathUtils;

namespace Optim {

//// VPPSolver class  //////////////////////////////////////////////

// Constructor
VPPSolver::VPPSolver(boost::shared_ptr<VPPItemFactory> VPPItemFactory):
						VPPSolverBase(VPPItemFactory),
						fixedB_(0.),
						fixedF_(1.){

}

// Destructor
VPPSolver::~VPPSolver() {
	// make nothing
}

// Reset the VPPSolver when reloading the initial data
void VPPSolver::reset(boost::shared_ptr<VPPItemFactory> VPPItemFactory) {

	// Decorator for the mother class method reset
	VPPSolverBase::reset(VPPItemFactory);

}


// Set the initial guess for the state variable vector
void VPPSolver::resetInitialGuess(int TWV, int TWA) {

	// Call the parent class that will guess a solution for us...
	VPPSolverBase::resetInitialGuess(TWV,TWA);

	// Make sure the values of the constrained values is correct
	xp_[2]= fixedB_;
	xp_[3]= fixedF_;

	std::cout<<"-->> VPPSolver first guess: "<<xp_.transpose()<<std::endl;

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
	Eigen::VectorXd residuals= pVppItemsContainer_->getResiduals();
	printf("      residuals: dF= %g, dM= %g\n\n",residuals(0),residuals(1) );

	// Push the result to the result container
	pResults_->push_back(TWV, TWA, xp_, residuals(0), residuals(1) );

	// Make sure the result does not exceeds the bounds
	for(size_t i=0; i<subPbSize_; i++)
		if(xp_[i]<lowerBounds_[i] || xp_[i]>upperBounds_[i]){
			std::cout<<"WARNING: VPPSolver result for tWv="<<TWV<<" and tWa="<<TWA<<" is out-of-bounds for variable "<<i<<std::endl;
			pResults_->remove(TWV, TWA);
		}

}


} // End namespace VPPSolver
