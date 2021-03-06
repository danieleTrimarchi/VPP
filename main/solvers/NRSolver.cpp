#include "NRSolver.h"
#include "VPPException.h"
#include "Interpolator.h"
#include <fstream>
#include <Eigen/Dense>
#include "mathUtils.h"
#include "VPPJacobian.h"

using namespace mathUtils;

//// NRSolver class  //////////////////////////////////////////////

// Constructor
NRSolver::NRSolver(VPPItemFactory* pVPPItemFactory,
		size_t dimension, size_t subPbSize ):
dimension_(dimension),
subPbSize_(subPbSize),
tol_(1.e-6),
maxIters_(100),
it_(0){

	// Resize the state vectors. Note that xp_ == xFull if the optimizer is
	// not used. If NR is the sub-problem solver for the otpimizer, xp_ is a
	// local subset of the full solution xFull, featuring optimization vars
	xp_.resize(dimension_);

	// Init the STATIC member vppItemsContainer
	pVppItemsContainer_= pVPPItemFactory;

	// Set the parser
	pParser_= pVppItemsContainer_->getParser();

	// Set the and apply the lower and the upper bounds
	// -> make sure the bounds are larger than the initial
	// 		guess!
	lowerBounds_.resize(dimension_);
	upperBounds_.resize(dimension_);

	lowerBounds_[0] = pParser_->get(Var::vBounds_.min_);   // Lower velocity
	upperBounds_[0] = pParser_->get(Var::vBounds_.max_); ;	// Upper velocity
	lowerBounds_[1] = pParser_->get(Var::heelBounds_.min_); // Lower PHI converted to rad
	upperBounds_[1] = pParser_->get(Var::heelBounds_.max_); // Upper PHI converted to rad

	// Also get a reference to the WindItem that has computed the
	// real wind velocity/angle for the current run
	pWind_=pVppItemsContainer_->getWind();

	// Init the ResultContainer that will be filled while running the results
	pResults_.reset(new ResultContainer(pWind_));

}

// Destructor
NRSolver::~NRSolver() {
	// make nothing
}

void NRSolver::reset(VPPItemFactory* pVPPItemFactory) {

	// Init the STATIC member vppItemsContainer
	pVppItemsContainer_= pVPPItemFactory;

	// Set the parser
	pParser_= pVppItemsContainer_->getParser();

	lowerBounds_[0] = pParser_->get(Var::vBounds_.min_);   // Lower velocity
	upperBounds_[0] = pParser_->get(Var::vBounds_.max_); ;	// Upper velocity
	lowerBounds_[1] = pParser_->get(Var::heelBounds_.min_); // Lower PHI in radians
	upperBounds_[1] = pParser_->get(Var::heelBounds_.max_); // Upper PHI in radians

	// Also get a reference to the WindItem that has computed the
	// real wind velocity/angle for the current run
	pWind_=pVppItemsContainer_->getWind();

	// Init the ResultContainer that will be filled while running the results
	pResults_.reset(new ResultContainer(pWind_));

	// Reset the iteration counter
	it_=0;
}

// This is similar to a reset, but it is used to change the subPbSize only.
// Used - for example - when computing the derivatives of the objective function
// in VPP_NLP::computederivative. In that case, the subPbSize is one for du/dphi
// but 2 for the other derivatives
void NRSolver::setSubPbSize(size_t subPbSize) {
	subPbSize_= subPbSize;
}

// The caller is the Optimizer, that resolves a larger problem with
// optimization variables. Here we get the initial guess of the optimizer
// and we try to solve a std sub-problem with no optimization vars
Eigen::VectorXd NRSolver::run(int twv, int twa, Eigen::VectorXd& xp ) {

	// Copy the optimizer solution into the local solution and to a local buffer, to be used
	// if the solver cannot converge
	xp_= xp;
	xpBuf_=xp;

	// now run std
	run(twv, twa);

	// Print the result to screen and save it to the result container
	printAndSave(twv, twa);

	return xp_;

}

