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
#include "VPPSolverBase.h"
#include "NRSolver.h"

namespace Optim {

// forward declaration
class IppOptSolverFactory;

/// VPP application for solving the optimization problem with IPOPT
///  The class is directly inspired by hs071_nlp, which implements a
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

class VPP_NLP : public TNLP, public VPPSolverBase {

	public:

		/// Default destructor
		virtual ~VPP_NLP();

		/// Method to return some info about the nlp
		virtual bool get_nlp_info(Ipopt::Index& n, Ipopt::Index& m, Ipopt::Index& nnz_jac_g,
				Ipopt::Index& nnz_h_lag, IndexStyleEnum& Index_style);

		/// Method to return the bounds for my problem
		virtual bool get_bounds_info(Ipopt::Index n, Number* x_l, Number* x_u,
				Ipopt::Index m, Number* g_l, Number* g_u);

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
				bool& use_x_scaling, Ipopt::Index n,
				Number* x_scaling,
				bool& use_g_scaling, Ipopt::Index m,
				Number* g_scaling);

		/// Method to return the starting point for the algorithm
		virtual bool get_starting_point(Ipopt::Index n, bool init_x, Number* x,
				bool init_z, Number* z_L, Number* z_U,
				Ipopt::Index m, bool init_lambda,
				Number* lambda);

		/// Method to return the objective value
		virtual bool eval_f(Ipopt::Index n, const Number* x, bool new_x, Number& obj_value);

		/// Method to return the gradient of the objective
		virtual bool eval_grad_f(Ipopt::Index n, const Number* x, bool new_x, Number* grad_f);

		/// Method to return the constraint residuals
		virtual bool eval_g(Ipopt::Index n, const Number* x, bool new_x, Ipopt::Index m, Number* g);

		/// Method to return:
		///  1) The structure of the jacobian (if "values" is NULL)
		///  2) The values of the jacobian (if "values" is not NULL)
		virtual bool eval_jac_g(Ipopt::Index n, const Number* x, bool new_x,
				Ipopt::Index m, Ipopt::Index nele_jac, Ipopt::Index* iRow, Ipopt::Index *jCol,
				Number* values);

		/// Method to return:
		///   1) The structure of the hessian of the lagrangian (if "values" is NULL)
		///   2) The values of the hessian of the lagrangian (if "values" is not NULL)
		virtual bool eval_h(Ipopt::Index n, const Number* x, bool new_x,
				Number obj_factor, Ipopt::Index m, const Number* lambda,
				bool new_lambda, Ipopt::Index nele_hess, Ipopt::Index* iRow,
				Ipopt::Index* jCol, Number* values);

		/// This method is called when the algorithm is complete so the TNLP can store/write the solution
		virtual void finalize_solution(SolverReturn status,
				Ipopt::Index n, const Number* x, const Number* z_L, const Number* z_U,
				Ipopt::Index m, const Number* g, const Number* lambda,
				Number obj_value,
				const IpoptData* ip_data,
				IpoptCalculatedQuantities* ip_cq);

		/// Set twv and twa for this run
		void run(int twv, int twa);

	private:

		/// This class is to be instantiated using a NLOptSolverFactory.
		/// The friendship allows the factory to call the private constructor
		friend class IppOptSolverFactory;

		/// Private constructor - the class can only be instantiated using
		/// a VPPSolverFactory
		VPP_NLP(std::shared_ptr<VPPItemFactory>);

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

		/// Number of equality constraints: dF=0, dM=0
		const size_t nEqualityConstraints_; // --> v, phi

		/// Wind angle and velocity Ipopt::Indexes. Set with setWind(size_t, size_t)
		static size_t twa_, twv_;

};
}

#endif
