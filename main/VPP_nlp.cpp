// Copyright (C) 2005, 2006 international Business Machines and others.
// All Rights Reserved.
// This code is published under the Eclipse Public License.
//
// $Id: VPP_NLP.cpp 2005 2011-06-06 12:55:16Z stefan $
//
// Authors:  Carl Laird, Andreas Waechter     IBM    2005-08-16

#include "VPP_nlp.h"

#include <cassert>
#include <iostream>
#include "VPPException.h"
#include "VPPJacobian.h"
#include "VPPException.h"
#include "VPPPlotSet.h"
#include "VPPResultIO.h"

using namespace Ipopt;
using namespace Eigen;

// Declare and init static members
size_t VPP_NLP::twa_=0, VPP_NLP::twv_=0;
Eigen::VectorXd VPP_NLP::xp0_((Eigen::VectorXd(4) << .5, 0., 0., 1.).finished());

//////////////

// Disallowed default constructor
VPP_NLP::VPP_NLP():
				dimension_(0),
				subPbSize_(0),
				nEqualityConstraints_(0),
				pParser_(NULL),
				pWind_(0) {

}

// Constructor with ptr to then VPPItemFactory
VPP_NLP::VPP_NLP(boost::shared_ptr<VPPItemFactory> pVppItemsContainer):
				pVppItemsContainer_(pVppItemsContainer),
				dimension_(4),  /* v, phi, crew, flat */
				subPbSize_(2),  /* v, phi */
				nEqualityConstraints_(2),
				pParser_(pVppItemsContainer->getParser()),
				pWind_(pVppItemsContainer_->getWind()){

	// Instantiate a nrSolver that will be used to compute the gradient
	// of the objective function : the vector [ du/du du/dPhi du/db du/df ]
	pSolver_.reset(new NRSolver(pVppItemsContainer.get(),dimension_,subPbSize_) );

	// Instantiate a VPPGradient that will be used to compute the gradient
	// of the objective function : the vector [ du/du du/dPhi du/db du/df ]
	pGradient_.reset(new VPPGradient(xp0_,pVppItemsContainer_.get()) );

	// Instantiate a result container
	pResults_.reset(new ResultContainer(pVppItemsContainer->getWind()));

}

// Default (virtual) destructor
VPP_NLP::~VPP_NLP(){

}

// Returns the size of the problem
bool VPP_NLP::get_nlp_info(int& dimension, int& nEqualityConstraints, int& nnz_jac_g,
		int& nnz_h_lag, IndexStyleEnum& int_style) {

	// The VPP problem has 4 variables, x[0] through x[3] or: v, phi, crew, flat
	dimension = dimension_;

	// Two equality constraints : dF=0 and dM=0
	nEqualityConstraints = nEqualityConstraints_;

	// The jacobian vector is dense and fully populated : df / dx_i
	// The jacobian of the constraints is dense and contains 8 nonzeros
	// dg1/dx1 dg1/dx2 ...
	// dg2/dx1 ...
	// for the VPP, g1=dF, g2=dM
	// So the jacobian reads:
	// dF/du dF/dPhi dF/db dF/df
	// dM/du dM/dPhi dM/db dM/df
	// to be computed with FD.
	// Note that the first two cols of this Jacobian are already
	// computed by VPPJacobian. We need to extend that class to
	// compute the full jacobian then!
	nnz_jac_g = dimension_ * nEqualityConstraints_;

	// The hessian is also dense and has 16 total non-zeros, but
	// a priori we do not need it because we have set
	// hessian_approximation","limited-memory"
	nnz_h_lag = 10;

	// Use the C style indexing (0-based)
	int_style = TNLP::C_STYLE;

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

	return true;
}

// Returns the variable bounds
bool VPP_NLP::get_bounds_info(int n, double* x_l, double* x_u,
		int m, double* g_l, double* g_u) {

	// Here, the n and m we gave IPOPT in get_nlp_info are passed back to us.
	// If desired, we could assert to make sure they are what we think they are.
	assert(n == dimension_);
	assert(m == nEqualityConstraints_);

	// Set the lower bounds for the state variables
	for(size_t i=0; i<lowerBounds_.size(); i++)
		x_l[i] = lowerBounds_[i];

	// Set the upper bounds for the state variables
	for(size_t i=0; i<upperBounds_.size(); i++)
		x_u[i] = upperBounds_[i];

	// Bounds on the constraints : as we only have equality constraints we set the
	// upper and lower bound to the same value.
	// It could be the case that relaxing this condition helps convergence.
	// test todo dtrimarchi
	g_l[0] = g_l[1] = 0.0;
	g_u[0] = g_u[1] = 0.0;

	return true;
}

