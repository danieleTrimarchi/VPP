#include "Optimizer.h"
#include "VPPException.h"

// Init static member
boost::shared_ptr<VPPItemFactory> Optimizer::vppItemsContainer_;
int optIterations=0;

///////// Optimizer Result Class ///////////////////////////////

/// Constructor
OptResult::OptResult(double twv, double twa, std::vector<double>& res, double dF, double dM) :
				twv_(twv),
				twa_(twa),
				result_(res),
				dF_(dF),
				dM_(dM) {

}

/// Destructor
OptResult::~OptResult(){
	// make nothing
}

// PrintOut the values stored in this result
void OptResult::print() {
	std::cout<<" "<<twv_<<" "<<twa_ <<"  --  ";
	for(size_t iRes=0; iRes<result_.size(); iRes++)
		std::cout<<" "<<result_[iRes];
	std::cout<<"  --  "<<dF_<<" "<<dM_;

	std::cout<<"\n";
}


// get the twv for this result
const double OptResult::getTWV() const {
	return twv_;
}

// get the twa for this result
const double OptResult::getTWA() const {
	return twa_;
}

// get the force residuals for this result
const double OptResult::getdF() const {
	return dF_;
}

// get the moment residuals for this result
const double OptResult::getdM() const {
	return dM_;
}

// get the state vector for this result
const std::vector<double>* OptResult::getX() const {
	return &result_;
}


/////////////////////////////////////////////////////////////

// Constructor
Optimizer::Optimizer(boost::shared_ptr<VPPItemFactory> VPPItemFactory):
				dimension_(4),
				tol_(1.e-6){

	// Instantiate a NLOpobject and set the ISRES "Improved Stochastic Ranking Evolution Strategy"
	// algorithm for nonlinearly-constrained global optimization
	//opt_.reset( new nlopt::opt(nlopt::LN_COBYLA,dimension_) );
	//opt_.reset( new nlopt::opt(nlopt::GN_ISRES,dimension_) );
	opt_.reset( new nlopt::opt(nlopt::LN_COBYLA,dimension_) );

	// Init the static member vppItemsContainer
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
	// and set the initial guess
	xp_.resize(dimension_);
	xp_[0]= 0.01;  	// V_0
	xp_[1]= 0.01;		// PHI_0
	xp_[2]= 0.01;		// b_0
	xp_[3]= .99;		// f_0

	// Set the objective function to be maximized (using set_max_objective)
	opt_->set_max_objective(VPP_speed, NULL);

	// Set the relative tolerance
	opt_->set_xtol_rel(tol_);

	// Set the max number of evaluations for a single run
	opt_->set_maxeval(5000);

	// Also get a reference to the WindItem that has computed the
	// real wind velocity/angle for the current run
	pWind_=vppItemsContainer_->getWind();

}

// Destructor
Optimizer::~Optimizer() {
	// make nothing
}

// Set the objective function for tutorial g13
double Optimizer::VPP_speed(unsigned n, const double* x, double *grad, void *my_func_data) {

	// Increment the number of iterations for each call of the objective function
	++optIterations;

	if(isnan(x[0])) throw VPPException(HERE,"x[0] is NAN!");

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
	vppItemsContainer_->computeResiduals(result[0],result[1]);

}

void Optimizer::run(int TWV, int TWA) {

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

	// Make a ptr to the non static member function VPPconstraint
	opt_->add_equality_mconstraint(VPPconstraint, &loopData, tol);

	// Instantiate the maximum objective value, upon return
	double maxf;

	// Launch the optimization; negative retVal implies failure
	nlopt::result result = opt_->optimize(xp_, maxf);

	if (result < 0)
		throw VPPException(HERE,"nlopt failed!\n");

	else {
		printf("found maximum after %d evaluations\n", optIterations);
		printf("      at f(%g,%g,%g,%g) = %0.10g\n",
				xp_[0],xp_[1],xp_[2],xp_[3],maxf);
		double dF, dM;
		vppItemsContainer_->getResiduals(dF,dM);
		printf("      residuals: dF= %g, dM= %g\n\n",dF,dM);

		// Push the result to the result vector
		results_.push_back(OptResult(pWind_->getTWV(), pWind_->getTWA(), xp_, dF, dM));
	}
}

// Make a printout of the results for this run
void Optimizer::printResults() {

	std::cout<<"==== OPTIMIZER RESULTS PRINTOUT ======"<<std::endl;
	std::cout<<" TWV  TWA  -- V  PHI  B  F -- dF  dM "<<std::endl;
	std::cout<<"--------------------------------------"<<std::endl;
	for(size_t iRes=0; iRes<results_.size();iRes++)
		results_[iRes].print();

}

/// Make a printout of the results for this run
void Optimizer::plotResults() {

	// Prepare the data for the plotter
	Eigen::ArrayXd windSpeeds(results_.size());
	Eigen::ArrayXd boatSpeeds(results_.size());
	Eigen::ArrayXd boatHeel(results_.size());
	Eigen::ArrayXd dF(results_.size());
	Eigen::ArrayXd dM(results_.size());

	for(size_t iRes=0; iRes<results_.size(); iRes++) {
		windSpeeds(iRes) = results_[iRes].getTWV();
		boatSpeeds(iRes) = results_[iRes].getX()->at(0);
		boatHeel(iRes)   = results_[iRes].getX()->at(1);
		dF(iRes)         = results_[iRes].getdF();
		dM(iRes)         = results_[iRes].getdM();
	}

	// Instantiate a plotter for the velocity
	Plotter plotter;
	plotter.plot(windSpeeds,boatSpeeds,windSpeeds,boatSpeeds,
			"Boat Speed","Wind Speed [m/s]","Boat Speed [m/s]");

	// Instantiate a plotter for the heel
	Plotter plotter2;
	plotter2.plot(windSpeeds,boatHeel,windSpeeds,boatHeel,
			"Boat Heel","Wind Speed [m/s]","Boat Heel [deg]");

	// Instantiate a plotter for the residuals
	Plotter plotter3;
	plotter3.plot(windSpeeds,dF,windSpeeds,dM,
				"Residuals","Wind Speed [m/s]","Residuals [N,N*m]");

}






