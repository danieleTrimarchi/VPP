// Copyright (C) 2005, 2006 International Business Machines and others.
// All Rights Reserved.
// This code is published under the Eclipse Public License.
//
// $Id: hs071_nlp.cpp 2005 2011-06-06 12:55:16Z stefan $
//
// Authors:  Carl Laird, Andreas Waechter     IBM    2005-08-16

#include "hs071_nlp.h"

#include <cassert>
#include <iostream>

using namespace Ipopt;

// constructor
HS071_NLP::HS071_NLP():
		dimension_(4),
		nEqualityConstraints_(2),
		objectiveVaue_(0),
		solution_(Eigen::VectorXd::Zero(dimension_)),
		constraints_(Eigen::VectorXd::Zero(nEqualityConstraints_)) {

}

//destructor
HS071_NLP::~HS071_NLP()
{}

// returns the size of the problem
bool HS071_NLP::get_nlp_info(Ipopt::Index& n, Ipopt::Index& m, Ipopt::Index& nnz_jac_g,
                             Ipopt::Index& nnz_h_lag, IndexStyleEnum& index_style) {

  // The problem described in HS071_NLP.hpp has 4 variables, x[0] through x[3]
  n = dimension_;

  // one equality constraint and one inequality constraint
  m = nEqualityConstraints_;

  // The jacobian of the constraints is dense and contains 8 nonzeros
  // dg1/dx1 dg1/dx2 ...
  // dg2/dx1 ...
  nnz_jac_g = 8;

  // the hessian is also dense and has 16 total nonzeros, but we
  // only need the lower left corner (since it is symmetric)
  nnz_h_lag = 10;

  // use the C style indexing (0-based)
  index_style = TNLP::C_STYLE;

  return true;
}

// returns the variable bounds
bool HS071_NLP::get_bounds_info(Ipopt::Index n, Number* x_l, Number* x_u,
                                Ipopt::Index m, Number* g_l, Number* g_u)
{
  // here, the n and m we gave IPOPT in get_nlp_info are passed back to us.
  // If desired, we could assert to make sure they are what we think they are.
  assert(n == dimension_);
  assert(m == nEqualityConstraints_);

  // the variables have lower bounds of 1
  for (Ipopt::Index i=0; i<4; i++) {
    x_l[i] = 1.0;
  }

  // the variables have upper bounds of 5
  for (Ipopt::Index i=0; i<4; i++) {
    x_u[i] = 5.0;
  }

  // the first constraint g1 has a lower bound of 25
  g_l[0] = 25;
  // the first constraint g1 has NO upper bound, here we set it to 2e19.
  // Ipopt interprets any number greater than nlp_upper_bound_inf as
  // infinity. The default value of nlp_upper_bound_inf and nlp_lower_bound_inf
  // is 1e19 and can be changed through ipopt options.
  g_u[0] = 2e19;

  // the second constraint g2 is an equality constraint, so we set the
  // upper and lower bound to the same value
  g_l[1] = g_u[1] = 40.0;

  return true;
}

// returns the initial point for the problem
bool HS071_NLP::get_starting_point(Ipopt::Index n, bool init_x, Number* x,
                                   bool init_z, Number* z_L, Number* z_U,
                                   Ipopt::Index m, bool init_lambda,
                                   Number* lambda)
{
  // Here, we assume we only have starting values for x, if you code
  // your own NLP, you can provide starting values for the dual variables
  // if you wish
  assert(init_x == true);
  assert(init_z == false);
  assert(init_lambda == false);

  // initialize to the given starting point
  x[0] = 1.0;
  x[1] = 5.0;
  x[2] = 5.0;
  x[3] = 1.0;

  return true;
}

// returns the value of the objective function
bool HS071_NLP::eval_f(Ipopt::Index n, const Number* x, bool new_x, Number& obj_value)
{
  assert(n == dimension_);

  obj_value = x[0] * x[3] * (x[0] + x[1] + x[2]) + x[2];

  return true;
}

// return the gradient of the objective function grad_{x} f(x)
bool HS071_NLP::eval_grad_f(Ipopt::Index n, const Number* x, bool new_x, Number* grad_f)
{
  assert(n == dimension_);

  grad_f[0] = x[0] * x[3] + x[3] * (x[0] + x[1] + x[2]);
  grad_f[1] = x[0] * x[3];
  grad_f[2] = x[0] * x[3] + 1;
  grad_f[3] = x[0] * (x[0] + x[1] + x[2]);

  return true;
}

// return the value of the constraints: g(x)
bool HS071_NLP::eval_g(Ipopt::Index n, const Number* x, bool new_x, Ipopt::Index m, Number* g)
{
  assert(n == dimension_);
  assert(m == nEqualityConstraints_);

  g[0] = x[0] * x[1] * x[2] * x[3];
  g[1] = x[0]*x[0] + x[1]*x[1] + x[2]*x[2] + x[3]*x[3];

  return true;
}

