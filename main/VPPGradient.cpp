#include "VPPGradient.h"
#include "mathUtils.h"
#include "math.h"

// Constructor - square pb
VPPGradient::VPPGradient(VectorXd& x,VPPItemFactory* pVppItemsContainer):
x_(x),
xp0_(x),
pVppItemsContainer_(pVppItemsContainer),
size_(4) {

	// Resize this Jacobian to the size of the state vector
	resize(size_);

	// Instantiate a nrSolver. Set the subPbSize at 2 to start with, as
	// this is the most generic case
	pSolver_.reset(new NRSolver(pVppItemsContainer,size_,2) );


}

// Set the operation point and run to compute the derivatives
void VPPGradient::run(const VectorXd& x, int twv, int twa) {

	// Set the operation point x
	x_=x;

	// Run to compute the derivatives
	run(twv,twa);
}

// Compute this Gradient
void VPPGradient::run(int twv, int twa) {

	// Set cout precision
	// std::cout.precision(5);

	// loop on the state variables
	for(size_t iVar=0; iVar<size_; iVar++) {

		// Compute du/du = 1
		if(iVar==0){
			coeffRef(0) = 1;
			continue;
		}

		// Compute the other components : du/dPhi  du/db  du/df

		// Init the state vector buffers xp and xm
		VectorXd xp(x_), xm(x_);

		// Reset the subPBsize based on the variable we are computing for.
		// The subPbSize is 1 when computing du/dPhi, as this is not an
		// optimization variable and we want effectively to find u st: dF=0
		// while we enforce Phi (and thus dM!=0). Not so for the other derivatives
		// where we fix optimization variables and want to assure the equilibrium
		// dF=0, dM=0.
		if(iVar==1)
			pSolver_->setSubPbSize(1);
		else
			pSolver_->setSubPbSize(2);

		// Compute the optimum eps for this variable
		double eps=std::sqrt( std::numeric_limits<double>::epsilon() );
		if(x_(iVar)) eps *= std::fabs(x_(iVar));

		// set x= x + eps
		xp(iVar) = x_(iVar) + eps;

		// Compute du/(dx(iVar)+eps) s.t: dF=0, dM=0 (iVar=2,3)
		xp= pSolver_->run(twv,twa,xp);

		// set x= x - eps
		xm(iVar) = x_(iVar) - eps;

		// Compute du/(dx(iVar)-eps) s.t: dF=0, dM=0 (iVar=2,3)
		xm= pSolver_->run(twv,twa,xm);

		// Compute the component of the Gradient
		// du / dVar = ( u_p - u_m ) / ( 2 * eps )
		coeffRef(iVar) =  ( xp(0) - xm(0) ) / ( 2 * eps);
	}

	// Update the items with the initial state vector
	pVppItemsContainer_->update(twv,twa,x_);

}

