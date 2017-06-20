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

	pMultiPlotWidget->addChart( chartVec[0],0,0 );
	pMultiPlotWidget->addChart( chartVec[1],1,0 );
	pMultiPlotWidget->addChart( chartVec[2],0,1 );
	pMultiPlotWidget->addChart( chartVec[3],1,1 );

}

// Plot the XY results
void VPPSolverBase::plotXY(MultiplePlotWidget* pMultiPlotWidget) {

	std::vector<VppXYCustomPlotWidget*> chartVec= pResults_->plotXY();

	pMultiPlotWidget->addChart( chartVec[0],0,0 );
	pMultiPlotWidget->addChart( chartVec[1],1,0 );
	pMultiPlotWidget->addChart( chartVec[2],0,1 );
	pMultiPlotWidget->addChart( chartVec[3],1,1 );
	pMultiPlotWidget->addChart( chartVec[4],2,0 );

}

//// Make XY plots for a given angle
//void VPPPlotSet::plotXY(size_t iWa) {
//
//	// Get the number of valid results (discard==false)
//	size_t numValidResults = pResults_->getNumValidResultsForAngle(iWa);
//
//	if(!numValidResults){
//		std::cout<<"No valid results found for plotXY! \n";
//		return;
//	}
//
//	// Prepare the data for the plotter
//	Eigen::ArrayXd windSpeeds(numValidResults);
//	Eigen::ArrayXd boatVelocity(numValidResults);
//	Eigen::ArrayXd boatHeel(numValidResults);
//	Eigen::ArrayXd boatFlat(numValidResults);
//	Eigen::ArrayXd boatB(numValidResults);
//	Eigen::ArrayXd dF(numValidResults);
//	Eigen::ArrayXd dM(numValidResults);
//
//	// Loop on all results but only plot the valid ones
//	size_t idx=0;
//	for(size_t iWv=0; iWv<pResults_->windVelocitySize(); iWv++) {
//
//		if(!pResults_->get(iWv,iWa).discard()) {
//
//			windSpeeds(idx)  = pResults_->get(iWv,iWa).getTWV();
//			boatVelocity(idx)= pResults_->get(iWv,iWa).getX()->coeff(0);
//			boatHeel(idx)    = mathUtils::toDeg(pResults_->get(iWv,iWa).getX()->coeff(1));
//			boatB(idx)    	 = pResults_->get(iWv,iWa).getX()->coeff(2);
//			boatFlat(idx)    = pResults_->get(iWv,iWa).getX()->coeff(3);
//			dF(idx)          = pResults_->get(iWv,iWa).getdF();
//			dM(idx)          = pResults_->get(iWv,iWa).getdM();
//			idx++;
//
//		}
//	}
//
//	char title[256];
//	sprintf(title,"AWA= %4.2f", toDeg(pResults_->getWind()->getTWA(iWa)) );
//
//	// Instantiate a plotter for the velocity
//	VPPPlotter plotter;
//	string t=string("Boat Speed")+string(title);
//	plotter.plot(windSpeeds,boatVelocity,windSpeeds,boatVelocity,
//			t,"Wind Speed [m/s]","Boat Speed [m/s]");
//
//	// Instantiate a plotter for the heel
//	VPPPlotter plotter2;
//	string t2=string("Boat Heel")+string(title);
//	plotter2.plot(windSpeeds,boatHeel,windSpeeds,boatHeel,
//			t2,"Wind Speed [m/s]","Boat Heelº");
//
//	// Instantiate a plotter for the Flat
//	VPPPlotter plotter3;
//	string t3=string("Sail FLAT")+string(title);
//	plotter3.plot(windSpeeds,boatFlat,windSpeeds,boatFlat,
//			t3,"Wind Speed [m/s]","Sail FLAT [-]");
//
//	// Instantiate a plotter for the position of the movable crew B
//	VPPPlotter plotter4;
//	string t4=string("Crew position")+string(title);
//	plotter4.plot(windSpeeds,boatB,windSpeeds,boatB,
//			t4,"Wind Speed [m/s]","Position of the movable crew [m]");
//
//	// Instantiate a plotter for the residuals
//	VPPPlotter plotter5;
//	string t5=string("Residuals")+string(title);
//	plotter5.plot(windSpeeds,dF,windSpeeds,dM,
//			t5,"Wind Speed [m/s]","Residuals [N,N*m]");
//}
//

// Returns the dimensionality of this problem (the size of the state vector)
size_t VPPSolverBase::getDimension() const {
	return dimension_;
}

ResultContainer* VPPSolverBase::getResults() {
	return pResults_.get();
}





