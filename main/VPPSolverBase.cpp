#include "VPPSolverBase.h"
#include "VPPException.h"
#include "Interpolator.h"
#include <fstream>
#include "mathUtils.h"
#include "VPPPlotSet.h"
#include "VPPResultIO.h"

using namespace mathUtils;

//// VPPSolverBase class  //////////////////////////////////////////////
// Init static member
boost::shared_ptr<VPPItemFactory> VPPSolverBase::vppItemsContainer_;
Eigen::VectorXd VPPSolverBase::xp0_((Eigen::VectorXd(4) << .5, 0., 0., 1.).finished());

// Constructor
VPPSolverBase::VPPSolverBase(boost::shared_ptr<VPPItemFactory> VPPItemFactory):
																dimension_(4),
																subPbSize_(2),
																tol_(1.e-4) {

	// Instantiate a NRSolver that will be used to feed the VPPSolverBase with
	// an equilibrated first guess solution. The solver will solve a subproblem
	// without optimization variables
	nrSolver_.reset( new NRSolver(VPPItemFactory.get(),dimension_,subPbSize_) );

	// Init the STATIC member vppItemsContainer
	vppItemsContainer_= VPPItemFactory;

	// Set the parser
	pParser_= vppItemsContainer_->getParser();

	// Resize the vector with the initial guess/VPPSolverBase results
	xp_.resize(dimension_);

	// Also get a reference to the WindItem that has computed the
	// real wind velocity/angle for the current run
	pWind_= vppItemsContainer_->getWind();

	// Init the ResultContainer that will be filled while running the results
	pResults_.reset(new ResultContainer(pWind_));

}

// Disallowed default constructor
VPPSolverBase::VPPSolverBase():
		dimension_(4),
		subPbSize_(2),
		tol_(1.e-3),
		pParser_(0),
		pWind_(0){
}

// Destructor
VPPSolverBase::~VPPSolverBase() {
	// make nothing
}

void VPPSolverBase::reset(boost::shared_ptr<VPPItemFactory> VPPItemFactory) {

	// Init the STATIC member vppItemsContainer
	vppItemsContainer_= VPPItemFactory;

	// Set the parser
	pParser_= vppItemsContainer_->getParser();

	// Also get a reference to the WindItem that has computed the
	// real wind velocity/angle for the current run
	pWind_=vppItemsContainer_->getWind();

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

	else if( TWV>1 ) {

		// For twv> 1 we can linearly predict the result of the state vector
		// we search for the last two converged results and we do discard the
		// diverged results, that would not serve our cause
		// Enclose by try-catch block in case getPreviousConverged does not find
		// a valid solution to be used for the linear guess
		try {

			std::cout<<"Print the result tvw, twa!"<<std::endl;
			pResults_->get(TWV,TWA).print();

			size_t tmOne=getPreviousConverged(TWV,TWA);

			std::cout<<"tmOne= "<<tmOne<<std::endl;
			std::cout<<"Print the result tmOne, twa!"<<std::endl;
			pResults_->get(tmOne,TWA).print();

			size_t tmTwo=getPreviousConverged(tmOne,TWA);

			std::cout<<"tmTwo= "<<tmTwo<<std::endl;
			std::cout<<"Print the result tmTwo, twa!"<<std::endl;
			pResults_->get(tmTwo,TWA).print();

			Extrapolator extrapolator(
					pResults_->get(tmTwo,TWA).getTWV(),
					pResults_->get(tmTwo,TWA).getX(),
					pResults_->get(tmOne,TWA).getTWV(),
					pResults_->get(tmOne,TWA).getX()
			);

			// Extrapolate the state vector for the current wind
			// velocity. Note that the items have not been init yet
			Eigen::VectorXd xp= extrapolator.get( pWind_->getTWV(TWV) );
			std::cout<<"Extrapolated solution: "<<xp.transpose()<<std::endl;
			// Do extrapolate ONLY if the velocity is increasing
			// This is beneficial to convergence
			//if(xp(0)>xp_(0))
			xp_=xp;

		} catch( NoPreviousConvergedException& e){
			// do nothing
		}

		// Make sure the initial guess does not exceeds the bounds
		for(size_t i=0; i<lowerBounds_.size(); i++) {
			if(xp_[i]<lowerBounds_[i]){
				std::cout<<"Resetting the lower bounds to "<<lowerBounds_[i]<<std::endl;
				xp_[i]=lowerBounds_[i];
			}
			if(xp_[i]>upperBounds_[i]){
				xp_[i]=upperBounds_[i];
				std::cout<<"Resetting the upper bounds to "<<upperBounds_[i]<<std::endl;
			}
		}
	}

	std::cout<<"-->> solver first guess: "<<xp_.transpose()<<std::endl;

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

// Returns the state vector for a given wind configuration
const Eigen::VectorXd VPPSolverBase::getResult(int TWV, int TWA) {
	return *(pResults_->get(TWV,TWA).getX());
}


// Make a printout of the results for this run
void VPPSolverBase::printResults() {

	std::cout<<"==== VPPSolverBase RESULTS PRINTOUT ==================="<<std::endl;
	pResults_->print();
	std::cout<<"---------------------------------------------------\n"<<std::endl;

}


// Save the current results to file
void VPPSolverBase::saveResults() {

	std::cout<<"==== VPPSolverBase RESULTS SAVING... ==================="<<std::endl;
	VPPResultIO writer(pResults_.get());
	writer.write();
	std::cout<<"---------------------------------------------------\n"<<std::endl;

}

// Read results from file and places them in the current results
void VPPSolverBase::importResults() {

	std::cout<<"==== VPPSolverBase RESULTS IMPORT... ==================="<<std::endl;
	VPPResultIO reader(pResults_.get());
	reader.read();
	std::cout<<"---------------------------------------------------\n"<<std::endl;

}



// Make a printout of the result bounds for this run
void VPPSolverBase::printResultBounds() {

	std::cout<<"==== VPPSolverBase RESULT BOUNDS PRINTOUT ==================="<<std::endl;
	pResults_->printBounds();
	std::cout<<"---------------------------------------------------\n"<<std::endl;

}

/// Make a printout of the results for this run
void VPPSolverBase::plotPolars() {

	// Instantiate a VPPPlotSet and sub-contract the plot
	VPPPlotSet plotSet(pResults_.get());
	plotSet.plotPolars();

}

/// Make a printout of the results for this run
void VPPSolverBase::plotXY(size_t iWa) {

	if( iWa>=pResults_->windAngleSize() ){
		std::cout<<"User requested a wrong index! \n";
		return;
	}

	// Ask the plotter manager to produce the plots given the
	// results. The plotter manager prepares the results (makes
	// sure to manage only valid results) and instantiates the
	// plotter to prepare the XY plot
		VPPPlotSet vppPlotSet(pResults_.get());
		vppPlotSet.plotXY(iWa);

}

// Add this method for compatibility with the NR solver.
// TODO dtrimarchi: this could go to a common parent class
void VPPSolverBase::plotJacobian() {
	nrSolver_->plotJacobian();
}




