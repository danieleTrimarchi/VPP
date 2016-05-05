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

	// Note that we do not need to update x_, because x_ is a reference to the
	// state vector of the class calling the constructor of this!

	// loop on the state variables
	for(size_t iVar=0; iVar<x_.rows(); iVar++) {

		// Compute the optimum eps for this variable
		double eps=std::sqrt( std::numeric_limits<double>::epsilon() );
		if(x_(iVar)) eps *= std::fabs(x_(iVar));

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
	// from -n/2 to n/2
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
		// buffer vector (half negative)
		x_(0)=	0.1 * ( (i+1) - 0.5 * n );

		x(0,i)= x_(0);

		// Store the force value for this plot
		Eigen::VectorXd residuals=vppItemsContainer_->getResiduals(twv,twa,x_);
		f(0,i)= residuals(0);
		M(0,i)= residuals(1);

		// Compute the Jacobian for this configuration
		run(twv, twa);

		// x-component of the jacobian derivative d./dx - the 'optimal' dx for
		// finite differences
		du_f(0,i)= fabs(x_(0)) * std::sqrt( std::numeric_limits<double>::epsilon() );
		du_M(0,i)= du_f(0,i);

		// y-component of the jacobian derivative: dF/dx * dx
	  df(0,i)= coeffRef(0,0) * du_f(0,i);

		// y-component of the jacobian derivative: dM/dx * dx
	  dM(0,i)= coeffRef(1,0) * du_M(0,i);

	}

	// Instantiate a vector plotter and produce the plot
	VectorPlotter dFdx;
	dFdx.plot(x,f,du_f,df,40,"dF/du Jacobian test plot","Vboat [m/s]","F[N]");

	// Instantiate a vector plotter and produce the plot
	VectorPlotter dMdx;
	dMdx.plot(x,M,du_M,dM,150,"dM/du Jacobian test plot","Vboat [m/s]","M[N*m]");

	// Reset the state vector to its initial state
	x_=xp0_;

	// plot the Jacobian components when varying Phi, say n is in deg
	for(size_t i=0; i<n; i++){

		// Vary the first state variable and record its value to the plotting
		// buffer vector
		x_(1) = mathUtils::toRad( (i + 1 ) * 0.1 ) - 0.5 * mathUtils::toRad( n * 0.1 );
		x(0,i)= x_(1);

		// Store the force value for this plot
		Eigen::VectorXd residuals=vppItemsContainer_->getResiduals(twv,twa,x_);
		f(0,i)= residuals(0);
		M(0,i)= residuals(1);

		// Compute the Jacobian for this configuration
		run(twv, twa);

		// x-component of the jacobian derivative d./dx - the 'optimal' dx for
		// finite differences
		du_f(0,i)= fabs(x_(1)) * std::sqrt( std::numeric_limits<double>::epsilon() );
		du_M(0,i)= du_f(0,i);

		// y-component of the jacobian derivative: dF/dx * dx
	  df(0,i)= coeffRef(0,1) * du_f(0,i);

		// y-component of the jacobian derivative: dM/dx * dx
	  dM(0,i)= coeffRef(1,1) * du_M(0,i);

	}

	// Instantiate a vector plotter and produce the plot
	VectorPlotter dFdPhi;
	dFdPhi.plot(x,f,du_f,df,.5,"dF/dPhi Jacobian test plot","Phi [RAD]","F[N]");

	// Instantiate a vector plotter and produce the plot
	VectorPlotter dMdPhi;
	dMdPhi.plot(x,M,du_M,dM,100,"dM/dPhi Jacobian test plot","Phi [RAD]","M[N*m]");

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
	Eigen::MatrixXd x(1,n), f(1,n), du_f(1,n), df(1,n),
													m(1,n), du_m(1,n), dm(1,n);

	// Feed the containers for plotting df/dx
	for(size_t i=0; i<n; i++){

		// Feed then first state variable: Vb
		x(0,i)= xs_[i](0);

		// Feed the force residual vector f,m
		f(0,i)= res_[i](0);

		// Feed the force derivative vectors: df = df/dx * dx
		du_f(0,i)= x(0,i) * std::sqrt( std::numeric_limits<double>::epsilon() );
	  df(0,i)= jacobians_[i].coeffRef(0,0) * du_f(0,i);

	}

	// Instantiate the vector plotters and produce the plots
	VectorPlotter dFdx;
	dFdx.plot(x,f,du_f,df,1,"dF/du JacobianChecker test plot","Vb [m/s]","F [N]");

	// ----------

	// Feed the containers for plotting dM/dx
	for(size_t i=0; i<n; i++){

		// Feed the first state variable: Vb
		x(0,i)= xs_[i](0);

		// Feed the moment residual vector dM
		m(0,i)= res_[i](1);

		// Feed the moment derivative vectors: dm = dm/dx * dx
		du_m(0,i)= x(0,i) * std::sqrt( std::numeric_limits<double>::epsilon() );
	  dm(0,i)= jacobians_[i].coeffRef(1,0) * du_m(0,i);

	}

	VectorPlotter dMdx;
	dMdx.plot(x,m,du_m,dm,1,"dM/du JacobianChecker test plot","Vb [m/s]","M [N*m]");

	// ----------

	// Feed the containers for plotting dF/dPhi
	for(size_t i=0; i<n; i++){

		// Feed the second state variable: Phi
		x(0,i)= xs_[i](1);

		// Feed the force residual vector dF
		f(0,i)= res_[i](0);

		// Feed the force derivative vectors: df = df/dPhi * dPhi
		du_f(0,i)= x(0,i) * std::sqrt( std::numeric_limits<double>::epsilon() );
	  df(0,i)= jacobians_[i].coeffRef(0,1) * du_f(0,i);

	}

	VectorPlotter dFdPhi;
	dFdPhi.plot(x,m,du_m,dm,1,"dF/dPhi JacobianChecker test plot","Phi [rad]","F [N]");

	// ----------

	// Feed the containers for plotting dM/dPhi
	for(size_t i=0; i<n; i++){

		// Feed the second state variable: Phi
		x(0,i)= xs_[i](1);

		// Feed the moment residual vector dM
		m(0,i)= res_[i](1);

		// Feed the moment derivative vectors: dm = dm/dPhi * dPhi
		du_m(0,i)= x(0,i) * std::sqrt( std::numeric_limits<double>::epsilon() );
		dm(0,i)= jacobians_[i].coeffRef(1,1) * du_m(0,i);

	}

	VectorPlotter dMdPhi;
	dMdPhi.plot(x,m,du_m,dm,1,"dM/dPhi JacobianChecker test plot","Phi [rad]","M [N*m]");

}









