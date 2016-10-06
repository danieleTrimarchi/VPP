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

// Constructor
VPPSolverBase::VPPSolverBase(boost::shared_ptr<VPPItemFactory> VPPItemFactory):
																dimension_(4),
																subPbSize_(2),
																tol_(1.e-3) {

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




