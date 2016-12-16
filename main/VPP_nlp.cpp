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

namespace Optim {

// Declare and init static members
size_t VPP_NLP::twa_=0, VPP_NLP::twv_=0;

//////////////

// Disallowed default constructor
VPP_NLP::VPP_NLP():
		nEqualityConstraints_(0) {

}

// Constructor with ptr to then VPPItemFactory
VPP_NLP::VPP_NLP(boost::shared_ptr<VPPItemFactory> pVppItemsContainer):
		VPPSolverBase(pVppItemsContainer),
				nEqualityConstraints_(2) {

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

	// Update the value of the current state vector
	xp_=xp;

	// Call the parent class method to make a guess of the solution
	resetInitialGuess(twv_, twa_);

	// Do NOT use the NR solve to make the guess more realistic
	// For some reason, attemtping to use the solver leads Newton
	// to divergence... It is not clear to me why, though...
	// solveInitialGuess(twv_, twa_);

	// Copy the current state vector to the c-style ipOpt buffer
	// not sure if this is required or if I could get out of this
	// with the map
	for(size_t i=0; i<xp_.size(); i++)
		x[i]=xp_(i);

	return true;
}

// Returns the value of the objective function. This is the equivalent of VPP_Speed
bool VPP_NLP::eval_f(int n, const double* x, bool new_x, double& obj_value) {

	assert(n == dimension_);

	// Maximize speed
	obj_value = x[0];

	return true;
}

// Set twv and twa for this run
void VPP_NLP::run(int twv, int twa) {
	twa_= twa;
	twv_= twv;
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

//	std::cout << std::endl << "Final value of the constraints:" << std::endl;
//	for (int i=0; i<m ;i++)
//		std::cout << g[i] << "  ";
//	std::cout<<"\n";

}

}
