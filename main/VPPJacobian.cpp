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

		// compile the i-th column of the Jacobian matrix with the
		// residuals for x_plus_epsilon
		col(iVar) = vppItemsContainer_->getResiduals(twv,twa,xp);

		// set x= x - eps
		xp(iVar) = x_(iVar) - eps;

		// compile the i-th column of the Jacobian matrix subtracting the
		// residuals for x_minus_epsilon
		col(iVar) -= vppItemsContainer_->getResiduals(twv,twa,xp);

		// divide the column of the Jacobian by 2*eps
		col(iVar) /= ( 2 * eps );

	}
}

// Produces a test plot for a range of values of the state variables
// in order to test for the coherence of the values that have been computed
void VPPJacobian::test(int twv, int twa) {

	// How many values this test is made of. The analyzed velocity range varies
	// from 0 to n/10
	size_t n=10;

	// Instantiate the containers used to feed the plotter
	// for the moment just one variable at the time...
	Eigen::MatrixXd x(1,n), y(1,n), du(1,n), dv(1,n);

	// plot dF/dx when varying x
	// Fill the vectors
	for(size_t i=0; i<n; i++){

		// Vary the first state variable and record its value to the plotting
		// buffer vector
		x_(0) = (i + 1 ) * 0.1;
		x(0,i)= x_(0);

		// Store the force value for this plot
		y(0,i)= vppItemsContainer_->getResiduals(twv,twa,x_)(0);

		// Compute the Jacobian for this configuration
		run(twv, twa);
		std::cout<<"J= "<<*this<<std::endl;

		// x-component of the jacobian derivative df/dx - the 'optimal' dx for
		// finite differences
		du(0,i)= x_(0) * std::sqrt( std::numeric_limits<double>::epsilon() );

		// y-component of the jacobian derivative: df/dx * dx
	  dv(0,i)= coeffRef(0,0) * du(0,i);

	  // Normalize the vector size
	  double norm=std::sqrt(du(0,i)*du(0,i)+dv(0,i)*dv(0,i));
	  du(0,i) /= norm;
	  dv(0,i) /= norm;
	}

	// Instantiate a vector plotter and produce the plot
	VectorPlotter testplot;
	testplot.plot(x,y,du,dv,"dF/dx Jacobian test plot","U [m/s]","F[N]");

}

// Destructor
VPPJacobian::~VPPJacobian(){
	// make nothing
}
