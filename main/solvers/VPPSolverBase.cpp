#include "VPPSolverBase.h"
#include "VPPException.h"
#include "Interpolator.h"
#include <fstream>
#include "mathUtils.h"
#include "VPPResultIO.h"

using namespace mathUtils;

//// VPPSolverBase class  //////////////////////////////////////////////
// Init static member
boost::shared_ptr<VPPItemFactory> VPPSolverBase::pVppItemsContainer_;
Eigen::VectorXd VPPSolverBase::xp0_((Eigen::VectorXd(4) << .5, 0., 0., 1.).finished());

// Constructor
VPPSolverBase::VPPSolverBase(boost::shared_ptr<VPPItemFactory> VPPItemFactory):
																				dimension_(xp0_.size()),
																				subPbSize_(2),
																				tol_(1.e-4) {

	// Init the STATIC member vppItemsContainer
	pVppItemsContainer_= VPPItemFactory;

	// Set the parser
	pParser_= pVppItemsContainer_->getParser();

	// Resize the vector with the initial guess/VPPSolverBase results
	xp_.resize(dimension_);

	// Also get a reference to the WindItem that has computed the
	// real wind velocity/angle for the current run
	pWind_= pVppItemsContainer_->getWind();

	// Init the ResultContainer that will be filled while running the results
	pResults_.reset(new ResultContainer(pWind_));

	// Instantiate a NRSolver that will be used to feed the VPPSolverBase with
	// an equilibrated first guess solution. The solver will solve a subproblem
	// without optimization variables
	nrSolver_.reset( new NRSolver(VPPItemFactory.get(),dimension_,subPbSize_) );

	// Instantiate a VPPGradient that will be used to compute the gradient
	// of the objective function : the vector [ du/du du/dPhi du/db du/df ]
	pGradient_.reset(new VPPGradient(xp0_,pVppItemsContainer_.get()) );

	// Resize the bound containers
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

// Disallowed default constructor
VPPSolverBase::VPPSolverBase():
						dimension_(xp0_.size()),
						subPbSize_(2),
						tol_(1.e-3),
						pParser_(0),
						pWind_(0){
}

// Destructor
VPPSolverBase::~VPPSolverBase() {
	// make nothing
}

// Reset the optimizer when reloading the initial data
void VPPSolverBase::reset(boost::shared_ptr<VPPItemFactory> VPPItemFactory) {

	// Init the STATIC member vppItemsContainer
	pVppItemsContainer_= VPPItemFactory;

	// Set the parser
	pParser_= pVppItemsContainer_->getParser();

	// Also get a reference to the WindItem that has computed the
	// real wind velocity/angle for the current run
	pWind_=pVppItemsContainer_->getWind();

	// Init the ResultContainer that will be filled while running the results
	pResults_.reset(new ResultContainer(pWind_));

}

// Set the initial guess for the state variable vector
void VPPSolverBase::resetInitialGuess(int TWV, int TWA) {

	// In it to something small to start the evals at each velocity
	if(TWV==0) {

		// This is the very first solution, so we must guess a solution
		// but have nothing to establish our guess
		if(TWA==0)
			xp_= xp0_;

		else

			if(!pResults_->get(TWV,TWA-1).discard())// In this case we have a solution at a previous angle we can use
				// to set the guess
				xp_ = *(pResults_->get(TWV,TWA-1).getX());
			else
				xp_= xp0_;

	}
	else if( TWV==1 ) {

		///////////////////

		// IF twv==1 and twa==0, just take the solution of twv-1, 0
		// if this is acceptable. Otherwise restart from x0
		if(TWA==0){
			if(!pResults_->get(TWV-1,TWA).discard())
				xp_= *(pResults_->get(TWV-1,TWA).getX());
			else
				xp_= xp0_;
		}
		// twv_==1 and twa_ > 0. Do as for twv_==0 : use the previous converged solution
		else {

			// If we have a result at the same speed and the previous angle, this should be the closest
			// guess
			if(!pResults_->get(TWV,TWA-1).discard())// In this case we have a solution at a previous angle we can use
				xp_ = *(pResults_->get(TWV,TWA-1).getX());

			// ...otherwise, use the previous solution for the same angle
			else if(!pResults_->get(TWV-1,TWA).discard())// In this case we have a solution at a previous angle we can use
				xp_ = *(pResults_->get(TWV-1,TWA).getX());

			// ... If we have really nothing, just init with the first guess
			else
				xp_= xp0_;
		}
		///////////////////
	}
	else if( TWV>1 ) {

		// For twv> 1 we can linearly predict the result of the state vector
		// we search for the last two converged results and we do discard the
		// diverged results, that would not serve our cause
		// Enclose by try-catch block in case getPreviousConverged does not find
		// a valid solution to be used for the linear guess
		try {

			size_t tmOne=getPreviousConverged(TWV,TWA);
			size_t tmTwo=getPreviousConverged(tmOne,TWA);

			Extrapolator extrapolator(
					pResults_->get(tmTwo,TWA).getTWV(),
					pResults_->get(tmTwo,TWA).getX(),
					pResults_->get(tmOne,TWA).getTWV(),
					pResults_->get(tmOne,TWA).getX()
			);

			// Extrapolate the state vector for the current wind
			// velocity. Note that the items have not been init yet
			xp_= extrapolator.get( pWind_->getTWV(TWV) );

		} catch( NoPreviousConvergedException& e){
			// do nothing
		}

		// Make sure the initial guess does not exceeds the bounds
		for(size_t i=0; i<lowerBounds_.size(); i++) {
			if(xp_[i]<lowerBounds_[i]){
				std::cout<<"Resetting xp_["<<i<<"]= "<< xp_[i]<<" to the lower bound "<<lowerBounds_[i]<<std::endl;
				xp_[i]=lowerBounds_[i];
			}
			if(xp_[i]>upperBounds_[i]){
				std::cout<<"Resetting xp_["<<i<<"]= "<< xp_[i]<<" to the upper bound "<<upperBounds_[i]<<std::endl;
				xp_[i]=upperBounds_[i];
			}
		}
	}

	//std::cout<<"-->> solver first guess: "<<xp_.transpose()<<std::endl;

}

// Returns the index of the previous velocity-wise (twv) result that is marked as
// converged (discard==false). It starts from 'current', so it can be used recursively
size_t VPPSolverBase::getPreviousConverged(size_t idx, size_t TWA) {

	while(idx){
		idx--;
		if(!pResults_->get(idx,TWA).discard())
			return idx;
	}

	throw NoPreviousConvergedException(HERE,"No previous converged index found");
}

// Ask the NRSolver to solve a sub-problem without the optimization variables
// this makes the initial guess an equilibrated solution
void VPPSolverBase::solveInitialGuess(int TWV, int TWA) {

	// Get
	xp_.block(0,0,2,1)= nrSolver_->run(TWV,TWA,xp_).block(0,0,2,1);

	// Make sure the initial guess does not exceeds the bounds
	for(size_t i=0; i<subPbSize_; i++) {
		if(xp_[i]<lowerBounds_[i]){
			std::cout<<"WARNING: Modifying initial guess for x["<<i<<"]  "<<xp_[i]<<" to "<<lowerBounds_[i]<<std::endl;
			xp_[i]=lowerBounds_[i];
		}
		if(xp_[i]>upperBounds_[i]){
			std::cout<<"WARNING: Modifying initial guess for x["<<i<<"]  "<<xp_[i]<<" to "<<upperBounds_[i]<<std::endl;
			xp_[i]=upperBounds_[i];
		}
	}

}

// Returns the state vector for a given wind configuration
const Eigen::VectorXd VPPSolverBase::getResult(int TWV, int TWA) {
	return *(pResults_->get(TWV,TWA).getX());
}


// Make a printout of the results for this run
void VPPSolverBase::printResults() {

	pResults_->print();

}


// Save the current results to file
void VPPSolverBase::saveResults(string fileName) {

	VPPResultIO writer(pParser_, pResults_.get());
	writer.write(fileName);

}

// Read results from file and places them in the current results
void VPPSolverBase::importResults(string fileName) {

	VPPResultIO reader(pParser_,pResults_.get());
	reader.parse(fileName);

}

// Make a printout of the result bounds for this run
void VPPSolverBase::printResultBounds() {

	pResults_->printBounds();

}

// Plot the polar plots for the state variables
void VPPSolverBase::plotPolars(MultiplePlotWidget* pMultiPlotWidget) {

	std::vector<VppPolarCustomPlotWidget*> chartVec= pResults_->plotPolars();

	// Assigns to a vector of shared_ptrs, so this won't leak because
	// the possession is taken by the MultiplePlotWidget
	pMultiPlotWidget->addChart( chartVec[0],0,0 );
	pMultiPlotWidget->addChart( chartVec[1],1,0 );
	pMultiPlotWidget->addChart( chartVec[2],0,1 );
	pMultiPlotWidget->addChart( chartVec[3],1,1 );

}

// Plot the XY results
void VPPSolverBase::plotXY(MultiplePlotWidget* pMultiPlotWidget, WindIndicesDialog& wd) {

	std::vector<VppXYCustomPlotWidget*> chartVec= pResults_->plotXY(wd);

	// Assigns to a vector of shared_ptrs, so this won't leak because
	// the possession is taken by the MultiplePlotWidget
	pMultiPlotWidget->addChart( chartVec[0],0,0 );
	pMultiPlotWidget->addChart( chartVec[1],1,0 );
	pMultiPlotWidget->addChart( chartVec[2],0,1 );
	pMultiPlotWidget->addChart( chartVec[3],1,1 );
	pMultiPlotWidget->addChart( chartVec[4],2,0 );

}

// Returns the dimensionality of this problem (the size of the state vector)
size_t VPPSolverBase::getDimension() const {
	return dimension_;
}

ResultContainer* VPPSolverBase::getResults() {
	return pResults_.get();
}





