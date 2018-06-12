#ifndef VPPGRADIENT_H
#define VPPGRADIENT_H

#include <Eigen/Core>
#include <Eigen/Dense>
using namespace Eigen;

#include "boost/shared_ptr.hpp"
#include "VPPItemFactory.h"
#include "NRSolver.h"

// Compute the Gradient vector:
//
// Grad(u) = | du/du du/dPhi  du/db  du/df  | =
// du = [ du/du  du/dPhi  du/db  du/df ] * [ du dPhi db df ]^T
//
// where the derivatives in the Gradient matrix are computed by
// centered finite differences:

class VPPGradient : public Eigen::VectorXd {

	public:

		/// Constructor
		VPPGradient(const VectorXd& x,VPPItemFactory* pVppItemsContainer );

		/// Set the operation point and run to compute the derivatives
		void run(const VectorXd& x, int twv, int twa);

		/// Compute this Gradient
		void run(int twv, int twa);

		/// Produces a plot for a range of values of the state variables
		/// in order to test for the coherence of the values that have been computed
		std::vector<VppXYCustomPlotWidget*> plot(WindIndicesDialog&);

		/// Destructor
		~VPPGradient();

	private:

		/// Const reference to the VPP state vector
		VectorXd x_;

		/// Vector with the very first initial guess for the current configuration
		/// Used to debug when the solver cannot produce a solution
		Eigen::VectorXd xp0_;

		/// Ptr to the vppItemContainer, to be called to update the items
		/// when computing the derivatives by finite difference
		VPPItemFactory* pVppItemsContainer_;

		/// NRSolver used to compute the derivatives of the objective function
		/// wrt the state variables, eg: the component of the Gradient vector
		/// Note that the component: du/dPhi requires resolving a mono-dimensional
		/// pb (instead of a 2d pb). This, because I compute du/dPhi fixing phi
		/// and finding u s.t. dF=0.
		/// In all of the other cases, the state vars u,Phi are free while the
		/// optimization variables are infinitesimally incremented or decremented
		boost::shared_ptr<NRSolver> pSolver_;

		/// Size of the complete optimization problem : u, phi, b, f.
		size_t size_;

};

#endif
