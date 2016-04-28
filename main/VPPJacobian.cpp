#include "VPPJacobian.h"
#include "mathUtils.h"

// Constructor
VPPJacobian::VPPJacobian(VectorXd& x,boost::shared_ptr<VPPItemFactory> vppItemsContainer):
x_(x),
xp0_(x),
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
void VPPJacobian::testPlot(int twv, int twa) {

	// How many values this test is made of. The analyzed velocity range varies
	// from 0 to n/10
	size_t n=50;

	// Init the state vector at the value of the state vector at the beginning of
	// the current iteration
	x_=xp0_;

	std::cout<<"Reset the state vector to: "<<x_.transpose()<<std::endl;

	// Instantiate the containers used to feed the plotter
	// for the moment just one variable at the time...
	Eigen::MatrixXd x(1,n),
			f(1,n), du_f(1,n), df(1,n),
			M(1,n), du_M(1,n), dM(1,n);

	// plot the Jacobian components when varying x
	for(size_t i=0; i<n; i++){

		// Vary the first state variable and record its value to the plotting
		// buffer vector
		x_(0) = (i + 1 ) * 0.1;
		x(0,i)= x_(0);

		// Store the force value for this plot
		f(0,i)= vppItemsContainer_->getResiduals(twv,twa,x_)(0);
		M(0,i)= vppItemsContainer_->getResiduals(twv,twa,x_)(1);

		// Compute the Jacobian for this configuration
		run(twv, twa);

		// x-component of the jacobian derivative d./dx - the 'optimal' dx for
		// finite differences
		du_f(0,i)= x_(0) * std::sqrt( std::numeric_limits<double>::epsilon() );
		du_M(0,i)= du_f(0,i);

		// y-component of the jacobian derivative: dF/dx * dx
	  df(0,i)= coeffRef(0,0) * du_f(0,i);

		// y-component of the jacobian derivative: dM/dx * dx
	  dM(0,i)= coeffRef(1,0) * du_M(0,i);

	  // Normalize the vector size. Introduce some scaling used to improve the
	  // visualization as a function of the number of vectors
	  double fnorm=std::sqrt(du_f(0,i)*du_f(0,i)+df(0,i)*df(0,i)) * 5/n;
	  du_f(0,i) /= fnorm;
	  df(0,i) /= fnorm;

	  double Mnorm=std::sqrt(du_M(0,i)*du_M(0,i)+dM(0,i)*dM(0,i)) * 2/n;
	  du_M(0,i) /= Mnorm;
	  dM(0,i) /= Mnorm;

	}

	// Instantiate a vector plotter and produce the plot
	VectorPlotter dFdx;
	dFdx.plot(x,f,du_f,df,"dF/du Jacobian test plot","Vboat [m/s]","F[N]");

	// Instantiate a vector plotter and produce the plot
	VectorPlotter dMdx;
	dMdx.plot(x,M,du_M,dM,"dM/du Jacobian test plot","Vboat [m/s]","M[N*m]");


	// Reset the state vector
	for(size_t i=0; i<x_.size(); i++)
		x_(i)=0;

	// plot the Jacobian components when varying Phi, say n is in deg
	for(size_t i=0; i<n; i++){

		// Vary the first state variable and record its value to the plotting
		// buffer vector
		x_(1) = mathUtils::toRad( (i + 1 ) * 0.1 );
		x(0,i)= x_(1);

		// Store the force value for this plot
		f(0,i)= vppItemsContainer_->getResiduals(twv,twa,x_)(0);
		M(0,i)= vppItemsContainer_->getResiduals(twv,twa,x_)(1);

		// Compute the Jacobian for this configuration
		run(twv, twa);

		// x-component of the jacobian derivative d./dx - the 'optimal' dx for
		// finite differences
		du_f(0,i)= x_(1) * std::sqrt( std::numeric_limits<double>::epsilon() );
		du_M(0,i)= du_f(0,i);

		// y-component of the jacobian derivative: dF/dx * dx
	  df(0,i)= coeffRef(0,1) * du_f(0,i);

		// y-component of the jacobian derivative: dM/dx * dx
	  dM(0,i)= coeffRef(1,1) * du_M(0,i);

	  // Normalize the vector size. Introduce some scaling used to improve the
	  // visualization as a function of the number of vectors
	  double fnorm=std::sqrt(du_f(0,i)*du_f(0,i)+df(0,i)*df(0,i)) * n;
	  du_f(0,i) /= fnorm;
	  df(0,i) /= fnorm;

	  double Mnorm=std::sqrt(du_M(0,i)*du_M(0,i)+dM(0,i)*dM(0,i)) * 2/n;
	  du_M(0,i) /= Mnorm;
	  dM(0,i) /= Mnorm;

	}

	// Instantiate a vector plotter and produce the plot
	VectorPlotter dFdPhi;
	dFdPhi.plot(x,f,du_f,df,"dF/dPhi Jacobian test plot","Phi [RAD]","F[N]");

	// Instantiate a vector plotter and produce the plot
	VectorPlotter dMdPhi;
	dMdPhi.plot(x,M,du_M,dM,"dM/dPhi Jacobian test plot","Phi [RAD]","M[N*m]");

}

// Destructor
VPPJacobian::~VPPJacobian(){
	// make nothing
}

//// JacobianChecker  ///////////////////////////////////////////////////
//
// Class used to plot the Jacobian derivatives for a step that
// has failed to converge. Note that the Jacobian contains a
// method 'test' used to check for the derivatives on a generic
// interval. The JacobianChecker differs from that method because
// it stores the actual iteration-wise history of Jacobian-solution

//Constructor
JacobianChecker::JacobianChecker() {
	// make nothing
}

JacobianChecker::~JacobianChecker() {
	// make nothing
}

// Buffer a Jacobian-state vector
void JacobianChecker::push_back(VPPJacobian& J, Eigen::VectorXd& x, Eigen::VectorXd& res ) {
	jacobians_.push_back(J);
	xs_.push_back(x);
	res_.push_back(res);
}

// Plot the Jacobian-state vector for the stored history
void JacobianChecker::testPlot() {

	size_t n=jacobians_.size();

	// Instantiate the containers used to feed the plotter
	Eigen::MatrixXd x(1,n), f(1,n), du_f(1,n), df(1,n);

	// Feed the containers for plotting df/dx
	for(size_t i=0; i<n; i++){

		// Feed then first state variable: Vb
		x(0,i)= xs_[i](0);

		// Feed the force residual vector f
		f(0,i)= res_[i](0);

		// Feed the derivative vectors
		du_f(0,i)= x(0,i) * std::sqrt( std::numeric_limits<double>::epsilon() );
	  df(0,i)= jacobians_[i].coeffRef(0,0) * du_f(0,i);

	  // Normalize the vector size. Introduce some scaling used to improve the
	  // visualization as a function of the number of vectors
	  double fnorm=std::sqrt(du_f(0,i)*du_f(0,i)+df(0,i)*df(0,i)) * 5/n;
	  du_f(0,i) /= fnorm;
	  df(0,i) /= fnorm;

	}

	// Instantiate a vector plotter and produce the plot
	VectorPlotter dFdx;
	dFdx.plot(x,f,du_f,df,"dF/du JacobianChecker test plot","Vb [m/s]","F[N]");


}