void NRSolver::run(int twv, int twa) {

	std::cout.precision(15);

	// std::cout<<"    "<<pWind_->getTWV(twv)<<"    "<<toDeg( pWind_->getTWA(twa) )<<std::endl;
	// std::cout<<"\n Entering NR with first guess: "<<xp_.transpose()<<std::endl;

	try{
		// Launch the optimization; negative retVal implies failure

		std::vector<double> velocityResiduals;
		std::vector<double> PhiResiduals;

		// instantiate a Jacobian
		VPPJacobian J(xp_,pVppItemsContainer_,subPbSize_);

		// Newton loop
		for( it_=0; it_<=maxIters_; it_++ ) {

			// throw if the solution was not found within the max number of iterations
			if(it_==maxIters_){

// TODO dtrimarchi : shall we provide a way to automatically pop-up this plot on
// divergence..?
//				// Plot the velocity residuals and throw
//				VPPPlotter vResPlot;
//				vResPlot.plot(velocityResiduals,"V_residuals");
//
//				// Plot the angular residuals and throw
//				VPPPlotter phiResPlot;
//				phiResPlot.plot(PhiResiduals,"PHI_residuals");
//
//				// Also plot some Jacobian diagnostics
//				J.testPlot(twv,twa);

				std::cout<<"\n\nWARNING: NR-Solver could not converge. Please press a key to continue"<<std::endl;
				string s;
				std::cin>>s;

				// Restore the buffer and throw
				xp_=xpBuf_;

				throw NonConvergedException(HERE,"VPP Solver could not converge");

			}

			// Build a state vector with the size of the outer vector

			// Compute the residuals vector - here only the part relative to the subproblem
			Eigen::VectorXd residuals= pVppItemsContainer_->getResiduals(twv,twa,xp_);
			//std::cout<<"NR it: "<<it_<<", residuals= "<<residuals.transpose()<<"   \n";

			if(it_>1) {
				velocityResiduals.push_back( residuals(0) );
				PhiResiduals.push_back(residuals(1) );
			}

			//  break if converged. TODO dtrimarchi: this condition is way too simple!
			if( residuals.block(0,0,subPbSize_,1).norm()<1e-5 && it_>0 )
				break;

			// Compute the Jacobian matrix
			J.run(twv,twa);
			//std::cout<<"  in NRSolver: J= \n"<<J<<std::endl;

			// A * x = residuals --  J * deltas = residuals
			// where deltas are also equal to f(x_i) / f'(x_i)
			VectorXd deltas = J.colPivHouseholderQr().solve(residuals.block(0,0,subPbSize_,1));

			// compute the new state vector
			//  x_(i+1) = x_i - f(x_i) / f'(x_i)
			xp_.block(0,0,subPbSize_,1) -= deltas;

			//std::cout<<"  In NRSolver: xp_= "<<xp_.transpose()<<std::endl;

		}

//		string s;
//		std::cout<<"Please press a key to continue"<<std::endl;
//		while(cin >> s){
//			break;
//		}

	}
	catch(NonConvergedException& e ){
		std::cout<<"Catching NonConvergedexception in NRSolver..."<<std::endl;
		throw NonConvergedException( HERE,e.what() );
	}
	catch (std::exception& e) {
		throw VPPException(HERE,e.what());
	}
	catch (...) {
		throw VPPException(HERE,"Unknown exception catched!\n");
	}
}

