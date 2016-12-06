// Copyright (C) 2005, 2007 International Business Machines and others.
// All Rights Reserved.
// This code is published under the Eclipse Public License.
//
// $Id: VPP_NLP.hpp 1861 2010-12-21 21:34:47Z andreasw $
//
// Authors:  Carl Laird, Andreas Waechter     IBM    2005-08-09

#ifndef VPP_NLP_H__
#define VPP_NLP_H__

#include "IpTNLP.hpp"
using namespace Ipopt;

#include "VPPItemFactory.h"
#include "NRSolver.h"
#include "VPPGradient.h"

/// VPP application for solving the optimization problem with IPOPT
///  The class is direcly inspired by hs071_nlp, which implements a
///  C++ example of problem 71 of the Hock-Schittkowski test suite.
///
/// Problem hs071 looks like this
///
///     min   x1*x4*(x1 + x2 + x3)  +  x3
///     s.t.  x1*x2*x3*x4                   >=  25
///           x1**2 + x2**2 + x3**2 + x4**2  =  40
///           1 <=  x1,x2,x3,x4  <= 5
///
///     Starting point:
///        x = (1, 5, 5, 1)
///
///     Optimal solution:
///        x = (1.00000000, 4.74299963, 3.82114998, 1.37940829)
///

class VPP_NLP : public TNLP
{
	public:

		/// Default constructor
		VPP_NLP(boost::shared_ptr<VPPItemFactory>);

		/// Default destructor
		virtual ~VPP_NLP();