// return the structure or values of the jacobian
bool HS071_NLP::eval_jac_g(Ipopt::Index n, const Number* x, bool new_x,
                           Ipopt::Index m, Ipopt::Index nele_jac, Ipopt::Index* iRow, Ipopt::Index *jCol,
                           Number* values)
{
  if (values == NULL) {
    // return the structure of the jacobian

    // this particular jacobian is dense
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
    // return the values of the jacobian of the constraints

    values[0] = x[1]*x[2]*x[3]; // 0,0
    values[1] = x[0]*x[2]*x[3]; // 0,1
    values[2] = x[0]*x[1]*x[3]; // 0,2
    values[3] = x[0]*x[1]*x[2]; // 0,3

    values[4] = 2*x[0]; // 1,0
    values[5] = 2*x[1]; // 1,1
    values[6] = 2*x[2]; // 1,2
    values[7] = 2*x[3]; // 1,3
  }

  return true;
}

//return the structure or values of the hessian
bool HS071_NLP::eval_h(Ipopt::Index n, const Number* x, bool new_x,
                       Number obj_factor, Ipopt::Index m, const Number* lambda,
                       bool new_lambda, Ipopt::Index nele_hess, Ipopt::Index* iRow,
                       Ipopt::Index* jCol, Number* values)
{
  if (values == NULL) {
    // return the structure. This is a symmetric matrix, fill the lower left
    // triangle only.

    // the hessian for this problem is actually dense
    Ipopt::Index idx=0;
    for (Ipopt::Index row = 0; row < 4; row++) {
      for (Ipopt::Index col = 0; col <= row; col++) {
        iRow[idx] = row;
        jCol[idx] = col;
        idx++;
      }
    }

    assert(idx == nele_hess);
  }
  else {
    // return the values. This is a symmetric matrix, fill the lower left
    // triangle only

    // fill the objective portion
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


    // add the portion for the first constraint
    values[1] += lambda[0] * (x[2] * x[3]); // 1,0

    values[3] += lambda[0] * (x[1] * x[3]); // 2,0
    values[4] += lambda[0] * (x[0] * x[3]); // 2,1

    values[6] += lambda[0] * (x[1] * x[2]); // 3,0
    values[7] += lambda[0] * (x[0] * x[2]); // 3,1
    values[8] += lambda[0] * (x[0] * x[1]); // 3,2

    // add the portion for the second constraint
    values[0] += lambda[1] * 2; // 0,0

    values[2] += lambda[1] * 2; // 1,1

    values[5] += lambda[1] * 2; // 2,2

    values[9] += lambda[1] * 2; // 3,3
  }

  return true;
}

void HS071_NLP::finalize_solution(SolverReturn status,
                                  Ipopt::Index n, const Number* x, const Number* z_L, const Number* z_U,
                                  Ipopt::Index m, const Number* g, const Number* lambda,
                                  Number obj_value,
				  const IpoptData* ip_data,
				  IpoptCalculatedQuantities* ip_cq)
{
  // here is where we would store the solution to variables, or write to a file, etc
  // so we could use the solution.

  // Map the c-style container x to an Eigen object
  Eigen::Map<const Eigen::VectorXd> solutionMap(x,dimension_);
  solution_= solutionMap;

  // Map the c-style container g to an Eigen object
  Eigen::Map<const Eigen::VectorXd> resMap(g,nEqualityConstraints_);
  constraints_= resMap;

  // Stores the objective value
  objectiveVaue_= obj_value;

  // For this example, we write the solution to the console
//  std::cout << std::endl << std::endl << "Solution of the primal variables, x" << std::endl;
//  for (Ipopt::Index i=0; i<n; i++) {
//     std::cout << "x[" << i << "] = " << x[i] << std::endl;
//  }
//
//  std::cout << std::endl << std::endl << "Solution of the bound multipliers, z_L and z_U" << std::endl;
//  for (Ipopt::Index i=0; i<n; i++) {
//    std::cout << "z_L[" << i << "] = " << z_L[i] << std::endl;
//  }
//  for (Ipopt::Index i=0; i<n; i++) {
//    std::cout << "z_U[" << i << "] = " << z_U[i] << std::endl;
//  }
//
//  std::cout << std::endl << std::endl << "Objective value" << std::endl;
//  std::cout << "f(x*) = " << obj_value << std::endl;
//
//  std::cout << std::endl << "Final value of the constraints:" << std::endl;
//  for (Ipopt::Index i=0; i<m ;i++) {
//    std::cout << "g(" << i << ") = " << g[i] << std::endl;
//  }
}

// Returns the dimensionality of this pb, or the number of variables (4)
size_t HS071_NLP::getDimension() const {
	return dimension_;
}

// Returns the solution vector
Eigen::VectorXd HS071_NLP::getSolution() const {
	return solution_;
}

// Returns the residuals vector
Eigen::VectorXd HS071_NLP::getConstraints() const {
	return constraints_;
}

// Returns the value of the objective function
double HS071_NLP::getObjectiveValue() const {
	return objectiveVaue_;
}


