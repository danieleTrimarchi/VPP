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
// TORESTORE
//dimension_(4),
dimension_(2),
tol_(1.e-6),
maxIters_(10){

	// Resize the result container
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
	lowerBounds_[1] = pParser_->get("PHI_MIN"); // Lower PHI
	upperBounds_[1] = pParser_->get("PHI_MAX"); // Upper PHI
	// TORESTORE
	//	lowerBounds_[2] = pParser_->get("B_MIN"); ;	// lower reef
	//	upperBounds_[2] = pParser_->get("B_MAX"); ;	// upper reef
	//	lowerBounds_[3] = pParser_->get("F_MIN"); ;	// lower FLAT
	//	upperBounds_[3] = pParser_->get("F_MAX"); ;	// upper FLAT

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
	lowerBounds_[1] = pParser_->get("PHI_MIN"); // Lower PHI
	upperBounds_[1] = pParser_->get("PHI_MAX"); // Upper PHI
	// TORESTORE
	//	lowerBounds_[2] = pParser_->get("B_MIN"); ;	// lower reef
	//	upperBounds_[2] = pParser_->get("B_MAX"); ;	// upper reef
	//	lowerBounds_[3] = pParser_->get("F_MIN"); ;	// lower FLAT
	//	upperBounds_[3] = pParser_->get("F_MAX"); ;	// upper FLAT

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
		xp_ << 0.01, 0.01;
		// TORESTORE
		//, 0.01, 0.99;

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
		xp_= extrapolator.get( pWind_->getTWV(TWV) );
		std::cout<<"-->>Extrapolated first giuess: "<<xp_.transpose()<<std::endl;
	}
	//
	//		// Make sure the initial guess does not exceeds the bounds
	//		for(size_t i=0; i<dimension_; i++) {
	//			if(xp_(i)<lowerBounds_[i])
	//				xp_(i)=lowerBounds_[i];
	//			if(xp_(i)>upperBounds_[i])
	//				xp_(i)=upperBounds_[i];
	//		}

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
		std::cout<<"------------------------------------------\n";
		std::cout<<"Entering the NRSolver with initial guess: "<<
				xp_[0]<<" "<<xp_[1]<<"\n";
		// TORESTORE
		//" "<<xp_[2]<<" "<<xp_[3]<<
		std::vector<double> velocityResiduals;
		std::vector<double> PhiResiduals;
		// TORESTORE
		//std::vector<double> c1Residuals;
		//std::vector<double> c2Residuals;

		// instantiate a Jacobian
		VPPJacobian J(xp_,vppItemsContainer_);

		// Instantiate a JacobianChecker
		JacobianChecker JCheck;

		// Newton loop
		for( it=0; it<=maxIters_; it++ ) {

			// throw if the solution was not found within the max number of iterations
			if(it==maxIters_){
				// plot the velocity residuals and throw
				Plotter plotter;
				plotter.plot(velocityResiduals,"V_residuals");
				Plotter plotter2;
				plotter2.plot(PhiResiduals,"PHI_residuals");
				// TORESTORE
				//				Plotter plotter3;
				//				plotter3.plot(c1Residuals,"c1_residuals");
				//				Plotter plotter4;
				//				plotter4.plot(c2Residuals,"c2_residuals");

				// Also plot some Jacobian diagnostics
				J.testPlot(twv,twa);

				// And plot the JacobianChecker diagnostics
				JCheck.testPlot();

				throw VPPException(HERE,"VPP Solver could not converge");
			}

			// Compute the residuals vector
			Eigen::VectorXd residuals= vppItemsContainer_->getResiduals(twv,twa,xp_);

			std::cout<<"NR it: "<<it<<", residuals= "<<residuals.transpose()<<std::endl;
			if(it>1) {
				velocityResiduals.push_back( residuals(0) );
				PhiResiduals.push_back(residuals(1) );
			}
			//			c1Residuals.push_back(residuals(2));
			//			c2Residuals.push_back(residuals(3));

			//  break if converged. todo dtrimarchi: this condition is way too simple!
			if( residuals.norm()<1e-6 )
				break;

			// Compute the Jacobian matrix
			J.run(twv,twa);
			std::cout<<"J= \n"<<J<<std::endl;

			// Right before computing the solution, store the relevant data to the JacobianChecker
			JCheck.push_back(J,xp_,residuals);

			// A * x = residuals --  J * deltas = residuals
			// where deltas are also equal to f(x_i) / f'(x_i)
			VectorXd deltas = J.colPivHouseholderQr().solve(residuals);

			// compute the new state vector
			//  x_(i+1) = x_i - f(x_i) / f'(x_i)
			xp_ -= deltas;
			std::cout<<"xp= "<<xp_.transpose()<<std::endl;

		}

	}
	catch (std::exception& e) {

		char msg[256];
		sprintf(msg,"%s\n",e.what());
		throw VPPException(HERE,msg);
	}
	catch (...) {
		throw VPPException(HERE,"Unknown exception catched!\n");
	}

	printf("found solution after %d iterations\n", it);
	// TORESTORE
	//	printf("      at f(%g,%g,%g,%g)",
	//			xp_(0),xp_(1),xp_(2),xp_(3));
	printf("      at f(%g,%g)",xp_(0),xp_(1));

	Eigen::VectorXd res= vppItemsContainer_->getResiduals();
	// TORESTORE
	//printf("      residuals: dF= %g, dM= %g, c1= %g, c2= %g\n\n",
	//		res(0),res(1),res(2),res(3) );
	printf("      residuals: dF= %g, dM= %g\n\n", res(0),res(1) );
	// Push the result to the result container
	pResults_->push_back(twv, twa, xp_, res);

}

