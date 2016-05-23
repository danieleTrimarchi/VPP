#include "Optimizer.h"
#include "VPPException.h"
#include "Interpolator.h"
#include <fstream>
#include "mathUtils.h"

using namespace mathUtils;

// Init static member
boost::shared_ptr<VPPItemFactory> Optimizer::vppItemsContainer_;
size_t Optimizer::maxIters_;
int optIterations=0;

//// Optimizer class  //////////////////////////////////////////////

// Constructor
Optimizer::Optimizer(boost::shared_ptr<VPPItemFactory> VPPItemFactory):
				dimension_(4),
				tol_(1.e-6) {

	// Instantiate a NLOpobject and set the ISRES "Improved Stochastic Ranking Evolution Strategy"
	// algorithm for nonlinearly-constrained global optimization
	//opt_.reset( new nlopt::opt(nlopt::GN_ISRES,dimension_) );
	opt_.reset( new nlopt::opt(nlopt::LN_COBYLA,dimension_) );

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
	lowerBounds_[2] = pParser_->get("B_MIN"); ;	// lower reef
	upperBounds_[2] = pParser_->get("B_MAX"); ;	// upper reef
	lowerBounds_[3] = pParser_->get("F_MIN"); ;	// lower FLAT
	upperBounds_[3] = pParser_->get("F_MAX"); ;	// upper FLAT

	// Set the bounds for the constraints
	opt_->set_lower_bounds(lowerBounds_);
	opt_->set_upper_bounds(upperBounds_);

	// Resize the vector with the initial guess/optimizer results
	xp_.resize(dimension_);

	// Set the objective function to be maximized (using set_max_objective)
	opt_->set_max_objective(VPP_speed, NULL);

	// Set the absolute tolerance on the state variables
	opt_->set_xtol_abs(tol_);

	// Set the absolute tolerance on the function value
	opt_->set_ftol_abs(tol_);

	// Set the max number of evaluations for a single run
	maxIters_= 40000;
	opt_->set_maxeval(maxIters_);

	// Also get a reference to the WindItem that has computed the
	// real wind velocity/angle for the current run
	pWind_=vppItemsContainer_->getWind();

	// Init the ResultContainer that will be filled while running the results
	pResults_.reset(new ResultContainer(pWind_));

}

// Destructor
Optimizer::~Optimizer() {
	// make nothing
}

void Optimizer::reset(boost::shared_ptr<VPPItemFactory> VPPItemFactory) {

	// Init the STATIC member vppItemsContainer
	vppItemsContainer_= VPPItemFactory;

	// Set the parser
	pParser_= vppItemsContainer_->getParser();

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

	// Also get a reference to the WindItem that has computed the
	// real wind velocity/angle for the current run
	pWind_=vppItemsContainer_->getWind();

	// Init the ResultContainer that will be filled while running the results
	pResults_.reset(new ResultContainer(pWind_));

}

