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
NRSolver::NRSolver(boost::shared_ptr<VPPItemFactory> VPPItemFactory):
dimension_(4),
tol_(1.e-6),
maxIters_(200){

	// Resize the state vectors. Note that xp_ == xFull if the optimizer is
	// not used. If NR is the sub-problem solver for the otpimizer, xp_ is a
	// local subset of the full solution xFull, featuring optimization vars
	xp_.resize(dimension_);

	// Init the STATIC member vppItemsContainer
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
	lowerBounds_[1] = pParser_->get("PHI_MIN"); // Lower PHI converted to rad
	upperBounds_[1] = pParser_->get("PHI_MAX"); // Upper PHI converted to rad

	// Also get a reference to the WindItem that has computed the
	// real wind velocity/angle for the current run
	pWind_=vppItemsContainer_->getWind();

	// Init the ResultContainer that will be filled while running the results
	pResults_.reset(new ResultContainer(pWind_));

}

// Destructor
NRSolver::~NRSolver() {
	// make nothing
}

void NRSolver::reset(boost::shared_ptr<VPPItemFactory> VPPItemFactory) {

	// Init the STATIC member vppItemsContainer
	vppItemsContainer_= VPPItemFactory;

	// Set the parser
	pParser_= vppItemsContainer_->getParser();

	lowerBounds_[0] = pParser_->get("V_MIN");   // Lower velocity
	upperBounds_[0] = pParser_->get("V_MAX"); ;	// Upper velocity
	lowerBounds_[1] = pParser_->get("PHI_MIN"); // Lower PHI in radians
	upperBounds_[1] = pParser_->get("PHI_MAX"); // Upper PHI in radians

	// Also get a reference to the WindItem that has computed the
	// real wind velocity/angle for the current run
	pWind_=vppItemsContainer_->getWind();

	// Init the ResultContainer that will be filled while running the results
	pResults_.reset(new ResultContainer(pWind_));

}

// Set the initial guess for the state variable vector
void NRSolver::resetInitialGuess(int TWV, int TWA) {

	// In it to something small to start the evals at each velocity
	if(TWV==0) {

		//     V_0   PHI_0 b_0   f_0
		xp_ << 0.01, mathUtils::toRad(0.01), 0, 0;

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

		std::cout<<"-->>Extrapolated first guess: "<<xp_.transpose()<<std::endl;
	}
}

// The caller is the Optimizer, that resolves a larger problem with
// optimization variables. Here we get the initial guess of the optimizer
// and we try to solve a std sub-problem with no optimisation vars
void NRSolver::run(int twv, int twa, Eigen::VectorXd xp ) {

	// Copy the optimizer solution into the local solution
	xp_= xp;

	// now run std
	run(twv, twa);


}

