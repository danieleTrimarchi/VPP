#include "SemiAnalyticalOptimizer.h"
#include "VPPException.h"
#include "Interpolator.h"
#include "Regression.h"
#include <fstream>
#include "mathUtils.h"
#include "VPPResultIO.h"

using namespace mathUtils;

namespace Optim {

// Init static member
size_t SemiAnalyticalOptimizer::maxIters_;
int SemiAnalyticalOptimizer::optIterations_=0;

//// SemiAnalyticalOptimizer class  //////////////////////////////////////////////

// Constructor
SemiAnalyticalOptimizer::SemiAnalyticalOptimizer(std::shared_ptr<VPPItemFactory> VPPItemFactory):
		VPPSolverBase(VPPItemFactory),
		saPbSize_(dimension_-subPbSize_) {

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

	// Generate bound vectors for the SA problem
	std::vector<double> lowerSAbounds(lowerBounds_.begin()+saPbSize_, lowerBounds_.end());
	std::vector<double> upperSAbounds(upperBounds_.begin()+saPbSize_, upperBounds_.end());

	// Set the bounds for the constraints
	opt_->set_lower_bounds(lowerSAbounds);
	opt_->set_upper_bounds(upperSAbounds);

	// Set the absolute tolerance on the state variables
	//	opt_->set_xtol_abs(tol_);
	opt_->set_xtol_rel(tol_);

	// Set the absolute tolerance on the function value
	//	opt_->set_ftol_abs(tol_);
	opt_->set_ftol_rel(tol_);

	// Set the max number of evaluations for a single run
	maxIters_= 4000;
	opt_->set_maxeval(maxIters_);

}

// Destructor
SemiAnalyticalOptimizer::~SemiAnalyticalOptimizer() {
	// make nothing
}

void SemiAnalyticalOptimizer::reset(std::shared_ptr<VPPItemFactory> VPPItemFactory) {

	// Decorator for the mother class method reset
	VPPSolverBase::reset(VPPItemFactory);

	// Generate bound vectors for the SA problem
	std::vector<double> lowerSAbounds(lowerBounds_.begin()+saPbSize_, lowerBounds_.end());
	std::vector<double> upperSAbounds(upperBounds_.begin()+saPbSize_, upperBounds_.end());

	// Set the bounds for the constraints
	opt_->set_lower_bounds(lowerSAbounds);
	opt_->set_upper_bounds(upperSAbounds);

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
	++optIterations_;

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

	// Declare a buffer vector xp for the optimizer. Declare it here so
	// that is available to the invalid_argument exception
	std::vector<double> xp(saPbSize_);

	// Declare the number of evaluations in the optimization space
	// Remember the state vector x={v phi b f}
	size_t nCrew=5, nFlat=5;

	// Declare a matrix that stores the velocity values in the opt space
	Eigen::MatrixXd x(nCrew,nFlat), y(nCrew,nFlat), u(nCrew,nFlat);

	// Beginning of the outer try-catch block used to stop the algorithm for this step
	// when the NR solver cannot converge
	try{

		// Loop on the optimization space
		for(size_t iFlat=0; iFlat<nFlat; iFlat++){

			// Set the value of Flat in the state vector
			xp_(3)= lowerBounds_[saPbSize_+1] + double(iFlat) / (nFlat-1) * (upperBounds_[saPbSize_+1]-lowerBounds_[saPbSize_+1]);

			for(size_t iCrew=0; iCrew<nCrew; iCrew++){

				// Set the value of b in the state vector
				xp_(2)= lowerBounds_[saPbSize_] + double(iCrew) / (nCrew-1) * (upperBounds_[saPbSize_]-lowerBounds_[saPbSize_]);

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
		optIterations_ = 0;

		try{

			// Launch the optimization; negative retVal implies failure
			std::cout<<"Entering the SemiAnalyticalOptimizer with: ";
			printf("%8.6f,%8.6f,%8.6f,%8.6f \n", xp_(0),xp_(1),xp_(2),xp_(3));

			// convert to standard vector
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

		printf("found maximum after %d evaluations\n", optIterations_);
		printf("      at f(%g,%g,%g,%g)\n",
				xp_(0),xp_(1),xp_(2),xp_(3) );

		// We have now tuned the optimization variables. Re-run NR to assure that the solution
		// is indeed an equilibrium state. We must be very close to the solution already
		solveInitialGuess(TWV, TWA);

		// Get and print the final residuals
		Eigen::VectorXd residuals= pVppItemsContainer_->getResiduals();
		printf("      residuals: dF= %g, dM= %g\n\n",residuals(0),residuals(1) );

		// Push the result to the result container
		pResults_->push_back(TWV, TWA, xp_, residuals(0), residuals(1) );
	}
	catch( NonConvergedException& e ){
		std::cout<<"Catching NonConvergedException in SAOA"<<std::endl;
		// Push to the result container a result to be discarded -> that won't be plot
		pResults_->remove(TWV, TWA);

	}
	catch(std::invalid_argument& e){
		std::cout<<"\nThe optimizer returned an invalid argument exception."<<std::endl;
		std::cout<<"This often happens if the specified initial guess exceeds the variable bounds."<<std::endl;
		std::cout<<"Initial guess: ";
		for(size_t i=0; i<xp.size(); i++) std::cout<<xp[i]<<", ";
		std::cout<<"\n\n";
		char msg[256];
		sprintf(msg,"%s",e.what());
		throw VPPException(HERE,msg);
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

} // end namespace Optim




