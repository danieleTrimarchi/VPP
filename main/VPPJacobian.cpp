#include "VPPJacobian.h"
#include "mathUtils.h"

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
	size_t n=50;

	// Init the state vector at Zero
	for(size_t i=0; i<x_.size(); i++)
		x_(i)=0;

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
	dFdx.plot(x,f,du_f,df,"dF/du Jacobian test plot","TWV [m/s]","F[N]");

	// Instantiate a vector plotter and produce the plot
	VectorPlotter dMdx;
	dMdx.plot(x,M,du_M,dM,"dM/du Jacobian test plot","TWV [m/s]","M[N*m]");


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