// Print the result to screen and save it to the result container
void NRSolver::printAndSave(int twv, int twa) {

	// Get the residuals
	Eigen::VectorXd res= pVppItemsContainer_->getResiduals();

	// Print the solution
	//printf("\n found solution after %d iterations\n     at f(", it);
//	printf("NR solver solution for iTwv= %i iTwa=%i : ", twv,twa);
//	for(size_t i=0; i<xp_.size(); i++)
//		printf("%g  ",xp_(i));
//	printf("\n");
//
//	// Print the residuals
//	printf("     residuals: ");
//	for(size_t i=0; i<res.size(); i++)
//		printf("%g  ",res(i));
//	printf("\n\n");

	// Push the result to the result container. Hide from plotting if
	// out-of-bounds
	bool discard=false;
	for(size_t i=0; i<xp_.size(); i++){
		if( xp_(i)<lowerBounds_[i] || xp_(i)>upperBounds_[i] )
			discard=true;
	}

	pResults_->push_back(twv, twa, xp_, res, discard);

}

// Returns the current number of iterations for the last run
size_t NRSolver::getNumIters(){
	return it_;
}


// Make a printout of the results for this run
void NRSolver::printResults() {

	std::cout<<"==== NRSolver RESULTS PRINTOUT ==================="<<std::endl;
	pResults_->print();
	std::cout<<"---------------------------------------------------\n"<<std::endl;

}

// Make a printout of the result bounds for this run
void NRSolver::printResultBounds() {

	std::cout<<"==== NRSolver RESULT BOUNDS PRINTOUT ==================="<<std::endl;
	pResults_->printBounds();
	std::cout<<"---------------------------------------------------\n"<<std::endl;

}

//// Make a printout of the results for this run
//void NRSolver::plotXY(size_t iWa) {
//
//	if( iWa>=pResults_->windAngleSize() ){
//		std::cout<<"User requested a wrong index! \n";
//		return;
//	}
//
//	// Prepare the data for the plotter
//	std::vector<double> windSpeeds;
//	windSpeeds.reserve(pResults_->windVelocitySize());
//	std::vector<double> boatVelocity;
//	boatVelocity.reserve(pResults_->windVelocitySize());
//	std::vector<double> boatHeel;
//	boatHeel.reserve(pResults_->windVelocitySize());
//
//	std::vector<double> dF;
//	dF.reserve(pResults_->windVelocitySize());
//	std::vector<double> dM;
//	dM.reserve(pResults_->windVelocitySize());
//
//	for(size_t iWv=0; iWv<pResults_->windVelocitySize(); iWv++) {
//
//		if( !pResults_->get(iWv,iWa).discard() ){
//
//			windSpeeds.push_back( pResults_->get(iWv,iWa).getTWV() );
//			boatVelocity.push_back( pResults_->get(iWv,iWa).getX()->coeff(0) );
//			// Convert the heel results to degrees
//			boatHeel.push_back( mathUtils::toDeg( pResults_->get(iWv,iWa).getX()->coeff(1) ) );
//
//			dF.push_back( pResults_->get(iWv,iWa).getdF() );
//			dM.push_back( pResults_->get(iWv,iWa).getdM() );
//		}
//	}
//
//	char title[256];
//	sprintf(title,"AWA= %4.2f", mathUtils::toDeg( pWind_->getTWA(iWa) ) );
//
//	// Instantiate a plotter for the velocity
//	VPPPlotter plotter;
//	string t=string("Boat Speed")+string(title);
//	plotter.plot(windSpeeds,boatVelocity,windSpeeds,boatVelocity,
//			t,"Wind Speed [m/s]","Boat Speed [m/s]");
//
//
//	// Instantiate a plotter for the heel
//	VPPPlotter plotter2;
//	string t2=string("Boat Heel")+string(title);
//	plotter2.plot(windSpeeds,boatHeel,windSpeeds,boatHeel,
//			t2,"Wind Speed [m/s]","Boat Heel [º]");
//
//	// Instantiate a plotter for the residuals
//	VPPPlotter plotter5;
//	string t5=string("dF and dM Residuals")+string(title);
//	plotter5.plot(windSpeeds,dF,windSpeeds,dM,
//			t5,"Wind Speed [m/s]","Residuals [N,N*m]");
//
//}