// Overrides Ipopt::TPNLP get_scaling_parameters. Provide scaling
// factors for the objective function as well as for the optimization
// variables and/or constraints. The return value should be true,
// unless an error occurred, and the program is to be aborted.
// This method is called if the nlp scaling method is chosen as "user-scaling".
// if the value of obj_scaling is negative, then Ipopt will maximize
// the objective function instead of minimizing it.
// -----
// Note that this method should be used to improve the conditioning of the pb
// so that all sensitivities, i.e., all non-zero first partial derivatives, are
// typically of the order 0.1   10.
// -----
bool VPP_NLP::get_scaling_parameters(double& obj_scaling,
                                    bool& use_x_scaling, Index n,
																		double* x_scaling,
                                    bool& use_g_scaling, Index m,
																		double* g_scaling) {

	assert(n == dimension_);
	assert(m == subPbSize_);

	// Maximize the objective function by setting obj_scaling = -1
	obj_scaling= -1;

	// No other scaling required
	use_x_scaling= false;
	use_g_scaling= false;

	return true;
}

// Returns the initial point for the problem
bool VPP_NLP::get_starting_point(int n, bool init_x, double* x,
		bool init_z, double* z_L, double* z_U,
		int m, bool init_lambda,
		double* lambda) {

	// Here, we assume we only have starting values for x, if you code
	// your own NLP, you can provide starting values for the dual variables
	// if you wish
	assert(init_x == true);
	assert(init_z == false);
	assert(init_lambda == false);

	///////////// Transfer the code of VPPSolverBase::resetInitialGuess ///////////////
	// Use an Eigen::Map to manipulate the solution buffer x
	Eigen::Map<Eigen::RowVectorXd> xp(x,n);

	// In it to something small to start the evals at each velocity
	if(twv_==0) {

		// This is the very first solution, so we must guess a solution
		// but have nothing to establish our guess
		if(twa_==0)
			xp= xp0_;

		else

			// In this case we have a solution at a previous angle we can use
			// to set the guess
			if(!pResults_->get(twv_,twa_-1).discard())
				xp = *(pResults_->get(twv_,twa_-1).getX());
			else
				xp= xp0_;

	}
	else if( twv_==1 ) {

		///////////////////

		// IF twv==1 and twa==0, just take the solution of twv-1, 0
		// if this is acceptable. Otherwise restart from x0
		if(twa_==0)
			if(!pResults_->get(twv_-1,twa_).discard())
				xp= *(pResults_->get(twv_-1,twa_).getX());
			else
				xp= xp0_;

		// twv_==1 and twa_ > 0. Do as for twv_==0 : use the previous converged solution
		else

			if(!pResults_->get(twv_,twa_-1).discard())// In this case we have a solution at a previous angle we can use
				// to set the guess
				xp = *(pResults_->get(twv_,twa_-1).getX());
			else
				xp= xp0_;

		///////////////////
	}
	else if( twv_>1 ) {

		// For twv> 1 we can linearly predict the result of the state vector
		// we search for the last two converged results and we do discard the
		// diverged results, that would not serve our cause
		// Enclose by try-catch block in case getPreviousConverged does not find
		// a valid solution to be used for the linear guess
		try {

			size_t tmOne=getPreviousConverged(twv_,twa_);
			size_t tmTwo=getPreviousConverged(tmOne,twa_);

			Extrapolator extrapolator(
					pResults_->get(tmTwo,twa_).getTWV(),
					pResults_->get(tmTwo,twa_).getX(),
					pResults_->get(tmOne,twa_).getTWV(),
					pResults_->get(tmOne,twa_).getX()
			);

			// Extrapolate the state vector for the current wind
			// velocity. Note that the items have not been init yet
			xp= extrapolator.get( pVppItemsContainer_->getWind()->getTWV(twv_) );

		} catch( NoPreviousConvergedException& e){
			// do nothing
		}

		// Make sure the initial guess does not exceeds the bounds
		for(size_t i=0; i<lowerBounds_.size(); i++) {
			if(xp[i]<lowerBounds_[i]){
				std::cout<<"Resetting the lower bounds to "<<lowerBounds_[i]<<std::endl;
				xp[i]=lowerBounds_[i];
			}
			if(xp[i]>upperBounds_[i]){
				xp[i]=upperBounds_[i];
				std::cout<<"Resetting the upper bounds to "<<upperBounds_[i]<<std::endl;
			}
		}
	}

	std::cout<<"-->> solver first guess: "<<xp<<std::endl;

	return true;
}

