#include "VPPJacobian.h"

// Constructor
VPPJacobian::VPPJacobian(VectorXd& x,boost::shared_ptr<VPPItemFactory> vppItemsContainer):
x_(x),
vppItemsContainer_(vppItemsContainer) {

	// Resize this Jacobian to the size of the state vector
	resize(x_.rows(),x_.rows());

}

void VPPJacobian::run(int twv, int twa) {

	// loop on the state variables
	for(size_t iVar=0; iVar<x_.rows(); iVar++) {

		// Compute the optimum eps for this variable
		double eps= x_(iVar) * std::sqrt( std::numeric_limits<double>::epsilon() );

		// Init a buffer of the state vector xp_
		VectorXd xp(x_);

		// set x= x + eps
		xp(iVar) = x_(iVar) + eps;

		// compile the ith column of the Jacobian matrix with the
		// residuals for x_plus_epsilon
		col(iVar) = vppItemsContainer_->getResiduals(twv,twa,xp);

		// set x= x - eps
		xp(iVar) = x_(iVar) - eps;

		// compile the ith column of the Jacobian matrix subtracting the
		// residuals for x_minus_epsilon
		col(iVar) -= vppItemsContainer_->getResiduals(twv,twa,xp);

		// divide the column of the Jacobian by 2*eps
		col(iVar) /= ( 2 * eps );

	}

}


// Destructor
VPPJacobian::~VPPJacobian(){
	// make nothing
}
