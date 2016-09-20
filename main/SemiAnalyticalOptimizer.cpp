#include "SemiAnalyticalOptimizer.h"
#include "VPPException.h"
#include "Interpolator.h"
#include "Regression.h"
#include <fstream>
#include "mathUtils.h"
#include "VPPPlotSet.h"

using namespace mathUtils;

namespace SAOA {

// Init static member
boost::shared_ptr<VPPItemFactory> SemiAnalyticalOptimizer::vppItemsContainer_;
size_t SemiAnalyticalOptimizer::maxIters_;
int optIterations=0;

//// SemiAnalyticalOptimizer class  //////////////////////////////////////////////

// Constructor
SemiAnalyticalOptimizer::SemiAnalyticalOptimizer(boost::shared_ptr<VPPItemFactory> VPPItemFactory):
												dimension_(4),
												subPbSize_(2),
												saPbSize_(dimension_-subPbSize_),
												tol_(1.e-3) {

	// Compute the size of the Semi-Analytical-Optimization-Approach problem size. This is
	// the size of the problem that will be handed to the optimizer. See explanation below
	if(saPbSize_!=2)
		throw VPPException(HERE,"saPbSize is supposed to be 2!");

	// Instantiate a NLOpobject and set the LD_MMA algorithm for gradient-based
	// local optimization including nonlinear inequality (not equality!) constraints
	// The size of the problem to optimize is subPbSize because I will be handing an
	// analytical polynomial where we recover the value of the objective function VPP_SPEED
	// when varying the optimization variables crew, flat.
	opt_.reset( new nlopt::opt(nlopt::LD_MMA,saPbSize_) );

	// Instantiate a NRSolver that will be used to feed the SemiAnalyticalOptimizer with
	// an equilibrated first guess solution. The solver will solve a subproblem
	// without optimization variables
	nrSolver_.reset( new NRSolver(VPPItemFactory.get(),dimension_,subPbSize_) );

	// Init the STATIC member vppItemsContainer
	vppItemsContainer_= VPPItemFactory;

	// Set the parser
	pParser_= vppItemsContainer_->getParser();

	// Set the and apply the lower and the upper bounds
	// -> make sure the bounds are larger than the initial
	// 		guess!
	lowerBounds_.resize(saPbSize_);
	upperBounds_.resize(saPbSize_);

	lowerBounds_[0] = pParser_->get("B_MIN"); ;	// lower reef
	upperBounds_[0] = pParser_->get("B_MAX"); ;	// upper reef
	lowerBounds_[1] = pParser_->get("F_MIN"); ;	// lower FLAT
	upperBounds_[1] = pParser_->get("F_MAX"); ;	// upper FLAT

	// Set the bounds for the constraints
	opt_->set_lower_bounds(lowerBounds_);
	opt_->set_upper_bounds(upperBounds_);

	// Resize the vector with the initial guess/SemiAnalyticalOptimizer results
	xp_.resize(dimension_);

	// Set the absolute tolerance on the state variables
	//	opt_->set_xtol_abs(tol_);
	opt_->set_xtol_rel(tol_);

	// Set the absolute tolerance on the function value
	//	opt_->set_ftol_abs(tol_);
	opt_->set_ftol_rel(tol_);

	// Set the max number of evaluations for a single run
	maxIters_= 4000;
	opt_->set_maxeval(maxIters_);

	// Also get a reference to the WindItem that has computed the
	// real wind velocity/angle for the current run
	pWind_=vppItemsContainer_->getWind();

	// Init the ResultContainer that will be filled while running the results
	pResults_.reset(new ResultContainer(pWind_));

}

// Destructor
SemiAnalyticalOptimizer::~SemiAnalyticalOptimizer() {
	// make nothing
}

void SemiAnalyticalOptimizer::reset(boost::shared_ptr<VPPItemFactory> VPPItemFactory) {

	// Init the STATIC member vppItemsContainer
	vppItemsContainer_= VPPItemFactory;

	// Set the parser
	pParser_= vppItemsContainer_->getParser();

	lowerBounds_[0] = pParser_->get("B_MIN"); ;	// lower reef
	upperBounds_[0] = pParser_->get("B_MAX"); ;	// upper reef
	lowerBounds_[1] = pParser_->get("F_MIN"); ;	// lower FLAT
	upperBounds_[1] = pParser_->get("F_MAX"); ;	// upper FLAT

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
void SemiAnalyticalOptimizer::resetInitialGuess(int TWV, int TWA) {

	// In it to something small to start the evals at each velocity
	if(TWV==0) {

		xp_(0)= 1.0;  	// V_0
		xp_(1)= 0.1;		// PHI_0
		xp_(2)= 0.01;		// b_0
		xp_(3)= 0.99;		// f_0

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

		// Make sure the initial guess does not exceeds the bounds for the opt vars
		for(size_t i=0; i<saPbSize_; i++) {
			if(xp_[subPbSize_+i]<lowerBounds_[i])
				xp_[subPbSize_+i]=lowerBounds_[i];
			if(xp_[subPbSize_+i]>upperBounds_[i])
				xp_[subPbSize_+i]=upperBounds_[i];
		}
	}

	std::cout<<"-->> SemiAnalyticalOptimizer first guess: "<<xp_.transpose()<<std::endl;

}

// Ask the NRSolver to solve a sub-problem without the optimization variables
// this makes the initial guess an equilibrated solution
void SemiAnalyticalOptimizer::solveInitialGuess(int TWV, int TWA) {

	std::cout<<"-->> SemiAnalyticalOptimizer solve first guess: "<<xp_.transpose()<<std::endl;

	// Get
	xp_.block(0,0,2,1)= nrSolver_->run(TWV,TWA,xp_).block(0,0,2,1);

}

// Struct holding the coefficients of the regression polynomial
typedef struct {
		Eigen::VectorXd coeffs;
} regression_coeffs;

// Set the function to be optimized and its gradient
double SemiAnalyticalOptimizer::VPP_speed(unsigned n, const double* x, double *grad, void *my_func_data) {

	// Cast the regression coefficients struct passed in as void*
	regression_coeffs* c = (regression_coeffs *) my_func_data;
    
	// Increment the number of iterations for each call of the objective function
	++optIterations;

	// Fill the coordinate vector: x^2, xy, y^2, x, y, 1;
	// --> 	Note that in this case x <- x[2] ; y <- x[3]
	// 			because we are in the opt space where the state vars
	//			u=x[0], phi=x[1] are left constant
	Eigen::VectorXd coords(6);
	coords << x[0]*x[0], x[0]*x[1], x[1]*x[1], x[0], x[1], 1;

	// c0 x^2 + c1 xy + c2 y^2 + c3 x + c4 y + c5
	// d/dx= 2 c0 x + c1 y + c3
	// d/dy= 2 c2 y + c1 x + c4
	if (grad) {

		grad[0] = 2 * c->coeffs(0) * x[0] + c->coeffs(1) * x[1] + c->coeffs(3);
		if(mathUtils::isNotValid(grad[0])) throw VPPException(HERE,"grad[0] is NAN!");

		grad[1] = 2 * c->coeffs(2) * x[1] + c->coeffs(1) * x[0] + c->coeffs(4);
		if(mathUtils::isNotValid(grad[1])) throw VPPException(HERE,"grad[1] is NAN!");
	}

	// Return the value of the function
	return c->coeffs.transpose()*coords;

}

// Execute a VPP-like analysis
void SemiAnalyticalOptimizer::run(int TWV, int TWA) {

	std::cout<<"    "<<pWind_->getTWV(TWV)<<"    "<<toDeg(pWind_->getTWA(TWA))<<std::endl;

	// For each wind velocity, reset the initial guess for the
	// state variable vector to zero. This is x0
	resetInitialGuess(TWV,TWA);

	// Refine the initial guess solving a sub-problem with no
	// optimization variables. This is x1
	std::cout<<"SAOA, solveInitialGuess: "<<std::endl;
	solveInitialGuess(TWV,TWA);
	std::cout<<"-------------------------"<<std::endl;

	// Buffer initial guess before entering the regression loop
	Eigen::VectorXd xpBuf= xp_;

	// Declare the number of evaluations in the optimization space
	// Remember the state vector x={v phi b f}
	size_t nCrew=3, nFlat=3;

	// Declare a matrix that stores the velocity values in the opt space
	Eigen::MatrixXd x(nCrew,nFlat), y(nCrew,nFlat), u(nCrew,nFlat);

	// Beginning of the outer try-catch block used to stop the algorithm for this step
	// when the NR solver cannot converge
	try{

		// Loop on the optimization space
		for(size_t iFlat=0; iFlat<nFlat; iFlat++){

			// Set the value of Flat in the state vector
			xp_(3)= lowerBounds_[1] + double(iFlat) / (nFlat-1) * (upperBounds_[1]-lowerBounds_[1]);

			for(size_t iCrew=0; iCrew<nCrew; iCrew++){

				// Set the value of b in the state vector
				xp_(2)= lowerBounds_[0] + double(iCrew) / (nCrew-1) * (upperBounds_[0]-lowerBounds_[0]);

				// Store the coordinates of this point in the optimization space
				x(iCrew,iFlat)= xp_(2);
				y(iCrew,iFlat)= xp_(3);

				// Solve this opt configuration with the Newton solver.
				// Store x(0) into the buffer matrix u
				//std::cout<<"\n-->> Running nrSolver for : iCrew= "<<iCrew<<"  iFlat= "<<iFlat<<std::endl;
				xp_= nrSolver_->run(TWV,TWA,xp_);

				// store u
				u(iCrew,iFlat)= xp_(0);
			}
		}

		// Restore the value of xp_ prior to the regression loop
		xp_= xpBuf;

		// At this point I have three arrays: x <- flat ; y <- crew ; u
		// that describes the change of velocity for each opt parameter.
		// The heeling angle is free to change but we do not care about its value

		// Instantiate a Regression based on the computational points contained in u,
		// so to express u(flat,crew)
		Regression regr(x,y,u);
		Eigen::VectorXd polynomial= regr.compute();
		//std::cout<<" POLYNOMIAL: \n"<<polynomial<<std::endl;

		// ====== Set NLOPT ============================================

		// Reset the number of iterations
		optIterations = 0;

		try{

			// Launch the optimization; negative retVal implies failure
			std::cout<<"Entering the SemiAnalyticalOptimizer with: ";
			printf("%8.6f,%8.6f,%8.6f,%8.6f \n", xp_(0),xp_(1),xp_(2),xp_(3));

			// convert to standard vector
			std::vector<double> xp(saPbSize_);
			for(size_t i=0; i<saPbSize_; i++)
				xp[i]=xp_(subPbSize_+i);

			// Place the regression polynomial into an object to be sent to nlOpt
			// todo dtrimarchi : improve the init of the regression_coeffs struct!
			regression_coeffs c;
			c.coeffs= polynomial;

			// Set the objective function to be maximized using the regression coeffs
			opt_->set_max_objective(VPP_speed, &c);

			// Instantiate the maximum objective value, upon return
			double maxf;

			// Launch the optimization
			nlopt::result result = opt_->optimize(xp, maxf);

			//store the results back to the member state vector
			for(size_t i=0; i<saPbSize_; i++)
				xp_(subPbSize_+i)=xp[i];
		}
		catch( nlopt::roundoff_limited& e ){
			std::cout<<"Roundoff limited result"<<std::endl;
			// do nothing because the result of roundoff-limited exception
			// is meant to be still a meaningful result
		}

		printf("found maximum after %d evaluations\n", optIterations);
		printf("      at f(%g,%g,%g,%g)\n",
				xp_(0),xp_(1),xp_(2),xp_(3) );

		// We have now tuned the optimization variables. Re-run NR to assure that the solution
		// is indeed an equilibrium state. We must be very close to the solution already
		solveInitialGuess(TWV, TWA);

		// Get and print the final residuals
		Eigen::VectorXd residuals= vppItemsContainer_->getResiduals();
		printf("      residuals: dF= %g, dM= %g\n\n",residuals(0),residuals(1) );

		// Push the result to the result container
		pResults_->push_back(TWV, TWA, xp_, residuals(0), residuals(1) );
	}
	catch( NonConvergedException& e ){

		// Push to the result container a result to be discarded -> that won't be plot
		pResults_->remove(TWV, TWA);

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
}

// Returns the state vector for a given wind configuration
const Eigen::VectorXd SemiAnalyticalOptimizer::getResult(int TWV, int TWA) {
	return *(pResults_->get(TWV,TWA).getX());
}

// Make a printout of the results for this run
void SemiAnalyticalOptimizer::printResults() {

	std::cout<<"==== SemiAnalyticalOptimizer RESULTS PRINTOUT ==================="<<std::endl;
	pResults_->print();
	std::cout<<"---------------------------------------------------\n"<<std::endl;

}

// Make a printout of the result bounds for this run
void SemiAnalyticalOptimizer::printResultBounds() {

	std::cout<<"==== SemiAnalyticalOptimizer RESULT BOUNDS PRINTOUT ==================="<<std::endl;
	pResults_->printBounds();
	std::cout<<"---------------------------------------------------\n"<<std::endl;

}

/// Make a printout of the results for this run
void SemiAnalyticalOptimizer::plotPolars() {

	// Instantiate the Polar Plotters for Boat velocity, Boat heel,
	// Sail flat, Crew B, dF and dM
	VPPPolarPlotter boatSpeedPolarPlotter("Boat Speed Polar Plot");
	VPPPolarPlotter boatHeelPolarPlotter("Boat Heel Polar Plot");
	VPPPolarPlotter crewBPolarPlotter("Crew B Polar Plot");
	VPPPolarPlotter sailFlatPolarPlotter("Sail Flat");

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
void SemiAnalyticalOptimizer::plotXY(size_t iWa) {

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
void SemiAnalyticalOptimizer::plotJacobian() {
	nrSolver_->plotJacobian();
}

}