// Recode this VPPSolverBase method here. To be removed when refactoring VPP_NLP
// as a child of VPPSolverBase
// Returns the index of the previous velocity-wise (twv) result that is marked as
// converged (discard==false). It starts from 'current', so it can be used recursively
size_t VPP_NLP::getPreviousConverged(size_t idx, size_t TWA) {

	while(idx){
		idx--;
		if(!pResults_->get(idx,TWA).discard())
			return idx;
	}

	throw NoPreviousConvergedException(HERE,"No previous converged index found");
}


// Returns the value of the objective function. This is the equivalent of VPP_Speed
bool VPP_NLP::eval_f(int n, const double* x, bool new_x, double& obj_value) {

	assert(n == dimension_);

	// Maximize speed
	obj_value = x[0];

	return true;
}

// Set twv and twa for this run
void VPP_NLP::setWind(size_t twv, size_t twa) {
	twa_= twa;
	twv_= twv;
}

// Make a printout of the results for this run
void VPP_NLP::printResults() {

	std::cout<<"==== VPP_NLP RESULTS PRINTOUT ==================="<<std::endl;
	pResults_->print();
	std::cout<<"---------------------------------------------------\n"<<std::endl;

}

// Return the gradient of the objective function grad_{x} f(x)
bool VPP_NLP::eval_grad_f(int n, const double* x, bool new_x, double* grad_f) {

	assert(n == dimension_);

	// Map the solution x to an Eigen object
	Eigen::Map<const Eigen::VectorXd> xMap(x,n);

	// Run the Gradient to compute the derivatives
	pGradient_->run(xMap,twv_,twa_);

	// Copy the values of the gradient to the c-style array
	for(size_t iVar=0; iVar<n; iVar++)
		grad_f[iVar] = pGradient_->coeffRef(iVar);

	return true;
}

// Return the value of the constraints: g(x)
// I have two constraints: dF=0 and dM=0
bool VPP_NLP::eval_g(int n, const double* x0, bool new_x, int m, double* g) {

	assert(n == dimension_);
	assert(m == subPbSize_);

	// Update the items
	pVppItemsContainer_->update(twv_,twa_,x0);

	// Push the residuals to the ipOpt residual vector g
	pVppItemsContainer_->getResiduals(g[0],g[1]);

	return true;
}

// Return the structure or values of the jacobian of the constraint g
bool VPP_NLP::eval_jac_g(int n, const double* x0, bool new_x,
		int m, int nele_jac, int* iRow, int *jCol,
		double* values) {

	assert(n == dimension_);
	assert(m == nEqualityConstraints_);

	if (values == NULL) {

		// return the structure of the jacobian of the constraint
		// functions -- in this particular case it is dense
		iRow[0] = 0;
		jCol[0] = 0;
		iRow[1] = 0;
		jCol[1] = 1;
		iRow[2] = 0;
		jCol[2] = 2;
		iRow[3] = 0;
		jCol[3] = 3;
		iRow[4] = 1;
		jCol[4] = 0;
		iRow[5] = 1;
		jCol[5] = 1;
		iRow[6] = 1;
		jCol[6] = 2;
		iRow[7] = 1;
		jCol[7] = 3;
	}
	else {

		//  	// Return the values of the jacobian of the constraints
		//  	// the structure of which has been specified below

		// Transform the c-style container into an Eigen container
		Map<const VectorXd>x(x0,dimension_);
		Eigen::VectorXd xTmp(x);

		// Instantiate a VPPJacobian
		VPPJacobian J(xTmp,pVppItemsContainer_.get(), subPbSize_, dimension_);

		// Run the VPPJacobian and compute the derivatives
		J.run(twv_, twa_);

		// Copy the values of the derivatives into the buffer values
		for(size_t iCmp=0; iCmp<dimension_; iCmp++){
			values[iCmp]=J(0,iCmp);
			values[iCmp+dimension_]=J(1,iCmp);
		}
	}

	return true;
}