		/// Method to return some info about the nlp
		virtual bool get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
				Index& nnz_h_lag, IndexStyleEnum& index_style);

		/// Method to return the bounds for my problem
		virtual bool get_bounds_info(Index n, Number* x_l, Number* x_u,
				Index m, Number* g_l, Number* g_u);

		/// Overrides Ipopt::TPNLP get_scaling_parameters. Provide scaling
		/// factors for the objective function as well as for the optimization
		/// variables and/or constraints. The return value should be true,
		/// unless an error occurred, and the program is to be aborted.
		/// This method is called if the nlp scaling method is chosen as "user-scaling".
		/// if the value of obj_scaling is negative, then Ipopt will maximize
		/// the objective function instead of minimizing it
		/// -----
		/// Note that this method should be used to improve the conditioning of the pb
		/// so that all sensitivities, i.e., all non-zero first partial derivatives, are
		/// typically of the order 0.1   10.
		/// -----
		virtual bool get_scaling_parameters(Number& obj_scaling,
				bool& use_x_scaling, Index n,
				Number* x_scaling,
				bool& use_g_scaling, Index m,
				Number* g_scaling);

		/// Method to return the starting point for the algorithm
		virtual bool get_starting_point(Index n, bool init_x, Number* x,
				bool init_z, Number* z_L, Number* z_U,
				Index m, bool init_lambda,
				Number* lambda);

		/// Method to return the objective value
		virtual bool eval_f(Index n, const Number* x, bool new_x, Number& obj_value);

		/// Method to return the gradient of the objective
		virtual bool eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f);

		/// Method to return the constraint residuals
		virtual bool eval_g(Index n, const Number* x, bool new_x, Index m, Number* g);

		/// Method to return:
		///  1) The structure of the jacobian (if "values" is NULL)
		///  2) The values of the jacobian (if "values" is not NULL)
		virtual bool eval_jac_g(Index n, const Number* x, bool new_x,
				Index m, Index nele_jac, Index* iRow, Index *jCol,
				Number* values);

		/// Method to return:
		///   1) The structure of the hessian of the lagrangian (if "values" is NULL)
		///   2) The values of the hessian of the lagrangian (if "values" is not NULL)
		virtual bool eval_h(Index n, const Number* x, bool new_x,
				Number obj_factor, Index m, const Number* lambda,
				bool new_lambda, Index nele_hess, Index* iRow,
				Index* jCol, Number* values);

		/// This method is called when the algorithm is complete so the TNLP can store/write the solution
		virtual void finalize_solution(SolverReturn status,
				Index n, const Number* x, const Number* z_L, const Number* z_U,
				Index m, const Number* g, const Number* lambda,
				Number obj_value,
				const IpoptData* ip_data,
				IpoptCalculatedQuantities* ip_cq);

		/// Set twv and twa for this run
		void setWind(size_t twv, size_t twa);

		/// Make a printout of the results for this run
		/// TODO dtrimarchi : shift this to a mother class
		void printResults();

		/// Make a printout of the results for this run
		void plotXY(size_t iWa);

		/// Add this method for compatibility with the NR solver.
		/// TODO dtrimarchi: this could go to a common parent class
		void plotJacobian();

		/// Plot the jacobian vector component (sanity check)
		/// TODO dtrimarchi: this could go to a common parent class
		void plotGradient();

		/// Make a printout of the results for this run
		void plotPolars();

		/// Reset the optimizer when reloading the initial data
		void reset(boost::shared_ptr<VPPItemFactory> VPPItemFactory);

		/// Read results from file and places them in the current results
		void importResults();

		/// Save the current results to file
		void saveResults();

		/// Make a printout of the result bounds for this run
		void printResultBounds();

	private:

		///@name Methods to block default compiler methods.
		/// The compiler automatically generates the following three methods.
		///  Since the default compiler implementation is generally not what
		///  you want (for all but the most simple classes), we usually
		///  put the declarations of these methods in the private section
		///  and never implement them. This prevents the compiler from
		///  implementing an incorrect "default" behavior without us
		///  knowing. (See Scott Meyers book, "Effective C++")

		/// Default constructor
		VPP_NLP();

		/// Copy constructor
		VPP_NLP(const VPP_NLP&);

		/// Assignment operator
		VPP_NLP& operator=(const VPP_NLP&);

		/// Recode this VPPSolverBase method here. To be removed when refactoring VPP_NLP
		/// as a child of VPPSolverBase
		/// Returns the index of the previous velocity-wise (twv) result that is marked as
		/// converged (discard==false). It starts from 'current', so it can be used recursively
		size_t getPreviousConverged(size_t idx, size_t TWA);

		/// Ptr to the VPPItemFactory that contains all of the ingredients
		/// required to compute the optimization constraints
		boost::shared_ptr<VPPItemFactory> pVppItemsContainer_;

		/// Size of the problem this Optimizer is handling
		size_t dimension_; // --> v, phi, crew, flat

		/// Size of the sub-problem to be pre-solved with the NRSolver
		size_t subPbSize_; // --> v, phi

		/// Number of equality constraints: dF=0, dM=0
		const size_t nEqualityConstraints_; // --> v, phi

		/// Ptr to the parser that contains all of the user variables
		VariableFileParser* pParser_;

		/// Wind angle and velocity indexes. Set with setWind(size_t, size_t)
		static size_t twa_, twv_;

		/// NRSolver used to compute the derivatives of the objective function
		/// wrt the state variables, i.e: the component of the Gradient vector
		/// Note that the component du/dPhi is not computed with this NRSolver,
		/// because in this case I resolve a mono-dimensional pb (instead of a 2d pb)
		/// This, because I compute du/dPhi fixing phi and finding u s.t. dF=0.
		/// In all of the other cases, the state vars u,Phi are free while the
		/// optimization variables are infinitesimally incremented or decremented
		boost::shared_ptr<NRSolver> pSolver_;

		/// VPPGradient used to compute the gradient by finite differences
		/// and feed the nlp solver
		boost::shared_ptr<VPPGradient> pGradient_;

		/// Matrix of results, one result per wind velocity/angle
		boost::shared_ptr<ResultContainer> pResults_;

		/// Ptr to the wind
		WindItem* pWind_;

		/// lower and upper bounds for the state variables
		std::vector<double> lowerBounds_, upperBounds_;

		/// Declare a static const initial guess state vector
		static Eigen::VectorXd xp0_;

};


#endif
