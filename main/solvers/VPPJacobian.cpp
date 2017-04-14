#include "VPPJacobian.h"
#include "mathUtils.h"

// Constructor - square pb
VPPJacobian::VPPJacobian(VectorXd& x,VPPItemFactory* pVppItemsContainer,
		size_t subProblemSize):
		x_(x),
		xp0_(x),
		pVppItemsContainer_(pVppItemsContainer),
		subPbSize_(subProblemSize),
		size_(subProblemSize) {

	// Resize this Jacobian to the size of the state vector
	resize(subPbSize_,size_);

}

// Constructor - non square pb
VPPJacobian::VPPJacobian(VectorXd& x,VPPItemFactory* pVppItemsContainer,
		size_t subProblemSize, size_t size):
		x_(x),
		xp0_(x),
		pVppItemsContainer_(pVppItemsContainer),
		subPbSize_(subProblemSize),
		size_(size) {

	// Resize this Jacobian to the size of the state vector
	resize(subPbSize_,size_);

}

void VPPJacobian::run(int twv, int twa) {

	// Note that we do not need to update x_, because x_ is a reference to the
	// state vector of the class calling the constructor of this!

	// loop on the state variables
	for(size_t iVar=0; iVar<size_; iVar++) {

		// Compute the optimum eps for this variable
		double eps=std::sqrt( std::numeric_limits<double>::epsilon() );
		if(x_(iVar)) eps *= std::fabs(x_(iVar));

		// Init a buffer of the state vector xp_
		VectorXd xp(x_);

		// set x= x + eps
		xp(iVar) = x_(iVar) + eps;

		// Compile the i-th column of the Jacobian matrix with the
		// residuals for x_plus_epsilon: ( dF/dvar(i) dM/dvar(i) )^T
		col(iVar) = pVppItemsContainer_->getResiduals(twv,twa,xp).block(0,0,subPbSize_,1);

		// set x= x - eps
		xp(iVar) = x_(iVar) - eps;

		// compile the i-th column of the Jacobian matrix subtracting the
		// residuals for x_minus_epsilon
		col(iVar) -= pVppItemsContainer_->getResiduals(twv,twa,xp).block(0,0,subPbSize_,1);

		// divide the column of the Jacobian by 2*eps
		col(iVar) /= ( 2 * eps );

	}

	// Update the items with the initial state vector
	pVppItemsContainer_->update(twv,twa,x_);

}