// Return the structure or values of the hessian
bool VPP_NLP::eval_h(int n, const double* x, bool new_x,
		double obj_factor, int m, const double* lambda,
		bool new_lambda, int nele_hess, int* iRow,
		int* jCol, double* values) {

	// Main sets the flag "hessian_approximation" to "limited-memory", so it should
	// not require the hessian matrix
	throw VPPException(HERE, "The hessian should not be requested!");

	return true;
}

void VPP_NLP::finalize_solution(SolverReturn status,
		int n, const double* x, const double* z_L, const double* z_U,
		int m, const double* g, const double* lambda,
		double obj_value,
		const IpoptData* ip_data,
		IpoptCalculatedQuantities* ip_cq) {

	// Here is where we would store the solution to variables, or write to a file, etc
	// so we could use the solution.

	// For this example, we write the solution to the console
	std::cout << std::endl << std::endl << "Solution of the state variables, x" << std::endl;
	for (int i=0; i<n; i++)
		std::cout <<  x[i] << "  ";
	std::cout<<"\n";

	// Map the c-style container x to an Eigen object
	Eigen::Map<const Eigen::VectorXd> solutionMap(x,dimension_);
	Eigen::VectorXd solution(solutionMap);

	// Map the c-style container g to an Eigen object
	Eigen::Map<const Eigen::VectorXd> residualMap(g,nEqualityConstraints_);
	Eigen::VectorXd residuals(residualMap);

	bool discard=false;
	for(size_t i=0; i<solutionMap.size(); i++){
		if( solutionMap(i)<lowerBounds_[i] || solutionMap(i)>upperBounds_[i] )
			discard=true;
	}

	// Push the solution to the result container
	pResults_->push_back(twv_, twa_, solution, residuals, discard);


//	std::cout << std::endl << std::endl << "Solution of the bound multipliers, z_L and z_U" << std::endl;
//	for (int i=0; i<n; i++) {
//		std::cout << "z_L[" << i << "] = " << z_L[i] << std::endl;
//	}
//	for (int i=0; i<n; i++) {
//		std::cout << "z_U[" << i << "] = " << z_U[i] << std::endl;
//	}

//	std::cout << std::endl << std::endl << "Objective value" << std::endl;
//	std::cout << "f(x*) = " << obj_value << std::endl;

	std::cout << std::endl << "Final value of the constraints:" << std::endl;
	for (int i=0; i<m ;i++)
		std::cout << g[i] << "  ";
	std::cout<<"\n";

}

// Make a printout of the results for this run
void VPP_NLP::plotXY(size_t iWa) {

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
void VPP_NLP::plotJacobian() {
	pSolver_->plotJacobian();
}

void VPP_NLP::plotGradient() {

	// Define a linearization point
	IOUtils io(pVppItemsContainer_->getWind());
	Eigen::VectorXd xp;
	io.askUserStateVector(xp);

	// Instantiate a Gradient
	VPPGradient G(xp,pVppItemsContainer_.get());

	// ask the user which awv, awa
	// For which TWV, TWA shall we plot the aero forces/moments?
	size_t twv=0, twa=0;
	io.askUserWindIndexes(twv, twa);

	// call jacobian.testPlot
	G.testPlot(twv, twa);

}

// Make a printout of the results for this run
void VPP_NLP::plotPolars() {

	// Instantiate a VPPPlotSet and sub-contract the plot
	VPPPlotSet plotSet(pResults_.get());
	plotSet.plotPolars();

}

// Reset the optimizer when reloading the initial data
void VPP_NLP::reset(boost::shared_ptr<VPPItemFactory> VPPItemFactory) {

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

// Read results from file and places them in the current results
void VPP_NLP::importResults() {

	std::cout<<"==== VPP_NLP RESULTS IMPORT... ==================="<<std::endl;
	VPPResultIO reader(pResults_.get());
	reader.read();
	std::cout<<"---------------------------------------------------\n"<<std::endl;

}

// Make a printout of the result bounds for this run
void VPP_NLP::printResultBounds() {

	std::cout<<"==== VPP_NLP RESULT BOUNDS PRINTOUT ==================="<<std::endl;
	pResults_->printBounds();
	std::cout<<"---------------------------------------------------\n"<<std::endl;

}

// Save the current results to file
void VPP_NLP::saveResults() {

	std::cout<<"==== VPP_NLP RESULTS SAVING... ==================="<<std::endl;
	VPPResultIO writer(pResults_.get());
	writer.write();
	std::cout<<"---------------------------------------------------\n"<<std::endl;

}
