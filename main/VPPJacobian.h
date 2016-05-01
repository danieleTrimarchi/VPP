#ifndef VPPJACOBIAN_H
#define VPPJACOBIAN_H

// Compute the Jacobian matrix:
//
// J = | dF /du dF /dPhi  dF/db  dF/df|	|du	 | 	 |dF |    |0|
//	   | dM /du dM /dPhi  dM/db  dM/df|	|dPhi| = |dM | -> |0|
//	   | dC1/du dC1/dPhi dC1/db dC1/df|	|db	 |	 |dC1|	  |0|
//	   | dC2/du dC2/dPhi dC2/db dC2/df|	|df	 |	 |dC2|    |0|
//
// where the derivatives in the Jacobian matrix are computed by
// centered finite differences:

#include <Eigen/Core>
#include <Eigen/Dense>
using namespace Eigen;

#include "boost/shared_ptr.hpp"
#include "VPPItemFactory.h"

/// Compute the Jacobian matrix:
///
/// J = | dF /du dF /dPhi  dF/db  dF/df|	|du	 | 	 |dF |    |0|
///	    | dM /du dM /dPhi  dM/db  dM/df|	|dPhi| = |dM | -> |0|
///	    | dC1/du dC1/dPhi dC1/db dC1/df|	|db	 |	 |dC1|	  |0|
///	    | dC2/du dC2/dPhi dC2/db dC2/df|	|df	 |	 |dC2|    |0|
///
/// where the derivatives in the Jacobian matrix are computed by
/// centered finite differences
class VPPJacobian : public Eigen::MatrixXd {

	public:

		/// Constructor
		VPPJacobian(VectorXd& x,boost::shared_ptr<VPPItemFactory> vppItemsContainer);

		/// Compute this Jacobian
		void run(Eigen::VectorXd& x, int twv, int twa);

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
		boost::shared_ptr<VPPItemFactory> vppItemsContainer_;

};

/// Class used to plot the Jacobian derivatives for a step that
/// has failed to converge. Note that the Jacobian contains a
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