// Produces a plot for a range of values of the state variables
// in order to test for the coherence of the values that have been computed
std::vector<VppXYCustomPlotWidget*> VPPJacobian::plot(WindIndicesDialog& wd) {

	// Instantiate a vector with the plot widgets to return
	std::vector<VppXYCustomPlotWidget*> retVector;

	// How many values this test is made of. The analyzed velocity range varies
	// from -n/2 to n/2
	size_t n=50;

	// Init the state vector at the value of the state vector at the beginning of
	// the current iteration
	x_=xp0_;

	// Instantiate the containers used to feed the plotter
	// for the moment just one variable at the time...
	QVector<double> x(n), f(n), du_f(n), df(n), M(n), du_M(n), dM(n);

	// plot the Jacobian components when varying the boat velocity x(0)
	for(size_t i=0; i<n; i++){

		// Vary the first state variable and record its value to the plotting
		// buffer vector (half negative)
		x_(0)=	0.1 * ( (i+1) - 0.5 * n );

		x[i]= x_(0);

		// Store the force value for this plot
		Eigen::VectorXd residuals=pVppItemsContainer_->getResiduals(wd.getTWV(),wd.getTWA(),x_);
		f[i]= residuals(0);
		M[i]= residuals(1);

		// Compute the Jacobian for this configuration
		run(wd.getTWV(),wd.getTWA());

		// x-component of the jacobian derivative d./dx - the 'optimal' dx for
		// finite differences
		double eps=std::sqrt( std::numeric_limits<double>::epsilon() );
		if(x_(0)) eps *= std::fabs(x_(0));

		du_f[i]= eps;
		du_M[i]= eps;

		// y-component of the jacobian derivative: dF/dx * dx
		df[i]= coeffRef(0,0) * du_f[i];

		// y-component of the jacobian derivative: dM/dx * dx
		dM[i]= coeffRef(1,0) * du_M[i];

	}

	VppXYCustomPlotWidget* pFPlot= new VppXYCustomPlotWidget("dF/du Jacobian","Vboat [m/s]","F [N]");

	pFPlot->addData(x,f,"f(u)");
	pFPlot->addQuivers(x,f,du_f,df);

	pFPlot->rescaleAxes();
	retVector.push_back(pFPlot);

	// --

	VppXYCustomPlotWidget* pMPlot= new VppXYCustomPlotWidget("dM/du Jacobian","Vboat [m/s]","M [N]");

	pMPlot->addData(x,M,"M(u)");
	pMPlot->addQuivers(x,M,du_M,dM);

	pMPlot->rescaleAxes();
	retVector.push_back(pMPlot);

	// --

	// Reset the state vector to its initial state
	x_=xp0_;

	// plot the Jacobian components when varying Phi, say n is in deg
	for(size_t i=0; i<n; i++){

		// Vary the first state variable and record its value to the plotting
		// buffer vector
		x_(1) = mathUtils::toRad( (i + 1 ) * 0.1 ) - 0.5 * mathUtils::toRad( n * 0.1 );
		x[i]= x_(1);

		// Store the force value for this plot
		Eigen::VectorXd residuals=pVppItemsContainer_->getResiduals(wd.getTWV(),wd.getTWA(),x_);
		f[i]= residuals(0);
		M[i]= residuals(1);

		// Compute the Jacobian for this configuration
		run(wd.getTWV(),wd.getTWA());

		// x-component of the jacobian derivative d./dx - the 'optimal' dx for
		// finite differences
		double eps=std::sqrt( std::numeric_limits<double>::epsilon() );
		if(x_(1)) eps *= std::fabs(x_(1));

		du_f[i]= eps;
		du_M[i]= eps;

		// y-component of the jacobian derivative: dF/dx * dx
		df[i]= coeffRef(0,1) * du_f[i];

		// y-component of the jacobian derivative: dM/dx * dx
		dM[i]= coeffRef(1,1) * du_M[i];

	}

	VppXYCustomPlotWidget* pFPhiPlot= new VppXYCustomPlotWidget("dF/dPhi Jacobian","Phi boat [rad]","F [N]");

	pFPhiPlot->addData(x,f,"f(Phi)");
	pFPhiPlot->addQuivers(x,f,du_f,df);

	pFPhiPlot->rescaleAxes();
	retVector.push_back(pFPhiPlot);

	// --

	VppXYCustomPlotWidget* pMPhiPlot= new VppXYCustomPlotWidget("dM/dPhi Jacobian","Phi boat [rad]","M [N]");

	pMPhiPlot->addData(x,M,"M(Phi)");
	pMPhiPlot->addQuivers(x,M,du_M,dM);

	pMPhiPlot->rescaleAxes();
	retVector.push_back(pMPhiPlot);

	return retVector;
}

// Destructor
VPPJacobian::~VPPJacobian(){
	// make nothing
}

//// JacobianChecker  ///////////////////////////////////////////////////
//
// Class used to plot the Jacobian derivatives for a step that
// has failed to converge. Note that the class Jacobian contains a
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
	VPPVectorPlotter dFdx;
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

	VPPVectorPlotter dMdx;
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

	VPPVectorPlotter dFdPhi;
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

	VPPVectorPlotter dMdPhi;
	dMdPhi.plot(x,m,du_m,dm,1,"dM/dPhi JacobianChecker test plot","Phi [rad]","M [N*m]");

}