// Make a printout of the results for this run
void NRSolver::printResults() {

	std::cout<<"==== NRSolver RESULTS PRINTOUT ==================="<<std::endl;
	pResults_->print();
	std::cout<<"---------------------------------------------------\n"<<std::endl;

}

// Make a printout of the results for this run
void NRSolver::plotPolars() {

	// Instantiate the Polar Plotters for Boat velocity, Boat heel,
	// Sail flat, Crew B, dF and dM
	PolarPlotter boatSpeedPolarPlotter("Boat Speed Polar Plot");
	PolarPlotter boatHeelPolarPlotter("Boat Heel Polar Plot");
	// TORESTORE
	//	PolarPlotter crewBPolarPlotter("Crew B Polar Plot");
	//	PolarPlotter sailFlatPolarPlotter("Sail Flat");

	// Instantiate the list of wind angles that will serve
	// for each velocity
	Eigen::ArrayXd windAngles(pResults_->windAngleSize());
	Eigen::ArrayXd boatVelocity(pResults_->windAngleSize());
	Eigen::ArrayXd boatHeel(pResults_->windAngleSize());
	// TORESTORE
	//	Eigen::ArrayXd crewB(pResults_->windAngleSize());
	//	Eigen::ArrayXd sailFlat(pResults_->windAngleSize());

	// Loop on the wind velocities
	for(size_t iWv=0; iWv<pResults_->windVelocitySize(); iWv++) {

		// Store the wind velocity as a label for this curve
		char windVelocityLabel[256];
		sprintf(windVelocityLabel,"%3.1f", pResults_->get(iWv,0).getTWV() );
		string wVLabel(windVelocityLabel);

		// Loop on the wind angles
		for(size_t iWa=0; iWa<pResults_->windAngleSize(); iWa++) {

			// fill the list of wind angles in degrees
			windAngles(iWa) = mathUtils::toDeg( pResults_->get(iWv,iWa).getTWA() );

			// fill the list of boat speeds to an ArrayXd
			boatVelocity(iWa) = pResults_->get(iWv,iWa).getX()->coeff(0);

			// fill the list of boat heel to an ArrayXd
			boatHeel(iWa) = mathUtils::toDeg( pResults_->get(iWv,iWa).getX()->coeff(1) );

			// TORESTORE
			//			// fill the list of Crew B to an ArrayXd
			//			crewB(iWa) = pResults_->get(iWv,iWa).getX()->coeff(2);
			//
			//			// fill the list of Sail flat to an ArrayXd
			//			sailFlat(iWa) = pResults_->get(iWv,iWa).getX()->coeff(3);

		}

		// Append the angles-data to the relevant plotter
		boatSpeedPolarPlotter.append(wVLabel,windAngles,boatVelocity);
		boatHeelPolarPlotter.append(wVLabel,windAngles,boatHeel);
		// TORESTORE
		//		crewBPolarPlotter.append(wVLabel,windAngles,crewB);
		//		sailFlatPolarPlotter.append(wVLabel,windAngles,sailFlat);

	}

	// Ask all plotters to plot
	boatSpeedPolarPlotter.plot();
	boatHeelPolarPlotter.plot();
	// TORESTORE
	//	crewBPolarPlotter.plot();
	//	sailFlatPolarPlotter.plot();
}

// Make a printout of the results for this run
void NRSolver::plotXY(size_t iWa) {

	if( iWa>=pResults_->windAngleSize() ){
		std::cout<<"User requested a wrong index! \n";
		return;
	}

	// Prepare the data for the plotter
	Eigen::ArrayXd windSpeeds(pResults_->windVelocitySize());
	Eigen::ArrayXd boatVelocity(pResults_->windVelocitySize());
	Eigen::ArrayXd boatHeel(pResults_->windVelocitySize());
	// TORESTORE
	//	Eigen::ArrayXd boatFlat(pResults_->windVelocitySize());
	//	Eigen::ArrayXd boatB(pResults_->windVelocitySize());
	Eigen::ArrayXd dF(pResults_->windVelocitySize());
	Eigen::ArrayXd dM(pResults_->windVelocitySize());

	for(size_t iWv=0; iWv<pResults_->windVelocitySize(); iWv++) {

		windSpeeds(iWv)  = pResults_->get(iWv,iWa).getTWV();
		boatVelocity(iWv)= pResults_->get(iWv,iWa).getX()->coeff(0);
		// Convert the heel results to degrees
		boatHeel(iWv)    = mathUtils::toDeg( pResults_->get(iWv,iWa).getX()->coeff(1) );
		// TORESTORE
		//		boatB(iWv)    	 = pResults_->get(iWv,iWa).getX()->coeff(2);
		//		boatFlat(iWv)    = pResults_->get(iWv,iWa).getX()->coeff(3);
		dF(iWv)          = pResults_->get(iWv,iWa).getdF();
		dM(iWv)          = pResults_->get(iWv,iWa).getdM();

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

	// TORESTORE
	//	// Instantiate a plotter for the Flat
	//	Plotter plotter3;
	//	string t3=string("Sail FLAT")+string(title);
	//	plotter3.plot(windSpeeds,boatFlat,windSpeeds,boatFlat,
	//			t3,"Wind Speed [m/s]","Sail FLAT [-]");

	// TORESTORE
	// Instantiate a plotter for the position of the movable crew B
	//	Plotter plotter4;
	//	string t4=string("Crew position")+string(title);
	//	plotter4.plot(windSpeeds,boatB,windSpeeds,boatB,
	//			t4,"Wind Speed [m/s]","Position of the movable crew [m]");

	// Instantiate a plotter for the residuals
	Plotter plotter5;
	string t5=string("Residuals")+string(title);
	plotter5.plot(windSpeeds,dF,windSpeeds,dM,
			t5,"Wind Speed [m/s]","Residuals [N,N*m]");
}