void NRSolver::run(int twv, int twa) {

	std::cout<<"    "<<pWind_->getTWV(twv)<<"    "<<toDeg( pWind_->getTWA(twa) )<<std::endl;

	// For each wind velocity, reset the initial guess for the
	// state variable vector to zero
	resetInitialGuess(twv,twa);

	// declare an out-of-scope loop variable to report the number
	// of Newton's iterations
	int it=0;

	try{
		// Launch the optimization; negative retVal implies failure

		std::vector<double> velocityResiduals;
		std::vector<double> PhiResiduals;

		// instantiate a Jacobian
		VPPJacobian J(xp_,vppItemsContainer_);

		// Instantiate a JacobianChecker
		// JacobianChecker JCheck;

		// Newton loop
		for( it=0; it<=maxIters_; it++ ) {

			// throw if the solution was not found within the max number of iterations
			if(it==maxIters_){
				// plot the velocity residuals and throw
        Plotter plotter2;
				plotter2.plot(PhiResiduals,"PHI_residuals");

				// Also plot some Jacobian diagnostics
				J.testPlot(twv,twa);

				// And plot the JacobianChecker diagnostics
				// JCheck.testPlot();

				throw VPPException(HERE,"VPP Solver could not converge");
			}

			// Build a state vector with the size of the outer vector

			// Compute the residuals vector
			Eigen::VectorXd residuals= vppItemsContainer_->getResiduals(twv,twa,xp_);
			std::cout<<"NR it: "<<it<<", residuals= "<<residuals.transpose()<<std::endl;

			if(it>1) {
				velocityResiduals.push_back( residuals(0) );
				PhiResiduals.push_back(residuals(1) );
			}

			//  break if converged. todo dtrimarchi: this condition is way too simple!
			if( residuals.norm()<1e-5 )
				break;

			// Compute the Jacobian matrix
			J.run(twv,twa);
//			std::cout<<"J= \n"<<J<<std::endl;

			// Right before computing the solution, store the relevant data to the JacobianChecker
			//JCheck.push_back(J,xp_,residuals);

			// A * x = residuals --  J * deltas = residuals
			// where deltas are also equal to f(x_i) / f'(x_i)
			VectorXd deltas = J.colPivHouseholderQr().solve(residuals);

			// compute the new state vector
			//  x_(i+1) = x_i - f(x_i) / f'(x_i)
			xp_ -= deltas;

		}

		// And plot the JacobianChecker diagnostics
		//JCheck.testPlot();

//		string s;
//		std::cout<<"Please press a key to continue"<<std::endl;
//		while(cin >> s){
//			break;
//		}


	}
	catch (std::exception& e) {

		char msg[256];
		sprintf(msg,"%s\n",e.what());
		throw VPPException(HERE,msg);
	}
	catch (...) {
		throw VPPException(HERE,"Unknown exception catched!\n");
	}

	// Get the residuals
	Eigen::VectorXd res= vppItemsContainer_->getResiduals();

	// Print the solution
	printf("found solution after %d iterations\n     at f(", it);
	for(size_t i=0; i<xp_.size(); i++)
		printf("%g  ",xp_(i));

	// Print the residuals
	printf("\n     residuals: ");
	for(size_t i=0; i<res.size(); i++)
		printf("%g  ",res(i));
	printf("\n ");

	// Push the result to the result container. Hide from plotting if
	// out-of-bounds
	bool discard=false;
	for(size_t i=0; i<xp_.size(); i++){
		if( xp_(i)<lowerBounds_[i] || xp_(i)>upperBounds_[i] )
			discard=true;
	}

	pResults_->push_back(twv, twa, xp_, res, discard);

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

// Make a printout of the results for this run
void NRSolver::plotPolars() {

	// Instantiate the Polar Plotters for Boat velocity, Boat heel,
	// Sail flat, Crew B, dF and dM
	PolarPlotter boatSpeedPolarPlotter("Boat Speed Polar Plot [m/s]");
	PolarPlotter boatHeelPolarPlotter("Boat Heel Polar Plot [deg]");

	// Instantiate the list of wind angles that will serve
	// for each velocity
	std::vector<double> windAngles, boatVelocity, boatHeel;

	// Loop on the wind velocities
	for(size_t iWv=0; iWv<pResults_->windVelocitySize(); iWv++) {

		// Store the wind velocity as a label for this curve
		char windVelocityLabel[256];
		sprintf(windVelocityLabel,"%3.1f", pResults_->get(iWv,0).getTWV() );
		string wVLabel(windVelocityLabel);

		windAngles.clear();
		boatVelocity.clear();
		boatHeel.clear();

		// Loop on the wind angles
		for(size_t iWa=0; iWa<pResults_->windAngleSize(); iWa++) {

			if( !pResults_->get(iWv,iWa).discard() ){

				// Fill the list of wind angles in degrees
				windAngles.push_back( mathUtils::toDeg( pResults_->get(iWv,iWa).getTWA() ) );

				// Fill the list of boat speeds
				boatVelocity.push_back( pResults_->get(iWv,iWa).getX()->coeff(0) );

				// Fill the list of boat heel
				boatHeel.push_back( mathUtils::toDeg( pResults_->get(iWv,iWa).getX()->coeff(1) ) );

			}
		}

		// Append the angles-data to the relevant plotter
		boatSpeedPolarPlotter.append(wVLabel,windAngles,boatVelocity);
		boatHeelPolarPlotter.append(wVLabel,windAngles,boatHeel);

	}

	// Ask all plotters to plot
	boatSpeedPolarPlotter.plot();
	boatHeelPolarPlotter.plot(50);
}

// Make a printout of the results for this run
void NRSolver::plotXY(size_t iWa) {

	if( iWa>=pResults_->windAngleSize() ){
		std::cout<<"User requested a wrong index! \n";
		return;
	}

	// Prepare the data for the plotter
	std::vector<double> windSpeeds;
	windSpeeds.reserve(pResults_->windVelocitySize());
	std::vector<double> boatVelocity;
	boatVelocity.reserve(pResults_->windVelocitySize());
	std::vector<double> boatHeel;
	boatHeel.reserve(pResults_->windVelocitySize());

	std::vector<double> dF;
	dF.reserve(pResults_->windVelocitySize());
	std::vector<double> dM;
	dM.reserve(pResults_->windVelocitySize());

	for(size_t iWv=0; iWv<pResults_->windVelocitySize(); iWv++) {

		if( !pResults_->get(iWv,iWa).discard() ){

			windSpeeds.push_back( pResults_->get(iWv,iWa).getTWV() );
			boatVelocity.push_back( pResults_->get(iWv,iWa).getX()->coeff(0) );
			// Convert the heel results to degrees
			boatHeel.push_back( mathUtils::toDeg( pResults_->get(iWv,iWa).getX()->coeff(1) ) );

			dF.push_back( pResults_->get(iWv,iWa).getdF() );
			dM.push_back( pResults_->get(iWv,iWa).getdM() );
		}
	}

	char title[256];
	sprintf(title,"AWA= %4.2f", mathUtils::toDeg( pWind_->getTWA(iWa) ) );

	// Instantiate a plotter for the velocity
	Plotter plotter;
	string t=string("Boat Speed")+string(title);
	plotter.plot(windSpeeds,boatVelocity,windSpeeds,boatVelocity,
			t,"Wind Speed [m/s]","Boat Speed [m/s]");


	// Instantiate a plotter for the heel
	Plotter plotter2;
	string t2=string("Boat Heel")+string(title);
	plotter2.plot(windSpeeds,boatHeel,windSpeeds,boatHeel,
			t2,"Wind Speed [m/s]","Boat Heel [deg]");

	// Instantiate a plotter for the residuals
	Plotter plotter5;
	string t5=string("dF and dM Residuals")+string(title);
	plotter5.plot(windSpeeds,dF,windSpeeds,dM,
			t5,"Wind Speed [m/s]","Residuals [N,N*m]");

}

// Plot the Jacobian derivatives on a fixed interval of linearization
// points and for given awv, awa
void NRSolver::plotJacobian(){

	// Define a linearization point
	Eigen::VectorXd xp(4);
	xp << 0.01, 0.01;

	// Instantiate a Jacobian
	VPPJacobian J(xp,vppItemsContainer_);

	// ask the user which awv, awa
	// For which TWV, TWA shall we plot the aero forces/moments?
	size_t twv=0, twa=0;

	std::cout<<"--> Please enter the values of twv and twa for the aero forces plot: "<<std::endl;
	while(true){
	cin >> twv >> twa;
	std::cout<<"got: "<<twv<<" "<<twa<<std::endl;
	bool vFine= twv < vppItemsContainer_->getWind()->getWVSize();
	bool aFine= twa < vppItemsContainer_->getWind()->getWASize();
	if(!vFine)
		std::cout<<"the value of twv is out of range, max is: "<<vppItemsContainer_->getWind()->getWVSize()-1<<std::endl;
	if(!aFine)
		std::cout<<"the value of twa is out of range, max is: "<<vppItemsContainer_->getWind()->getWASize()-1<<std::endl;
	if(vFine&&aFine)
		break;
	}

	// call jacobian.testPlot
	J.testPlot(twv, twa);

}






