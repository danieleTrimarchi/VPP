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

		/// Produces a plot for a range of values of the state variables
		/// in order to test for the coherence of the values that have been computed
		std::vector<VppXYCustomPlotWidget*> plot(WindIndicesDialog&);

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

#endif
