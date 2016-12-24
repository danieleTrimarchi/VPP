#ifndef VPPJACOBIAN_H
#define VPPJACOBIAN_H

#include <Eigen/Core>
#include <Eigen/Dense>
using namespace Eigen;

#include "boost/shared_ptr.hpp"
#include "VPPItemFactory.h"

/// Compute the Jacobian matrix:
///
/// J = | dF/du dF/dPhi |	|du	 | 	 |dF |    |0|
///	    | dM/du dM/dPhi |	|dPhi| = |dM | -> |0|
///
/// where the derivatives in the Jacobian matrix are computed by
/// centered finite differences
/// It is possible to compute a non-diagonal Jacobian used to feed ipOpt
///
/// J = | dF/du dF/dPhi dF/db dF/df |
///	    | dM/du dM/dPhi dM/db dM/df |
///
class VPPJacobian : public Eigen::MatrixXd {

	public:

		/// Constructor
		VPPJacobian(VectorXd& x,VPPItemFactory* pVppItemsContainer, size_t subProblemSize);

		/// Constructor for non square Jacobian
		VPPJacobian(VectorXd& x,VPPItemFactory* pVppItemsContainer, size_t subProblemSize, size_t nVars);

		/// Compute this Jacobian
		void run(int twv, int twa);

		/// Produces a test plot for a range of values of the state variables
		/// in order to test for the coherence of the values that have been computed
		void testPlot(int twv, int twa);

		/// Destructor
		~VPPJacobian();

	private:

		/// Const reference to the VPP state vector
		VectorXd& x_;

		/// Vector with the very first initial guess for the current configuration
		/// Used to debug when the solver cannot produce a solution
		Eigen::VectorXd xp0_;

		/// Ptr to the vppItemContainer, to be called to update the items
		/// when computing the derivatives by finite difference
		VPPItemFactory* pVppItemsContainer_;

		/// Size of the subproblem we aim to solve with the help of this NR
		/// the variables of the subProblem are the firsts in the state vector
		size_t subPbSize_;

		/// Size of the complete optimization problem : u, phi, b, f.
		size_t size_;
};

/// Class used to plot the Jacobian derivatives for a step that
/// has failed to converge. Note that the class Jacobian contains a
/// method 'test' used to check for the derivatives on a generic
/// interval. The JacobianChecker differs from that method because
/// it stores the actual iteration-wise history of Jacobian-solution
class JacobianChecker {

public:

	/// Constructor
	JacobianChecker();

	/// Destructor
	~JacobianChecker();

	/// Buffer a Jacobian-state vector
	void push_back(VPPJacobian& J, Eigen::VectorXd& x, Eigen::VectorXd& res );

	/// Plot the Jacobian-state vector for the stored history
	void testPlot();

private:

	/// Vector that buffers a Jacobian for each iteration
	std::vector<VPPJacobian> jacobians_;

	/// Vector that buffers the state vector x and the residual vector res
	/// for each iteration
	std::vector<Eigen::VectorXd> xs_, res_;

};

#endif
