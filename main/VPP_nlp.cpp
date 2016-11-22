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
				pParser_(NULL){

}

// Constructor with ptr to then VPPItemFactory
VPP_NLP::VPP_NLP(boost::shared_ptr<VPPItemFactory> pVppItemsContainer):
				pVppItemsContainer_(pVppItemsContainer),
				dimension_(4),  /* v, phi, crew, flat */
				subPbSize_(2),  /* v, phi */
				nEqualityConstraints_(2),
				pParser_(pVppItemsContainer->getParser()) {

	// Instantiate a nrSolver that will be used to compute the gradient
	// of the objective function : the vector [ du/du du/dPhi du/db du/df ]
	pSolver_.reset(new NRSolver(pVppItemsContainer.get(),dimension_,subPbSize_) );

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
	nnz_jac_g = dimension_*nEqualityConstraints_;

	// The hessian is also dense and has 16 total non-zeros, but
	// a priori we do not need it because we have set
	// hessian_approximation","limited-memory"
	nnz_h_lag = dimension_*dimension_;

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
		x_l[i] = upperBounds_[i];

	// Bounds on the constraints : as we only have equality constraints we set the
	// upper and lower bound to the same value.
	// It could be the case that relaxing this condition helps convergence.
	// test todo dtrimarchi
	g_l[0] = g_u[0] = 0.;
	g_l[1] = g_u[1] = 0.;

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
	Eigen::Map<Eigen::VectorXd> xp(x,n);

	// In it to something small to start the evals at each velocity
	if(twv_==0) {

		// This is the very first solution, so we must guess a solution
		// but have nothing to establish our guess
		if(twa_==0)
			xp= xp0_;

		else

			if(!pResults_->get(twv_,twa_-1).discard())// In this case we have a solution at a previous angle we can use
				// to set the guess
				xp = *(pResults_->get(twv_,twa_-1).getX());
			else
				xp= xp0_;

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

	std::cout<<"-->> solver first guess: "<<xp.transpose()<<std::endl;

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
	obj_value = -x[0];

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

	// Buffer the solution vector x to be used in Eigen world

	// grad(u) = [ du/du du/dPhi du/db du/df ]
	//         = [ 1 du/dPhi du/db du/df ]
	// This requires being able to compute u when all of the other quantities are fixed.
	// It requires a simple NR 1d
	// f(x) = f(x0) + f'(x0) (x-x0) = 0 => x = x0 - f(x0) / f'(x0)
	// so u(Phi) = u(Phi0) + u'(Phi0) (Phi-Phi0) => u = u0 - u(Phi0) / u'(Phi0)
	// u(Phi0)=
	grad_f[0] = 1;

	// Compute du/dPhi, du/db and du/df
	grad_f[1] = computederivative(x,1);
	grad_f[2] = computederivative(x,2);
	grad_f[3] = computederivative(x,3);

	return true;
}

// du/dPhi = u(phi2) - u(Phi1) / dPhi
// where u(phi1), u(phi2) are computed with 1d NR
// du/db, du/df are computed in a similar way but using
// a 2d Newton-Raphson
double VPP_NLP::computederivative(const double* x0, size_t pos) {

	// Transform the c-style container into an Eigen container
	Map<const VectorXd>x(x0,dimension_);

	// Compute the optimum eps for this variable
	double eps=std::sqrt( std::numeric_limits<double>::epsilon() );

	// Init a buffer of the state vector xp_
	VectorXd xp=x, xm=x;

	// Reset the subPBsize based on the variable we are computing for.
	// The subPbSize is 1 when computing du/dPhi, as this is not an
	// optimization variable and we want effectively to find u st: dF=0
	// while we enforce Phi (and thus dM!=0). Not so for the other derivatives
	// where we fix optimization variables and want to assure the equilibrium
	// dF=0, dM=0.
	if(pos==1)
		pSolver_->setSubPbSize(1);
	else
		pSolver_->setSubPbSize(2);

	// set x= x + eps
	xp(pos) = xp(pos) * ( 1 + eps );

	xp.block(0,0,1,1)= pSolver_->run(twv_,twa_,xp).block(0,0,1,1);

	// set x= x + eps
	xm(pos) = xm(pos) * ( 1 - eps );

	// Compute u_m s.t. dF=0
	xm.block(0,0,1,1)= pSolver_->run(twv_,twa_,xm).block(0,0,1,1);

	// Return du / dPhi = ( u_p - u_m ) / ( 2 * eps )
	return ( xp(0) - xm(0) ) / ( 2 * eps);

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

	if (values == NULL) {
		// Return the structure. This is a symmetric matrix, fill the lower left
		// triangle only.

		// The hessian for this problem is actually dense
		int idx=0;
		for (int row = 0; row < 4; row++) {
			for (int col = 0; col <= row; col++) {
				iRow[idx] = row;
				jCol[idx] = col;
				idx++;
			}
		}

		assert(idx == nele_hess);
	}
	else {
		// Return the values. This is a symmetric matrix, fill the lower left
		// triangle only

		// Fill the objective portion
		values[0] = obj_factor * (2*x[3]); // 0,0

		values[1] = obj_factor * (x[3]);   // 1,0
		values[2] = 0.;                    // 1,1

		values[3] = obj_factor * (x[3]);   // 2,0
		values[4] = 0.;                    // 2,1
		values[5] = 0.;                    // 2,2

		values[6] = obj_factor * (2*x[0] + x[1] + x[2]); // 3,0
		values[7] = obj_factor * (x[0]);                 // 3,1
		values[8] = obj_factor * (x[0]);                 // 3,2
		values[9] = 0.;                                  // 3,3


		// Add the portion for the first constraint
		values[1] += lambda[0] * (x[2] * x[3]); // 1,0

		values[3] += lambda[0] * (x[1] * x[3]); // 2,0
		values[4] += lambda[0] * (x[0] * x[3]); // 2,1

		values[6] += lambda[0] * (x[1] * x[2]); // 3,0
		values[7] += lambda[0] * (x[0] * x[2]); // 3,1
		values[8] += lambda[0] * (x[0] * x[1]); // 3,2

		// Add the portion for the second constraint
		values[0] += lambda[1] * 2; // 0,0

		values[2] += lambda[1] * 2; // 1,1

		values[5] += lambda[1] * 2; // 2,2

		values[9] += lambda[1] * 2; // 3,3
	}

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
	std::cout << std::endl << std::endl << "Solution of the primal variables, x" << std::endl;
	for (int i=0; i<n; i++) {
		std::cout << "x[" << i << "] = " << x[i] << std::endl;
	}

	// Map the c-style container x to an Eigen object
	Eigen::Map<const Eigen::VectorXd> solutionMap(x,dimension_);
	Eigen::VectorXd solution(solutionMap);

	// Map the c-style container g to an Eigen object
	Eigen::Map<const Eigen::VectorXd> resMap(g,nEqualityConstraints_);
	Eigen::VectorXd residuals(resMap);

	bool discard=false;
	for(size_t i=0; i<solutionMap.size(); i++){
		if( solutionMap(i)<lowerBounds_[i] || solutionMap(i)>upperBounds_[i] )
			discard=true;
	}

	// Push the solution to the result container
	pResults_->push_back(twv_, twa_, solution, residuals, discard);


	std::cout << std::endl << std::endl << "Solution of the bound multipliers, z_L and z_U" << std::endl;
	for (int i=0; i<n; i++) {
		std::cout << "z_L[" << i << "] = " << z_L[i] << std::endl;
	}
	for (int i=0; i<n; i++) {
		std::cout << "z_U[" << i << "] = " << z_U[i] << std::endl;
	}

	std::cout << std::endl << std::endl << "Objective value" << std::endl;
	std::cout << "f(x*) = " << obj_value << std::endl;

	std::cout << std::endl << "Final value of the constraints:" << std::endl;
	for (int i=0; i<m ;i++) {
		std::cout << "g(" << i << ") = " << g[i] << std::endl;
	}
}