// Produces a test plot for a range of values of the state variables
// in order to test for the coherence of the values that have been computed
void VPPGradient::testPlot(int twv, int twa) {

	// Init the state vector at the value of the state vector at the beginning of
	// the current iteration
	x_= xp0_;

	// How many values this test is made of
	size_t n=100;

	// Instantiate the containers used to feed the plotter
	// for the moment just one variable at the time...
	Eigen::MatrixXd phi(1,n), u(1,n), du_dPhi(1,n), dPhi(1,n);

	// Plot the Jacobian components when varying the boat velocity x(0)
	for(int iStep=0; iStep<n; iStep++) {

		// Vary Phi from 0 to p/4 and record its value to the plotting
		LinSpace space(0,M_PI/4,n);
		x_(1)= space.get( iStep );

		// Set the subPbSize to 1 for this Pb as Phi remains fixed (iVar is
		// consistently =1), so we are only interested into du as phi evolves.
		// Note that this setting is overridden by when calling run(twv, twa)
		// This is why we NEED to reset the subPbSize at each iStep for the initial
		// guess to respect the given phi
		pSolver_->setSubPbSize(1);

		// Compute the equilibrium velocity for this Phi
		x_= pSolver_->run(twv,twa,x_);

		// Store the values into the buffer vectors
		// These buffers will be used to plot(phi,u)
		phi(0,iStep) = x_(1);
		u(0,iStep) = x_(0);

		// Now run the vppGradient
		run(twv, twa);

		// x-component of the jacobian derivative d./dx - the 'optimal' dx for
		// finite differences. But here we use d./dPhi
		double eps=std::sqrt( std::numeric_limits<double>::epsilon() );
		if(x_(1)) eps *= std::fabs(x_(1));

		// x-component of the gradient vector du/dPhi
		dPhi(0,iStep) = eps;
		// y-component of the gradient vector du/dPhi
		du_dPhi(0,iStep) = coeffRef(1) * eps;

	}

	// Instantiate a vector plotter and produce the plot
	VPPVectorPlotter dudPhi;
	dudPhi.plot(phi,u,dPhi,du_dPhi,2,"du/dPhi Gradient test plot","Phi [rad]","u [m/s]");

	// Reset the state vector to its initial state
	x_=xp0_;

	// --

	// Repeat the procedure for the third state variable b, the position of the crew
	// Instantiate the containers used to feed the plotter
	// for the moment just one variable at the time...
	Eigen::MatrixXd b(1,n), /*u(1,n),*/ du_db(1,n), db(1,n);
	u.setZero();

	// Plot the Jacobian components when varying the boat velocity x(0)
	for(int iStep=0; iStep<n; iStep++) {

		// Vary the crew position b 0 to 3 and record its value to the plotting
		LinSpace space(0,3,n);
		x_(2)= space.get(iStep);

		pSolver_->setSubPbSize(2);

		// Compute the equilibrium velocity for this Phi
		x_= pSolver_->run(twv,twa,x_);

		// Store the values into the buffer vectors
		// These buffers will be used to plot(phi,u)
		b(0,iStep) = x_(2);
		u(0,iStep) = x_(0);

		// Now run the vppGradient
		run(twv, twa);

		// x-component of the jacobian derivative d./dx - the 'optimal' dx for
		// finite differences. But here we use d./dPhi
		double eps=std::sqrt( std::numeric_limits<double>::epsilon() );
		if(x_(2)) eps *= std::fabs(x_(2));

		// x-component of the gradient vector du/dPhi
		db(0,iStep) = eps;
		// y-component of the gradient vector du/dPhi
		du_db(0,iStep) = coeffRef(2) * eps;

	}


	VPPVectorPlotter dudb;
	dudb.plot(b,u,db,du_db,3,"du/db Gradient test plot","b [m]","u [m/s]");

	// Reset the state vector to its initial state
	x_=xp0_;

	// --

	// Repeat the procedure for the fourth state variable f, the sail flat
	// Instantiate the containers used to feed the plotter
	// for the moment just one variable at the time...
	Eigen::MatrixXd f(1,n), /*u(1,n),*/ du_df(1,n), df(1,n);
	u.setZero();

	// Plot the Jacobian components when varying the boat velocity x(0)
	for(int iStep=0; iStep<n; iStep++) {

		// Vary flat from 0 to 1 and record its value to the plotting
		// todo: enhance this condition which is not robust enough!
		LinSpace space(.3,1,n);
		x_(3)= space.get(iStep);

		pSolver_->setSubPbSize(2);

		// Compute the equilibrium velocity for this Phi
		x_= pSolver_->run(twv,twa,x_);

		// Store the values into the buffer vectors
		// These buffers will be used to plot(phi,u)
		f(0,iStep) = x_(3);
		u(0,iStep) = x_(0);

		// Now run the vppGradient
		run(twv, twa);

		// x-component of the jacobian derivative d./dx - the 'optimal' dx for
		// finite differences. But here we use d./dPhi
		double eps=std::sqrt( std::numeric_limits<double>::epsilon() );
		if(x_(3)) eps *= std::fabs(x_(2));

		// x-component of the gradient vector du/dPhi
		df(0,iStep) = eps;
		// y-component of the gradient vector du/dPhi
		du_df(0,iStep) = coeffRef(3) * eps;

	}

	VPPVectorPlotter dudf;
	dudf.plot(f,u,df,du_df,5,"du/df Gradient test plot","f [m]","u [m/s]");

	// Reset the state vector to its initial state
	x_=xp0_;

}

// Destructor
VPPGradient::~VPPGradient(){
	// make nothing
}