// Set the initial guess for the state variable vector
void Optimizer::resetInitialGuess(int TWV, int TWA) {

	// In it to something small to start the evals at each velocity
	if(TWV==0) {

		xp_(0)= 0.01;  	// V_0
		xp_(1)= 0.01;		// PHI_0
		xp_(2)= 0.01;		// b_0
		xp_(3)= .9;		// f_0

	}

////	else if(TWV>1) {
////
////		// For twv> 1 we can linearly predict the result of the state vector
////		Extrapolator extrapolator(
////				pResults_->get(TWV-2,TWA).getTWV(),
////				pResults_->get(TWV-2,TWA).getX(),
////				pResults_->get(TWV-1,TWA).getTWV(),
////				pResults_->get(TWV-1,TWA).getX()
////		);
////
////		// Extrapolate the state vector for the current wind
////		// velocity. Note that the items have not been init yet
////		xp_= extrapolator.get( pWind_->getTWV(TWV) );
////	}
////
////	// Make sure the initial guess does not exceeds the bounds
////	for(size_t i=0; i<dimension_; i++) {
////		if(xp_[i]<lowerBounds_[i])
////			xp_[i]=lowerBounds_[i];
////		if(xp_[i]>upperBounds_[i])
////			xp_[i]=upperBounds_[i];
////	}
//
//	std::cout<<"INITIAL GUESS: "<<std::endl;
//	cout<<"  "<<xp_[0]<<" , "<<xp_[1]<<" , "<<xp_[2]<<" , "<<xp_[3]<<endl;

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

	// Make a ptr to the non static member function VPPconstraint
	opt_->add_equality_mconstraint(VPPconstraint, &loopData, tol);

	// Instantiate the maximum objective value, upon return
	double maxf;

	nlopt::result result;
	try{
		// Launch the optimization; negative retVal implies failure
		std::cout<<"Entering the optimizer with: "<<
				xp_(0)<<" "<<xp_(1)<<" "<<xp_(2)<<" "<<xp_(3)<<"\n";
		// convert to standard vector
		std::vector<double> xp(xp_.rows());
		for(size_t i=0; i<xp_.rows(); i++)
			xp[i]=xp_(i);
		result = opt_->optimize(xp, maxf);
		//store the results back to the member state vector
		for(size_t i=0; i<xp_.size(); i++)
			xp_(i)=xp[i];
	}
	catch( nlopt::roundoff_limited& e ){
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
	printf("      at f(%g,%g,%g,%g) = %0.10g\n",
			xp_(0),xp_(1),xp_(2),xp_(3),maxf);
	double dF, dM;
	vppItemsContainer_->getResiduals(dF,dM);
	printf("      residuals: dF= %g, dM= %g\n\n",dF,dM);

	// Push the result to the result container
	pResults_->push_back(TWV, TWA, xp_, dF, dM);

}

// Make a printout of the results for this run
void Optimizer::printResults() {

	std::cout<<"==== OPTIMIZER RESULTS PRINTOUT ==================="<<std::endl;
	pResults_->print();
	std::cout<<"---------------------------------------------------\n"<<std::endl;

}

// Make a printout of the result bounds for this run
void Optimizer::printResultBounds() {

	std::cout<<"==== OPTIMIZER RESULT BOUNDS PRINTOUT ==================="<<std::endl;
	pResults_->printBounds();
	std::cout<<"---------------------------------------------------\n"<<std::endl;

}

/// Make a printout of the results for this run
void Optimizer::plotPolars() {

	// Instantiate the Polar Plotters for Boat velocity, Boat heel,
	// Sail flat, Crew B, dF and dM
	PolarPlotter boatSpeedPolarPlotter("Boat Speed Polar Plot");
	PolarPlotter boatHeelPolarPlotter("Boat Heel Polar Plot");
	PolarPlotter crewBPolarPlotter("Crew B Polar Plot");
	PolarPlotter sailFlatPolarPlotter("Sail Flat");

	// Instantiate the list of wind angles that will serve
	// for each velocity
	Eigen::ArrayXd windAngles(pResults_->windAngleSize());
	Eigen::ArrayXd boatVelocity(pResults_->windAngleSize());
	Eigen::ArrayXd boatHeel(pResults_->windAngleSize());
	Eigen::ArrayXd crewB(pResults_->windAngleSize());
	Eigen::ArrayXd sailFlat(pResults_->windAngleSize());

	// Loop on the wind velocities
	for(size_t iWv=0; iWv<pResults_->windVelocitySize(); iWv++) {

		// Store the wind velocity as a label for this curve
		char windVelocityLabel[256];
		sprintf(windVelocityLabel,"%3.1f", pResults_->get(iWv,0).getTWV() );
		string wVLabel(windVelocityLabel);

		// Loop on the wind angles
		for(size_t iWa=0; iWa<pResults_->windAngleSize(); iWa++) {

			// fill the list of wind angles
			windAngles(iWa) = mathUtils::toDeg(pResults_->get(iWv,iWa).getTWA());

			// fill the list of boat speeds to an ArrayXd
			boatVelocity(iWa) = pResults_->get(iWv,iWa).getX()->coeff(0);

			// fill the list of boat heel to an ArrayXd
			boatHeel(iWa) = mathUtils::toDeg(pResults_->get(iWv,iWa).getX()->coeff(1));

			// fill the list of Crew B to an ArrayXd
			crewB(iWa) = pResults_->get(iWv,iWa).getX()->coeff(2);

			// fill the list of Sail flat to an ArrayXd
			sailFlat(iWa) = pResults_->get(iWv,iWa).getX()->coeff(3);

		}

		// Append the angles-data to the relevant plotter
		boatSpeedPolarPlotter.append(wVLabel,windAngles,boatVelocity);
		boatHeelPolarPlotter.append(wVLabel,windAngles,boatHeel);
		crewBPolarPlotter.append(wVLabel,windAngles,crewB);
		sailFlatPolarPlotter.append(wVLabel,windAngles,sailFlat);

	}

	// Ask all plotters to plot
	boatSpeedPolarPlotter.plot();
	boatHeelPolarPlotter.plot(5);
	crewBPolarPlotter.plot();
	sailFlatPolarPlotter.plot();
}

/// Make a printout of the results for this run
void Optimizer::plotXY(size_t iWa) {

	if( iWa>=pResults_->windAngleSize() ){
		std::cout<<"User requested a wrong index! \n";
		return;
	}

	// Prepare the data for the plotter
	Eigen::ArrayXd windSpeeds(pResults_->windVelocitySize());
	Eigen::ArrayXd boatVelocity(pResults_->windVelocitySize());
	Eigen::ArrayXd boatHeel(pResults_->windVelocitySize());
	Eigen::ArrayXd boatFlat(pResults_->windVelocitySize());
	Eigen::ArrayXd boatB(pResults_->windVelocitySize());
	Eigen::ArrayXd dF(pResults_->windVelocitySize());
	Eigen::ArrayXd dM(pResults_->windVelocitySize());

	for(size_t iWv=0; iWv<pResults_->windVelocitySize(); iWv++) {

		windSpeeds(iWv)  = pResults_->get(iWv,iWa).getTWV();
		boatVelocity(iWv)= pResults_->get(iWv,iWa).getX()->coeff(0);
		boatHeel(iWv)    = pResults_->get(iWv,iWa).getX()->coeff(1);
		boatB(iWv)    	 = pResults_->get(iWv,iWa).getX()->coeff(2);
		boatFlat(iWv)    = pResults_->get(iWv,iWa).getX()->coeff(3);
		dF(iWv)          = pResults_->get(iWv,iWa).getdF();
		dM(iWv)          = pResults_->get(iWv,iWa).getdM();

	}

	char title[256];
	sprintf(title,"AWA= %4.2f", toDeg(pWind_->getTWA(iWa)) );

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

	// Instantiate a plotter for the Flat
	Plotter plotter3;
	string t3=string("Sail FLAT")+string(title);
	plotter3.plot(windSpeeds,boatFlat,windSpeeds,boatFlat,
			t3,"Wind Speed [m/s]","Sail FLAT [-]");

	// Instantiate a plotter for the position of the movable crew B
	Plotter plotter4;
	string t4=string("Crew position")+string(title);
	plotter4.plot(windSpeeds,boatB,windSpeeds,boatB,
			t4,"Wind Speed [m/s]","Position of the movable crew [m]");

	// Instantiate a plotter for the residuals
	Plotter plotter5;
	string t5=string("Residuals")+string(title);
	plotter5.plot(windSpeeds,dF,windSpeeds,dM,
			t5,"Wind Speed [m/s]","Residuals [N,N*m]");
}

// Add this method for compatibility with the NR solver.
// TODO dtrimarchi: this could go to a common parent class
void Optimizer::plotJacobian() {
	std::cout<<"Jacobian plot not allowed for this optimizer!"<<std::endl;
	std::cout<<"-- please retry with a NR solver! --        "<<std::endl;
}






