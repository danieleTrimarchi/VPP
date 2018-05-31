#include "VPPAeroItem.h"

#include "IOUtils.h"
#include "VPPException.h"
#include "mathUtils.h"
#include "MultiplePlotWidget.h"
#include "VppTabDockWidget.h"
#include "VppXYCustomPlotWidget.h"
#include "VPPDialogs.h"
#include "VPPSailCoefficientIO.h"

using namespace mathUtils;

/// Constructor
WindItem::WindItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet) :
								VPPItem(pParser,pSailSet),
								twv_(0),
								twa_(0),
								awa_(0),
								awv_(Eigen::Vector2d::Zero())   {

	// Get the max/min wind velocities from the parser
	v_tw_min_= pParser_->get("VTW_MIN");
	v_tw_max_= pParser_->get("VTW_MAX");
	n_twv_= pParser_->get("NTW");

	// Fill the values of the wind true velocities
	vTwv_.resize(n_twv_);

	// reset and compute the delta for awv
	double delta=0;
	if(n_twv_>1)
		delta=( ( v_tw_max_ - v_tw_min_ ) / (n_twv_-1) );

	for(size_t i=0; i<n_twv_; i++)
		vTwv_[i]= v_tw_min_ + i * delta;

	// Get the max/min wind angles [rad] from the parser
	alpha_tw_min_= pParser_->get("TWA_MIN");
	alpha_tw_max_= pParser_->get("TWA_MAX");
	n_alpha_tw_= pParser_->get("N_TWA");

	// Fill the values of the wind true angles
	vTwa_.resize(n_alpha_tw_);

	// reset and compute the delta for awa
	delta=0;
	if(n_alpha_tw_>1)
		delta=( ( alpha_tw_max_ - alpha_tw_min_ ) /  (n_alpha_tw_-1) );

	// store the wind angles in RADIANS
	for(size_t i=0; i<n_alpha_tw_; i++)
		vTwa_[i]= alpha_tw_min_ + i * delta;

}

/// Destructor
WindItem::~WindItem() {

}

/// Update the items for the current step (wind velocity and angle)
void WindItem::update(int vTW, int aTW) {

	// TODO dtrimarchi : it this method is called n times by the optimizer, so it is
	// stupid to re-compute twv_ e twa_ at each time! But it is the only place where I
	// can update.

	// Update the true wind velocity
	// vmin to vmax in N steps : vMin + vTW * ( (vMax-vMin)/(nSteps-2) - 1 )
	twv_= getTWV(vTW);
	if(mathUtils::isNotValid(twv_)) throw VPPException(HERE,"twv_ is NAN!");

	// Update the true wind angle: make as per the velocity
	twa_= getTWA(aTW);
	if(mathUtils::isNotValid(twa_)) throw VPPException(HERE,"twa_ is NAN!");

	// Update the apparent wind velocity vector
	awv_(0)= V_ + twv_ * cos( twa_ );
	if(mathUtils::isNotValid(awv_(0))) throw VPPException(HERE,"awv_(0) is NAN!");

	awv_(1)= twv_ * sin( twa_ );
	if(mathUtils::isNotValid(awv_(1))) throw VPPException(HERE,"awv_(1) is NAN!");
	if(awv_(1)<0)
		throw VPPException(HERE,"awv_(1) is Negative!");

	// Update the apparent wind angle
	awa_= atan2( awv_(1),awv_(0) );
	if(mathUtils::isNotValid(awa_))	throw VPPException(HERE,"awa_ is NAN!");

}

// Returns the true wind velocity for a given step
const double WindItem::getTWV(size_t iV) const {
	return vTwv_[iV];
}

// Returns the current true wind velocity for this step
const double WindItem::getTWV() const {
	return twv_;
}

// Returns the number of true wind velocities
const int WindItem::getWVSize() const {
	return n_twv_;
}

// Returns the true wind angle [rad] for a given step
const double WindItem::getTWA(size_t iA) const {
	return vTwa_[iA];
}

// Returns the current true wind angle for this step
const double WindItem::getTWA() const {
	return twa_;
}

// Returns the number of true wind angles
const int WindItem::getWASize() const {
	return n_alpha_tw_;
}

/// Returns the apparent wind angle for this step
const double WindItem::getAWA() const {
	return awa_;
}

// Returns the apparent wind velocity vector for this step
const Eigen::Vector2d WindItem::getAWV() const {
	return awv_;
}

// Returns the apparent wind velocity vector norm for this step
const double WindItem::getAWNorm() const {
	return awv_.norm();
}

//=================================================================

// Constructor
SailCoefficientItem::SailCoefficientItem(WindItem* pWindItem) :
								VPPItem(pWindItem->getParser(), pWindItem->getSailSet()),
								pWindItem_(pWindItem),
								awa_(0),
								ar_(0),
								cl_(0),
								cdp_(0),
								cdI_(0),
								cd0_(0),
								cd_(0) {

	// Instantiate a VPPSailCoefficientIO to get the sail coefficients
	// (default OR user-defined
	pCl_.reset(new VPP_CL_IO(pParser_));
	pCd_.reset(new VPP_CD_IO );

	interpolateCoeffs();
}

void SailCoefficientItem::interpolateCoeffs() {

	// Reset the interpolator vectors before filling them
	interpClVec_.clear();
	interpCdVec_.clear();

	Eigen::ArrayXd x, y;
	x=pCl_->getCoefficientMatrix()->col(0);
	// Interpolate the values of the sail coefficients for the MainSail
	for(size_t i=1; i<4; i++){
		y=pCl_->getCoefficientMatrix()->col(i);
		interpClVec_.push_back( boost::shared_ptr<SplineInterpolator>( new SplineInterpolator(x,y) ) );
	}
	x=pCd_->getCoefficientMatrix()->col(0);
	// Interpolate the values of the sail coefficients for the MainSail
	for(size_t i=1; i<4; i++){
		y=pCd_->getCoefficientMatrix()->col(i);
		interpCdVec_.push_back( boost::shared_ptr<SplineInterpolator>( new SplineInterpolator(x,y)) );
	}

	// resize and init cl_ and cd_ for storing the interpolated values
	allCl_=Eigen::Vector3d::Zero();
	allCd_=Eigen::Vector3d::Zero();
}

// Destructor
SailCoefficientItem::~SailCoefficientItem() {
}

// Implement the pure virtual
void SailCoefficientItem::update(int vTW, int aTW) {

	// Update the local copy of the the apparent wind angle
	awa_= pWindItem_->getAWA();
	if(mathUtils::isNotValid(awa_)) throw VPPException(HERE,"awa_ is NaN");

	// Update the Aspect Ratio
	double h= pParser_->get("EHM") + pParser_->get("AVGFREB");

//	// TODO: restore this and introduce a proper smoothing function
//	if(awa_ < toRad(45))
//		// h = mast height above deck + Average freeboard
//		h= pParser_->get("EHM") + pParser_->get("AVGFREB");
//	else
//		// h = mast height above deck
//		h= pParser_->get("EHM");

	// Compute the aspect ratio for this awa_
	ar_ = 1.1 * h * h / pSailSet_->get("AN");

	// Compute cd0, see the Hazen's model Larsson p.148
	cd0_= 1.13 * ( 	(pParser_->get("B") * pParser_->get("AVGFREB")) +
									(pParser_->get("EHM")*pParser_->get("EMDC") ) ) /
											pSailSet_->get("AN");

	// Compute the coefficients based on the sail configuration
	compute();

}

// Called by the children as a decorator for update.
// Do all is required after cl and cdp have been computed
void SailCoefficientItem::postUpdate() {

	// Reduce cl with the flattening factor of the state vector
	cl_ *= f_;
	if(mathUtils::isNotValid(cl_)) throw VPPException(HERE,"cl_ is nan");

	// Compute the induced resistance
	cdI_ = cl_ * cl_ * ( 1. / (M_PI * ar_) + 0.005 );

	// Compute the total sail drag coefficient now
	cd_ = cdp_ + cd0_ + cdI_;
	if(mathUtils::isNotValid(cd_)) throw VPPException(HERE,"cd_ is nan");

}

// Returns a ptr to the wind Item
WindItem* SailCoefficientItem::getWindItem() const {
	return pWindItem_;
}

// Returns a ptr to the CL_IO container
VPP_CL_IO* SailCoefficientItem::getClIO() const {
	return pCl_.get();
}

// Returns a ptr to the CD_IO container
VPP_CD_IO* SailCoefficientItem::getCdIO() const {
	return pCd_.get();
}

void SailCoefficientItem::computeForMain() {

	// Interpolate the values of the sail coefficients for the MainSail
	allCl_(activeSail::mainSail) = interpClVec_[activeSail::mainSail]->interpolate(awa_);
	allCd_(activeSail::mainSail) = interpCdVec_[activeSail::mainSail]->interpolate(awa_);

}

void SailCoefficientItem::computeForJib() {

	// Interpolate the values of the sail coefficients for the Jib
	allCl_(activeSail::jib)  = interpClVec_[activeSail::jib]->interpolate(awa_);
	allCd_(activeSail::jib)  = interpCdVec_[activeSail::jib]->interpolate(awa_);

}

void SailCoefficientItem::computeForSpi() {

	// Interpolate the values of the sail coefficients for the Spi
	allCl_(activeSail::jib)  = interpClVec_[activeSail::spi]->interpolate(awa_);
	allCd_(activeSail::spi) = interpCdVec_[activeSail::spi]->interpolate(awa_);

}

// Returns the current value of the lift coefficient for
// the current sail
const double SailCoefficientItem::getCl() const {
	return cl_;
}

// Returns the current value of the lift coefficient
const double SailCoefficientItem::getCd() const {
	return cd_;
}

/// PrintOut the coefficient matrices
void SailCoefficientItem::printCoefficients() {

	std::cout<<"\n=== Sail Coefficients: ============\n "<<std::endl;
	std::cout<<"Cl= \n"<<allCl_<<std::endl;
	std::cout<<"Cd= \n"<<allCd_<<std::endl;
	std::cout<<"\n===================================\n "<<std::endl;
}

//=================================================================

// Constructor
MainOnlySailCoefficientItem::MainOnlySailCoefficientItem(WindItem* pWind) :
						SailCoefficientItem(pWind) {
	// do nothing
}

// Destructor
MainOnlySailCoefficientItem::~MainOnlySailCoefficientItem() {

}

// Implement the pure virtual method of the abstract base class
void MainOnlySailCoefficientItem::compute() {

	// Compute the sail coefficients for the main sail
	computeForMain();

}

// Update the item for the current step (wind velocity and angle),
// the values of the state vector x computed by the optimizer have
// already been treated by the parent
void MainOnlySailCoefficientItem::update(int vTW, int aTW) {

	// Decorate the parent class method that updates the value of awa_
	SailCoefficientItem::update(vTW,aTW);

	// In this case the lift/drag coeffs are just what was computed for main
	cl_ = allCl_(activeSail::mainSail) ;
	cdp_ = allCd_(activeSail::mainSail) ;

	// Call the parent method that computes the effective cd=cdp+cd0+cdI
	SailCoefficientItem::postUpdate();

}

// Plot the spline-interpolated curves based on the Larsson's
// sail coefficients. The range is set 0-180deg
// Fill a multiple plot
void MainOnlySailCoefficientItem::plotInterpolatedCoefficients( MultiplePlotWidget* pMultiPlotWidget ) const {

	// -> Instantiate a VppXYCustomPlotWidget for cl of Main. Set title and axis title
	VppXYCustomPlotWidget* pClMainPlot = new VppXYCustomPlotWidget("CL for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::mainSail]->plot(pClMainPlot,0,toRad(180),50);

	// Add it to the multiplot widget
	pMultiPlotWidget->addChart(pClMainPlot,0,0);

	//---

	// -> Instantiate a VppXYCustomPlotWidget for cd of Main. Set title and axis title
	VppXYCustomPlotWidget* pCdMainPlot = new VppXYCustomPlotWidget("CD for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail]->plot(pCdMainPlot,0,toRad(180),50);

	// Add it to the multiplot widget
	pMultiPlotWidget->addChart(pCdMainPlot,0,1);

}

// Plot the spline-interpolated curves based on the Larsson's
// sail coefficients. The range is set 0-180deg
// Fill a multiple plot
void MainOnlySailCoefficientItem::plot_D_InterpolatedCoefficients( MultiplePlotWidget* pMultiPlotWidget ) const {

	// -> Instantiate a VppXYCustomPlotWidget for cl of Main. Set title and axis title
	VppXYCustomPlotWidget* pClMainPlot = new VppXYCustomPlotWidget("d(CL) for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::mainSail]->plotD1(pClMainPlot,0,toRad(180),50);

	// Add it to the multiplot widget
	pMultiPlotWidget->addChart(pClMainPlot,0,0);

	//---

	// -> Instantiate a VppXYCustomPlotWidget for cd of Main. Set title and axis title
	VppXYCustomPlotWidget* pCdMainPlot = new VppXYCustomPlotWidget("d(CD) for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail]->plotD1(pCdMainPlot,0,toRad(180),50);

	// Add it to the multiplot widget
	pMultiPlotWidget->addChart(pCdMainPlot,0,1);

}

// Plot the spline-interpolated curves based on the Larsson's
// sail coefficients. The range is set 0-180deg
// Fill a multiple plot
void MainOnlySailCoefficientItem::plot_D2_InterpolatedCoefficients( MultiplePlotWidget* pMultiPlotWidget ) const {

	// -> Instantiate a VppXYCustomPlotWidget for cl of Main. Set title and axis title
	VppXYCustomPlotWidget* pClMainPlot = new VppXYCustomPlotWidget("d2(CL) for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::mainSail]->plotD2(pClMainPlot,0,toRad(180),75);

	// Add it to the multiplot widget
	pMultiPlotWidget->addChart(pClMainPlot,0,0);

	//---

	// -> Instantiate a VppXYCustomPlotWidget for cd of Main. Set title and axis title
	VppXYCustomPlotWidget* pCdMainPlot = new VppXYCustomPlotWidget("d2(CD) for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail]->plotD2(pCdMainPlot,0,toRad(180),75);

	// Add it to the multiplot widget
	pMultiPlotWidget->addChart(pCdMainPlot,0,1);

}


//=================================================================

// Constructor
MainAndJibCoefficientItem::MainAndJibCoefficientItem(WindItem* pWind) :
						SailCoefficientItem(pWind) {
	// do nothing
}

// Destructor
MainAndJibCoefficientItem::~MainAndJibCoefficientItem() {

}

// Implement the pure virtual method of the abstract base class
void MainAndJibCoefficientItem::compute() {

	// Compute the sail coefficients for the main sail and the Jib
	computeForMain();
	computeForJib();

}

// Update the item for the current step (wind velocity and angle),
// the values of the state vector x computed by the optimizer have
// already been treated by the parent
void MainAndJibCoefficientItem::update(int vTW, int aTW) {

	// Decorate the parent class method that updates the value of awa_
	SailCoefficientItem::update(vTW,aTW);

	// create an alias for code readability
	boost::shared_ptr<SailSet> ps= pSailSet_;

	// 	Cl = ( Cl_M * AM + Cl_J * AJ ) / AN
	cl_ = ( allCl_(activeSail::mainSail)  * ps->get("AM") + allCl_(activeSail::jib)  *  ps->get("AJ") ) /  ps->get("AN");
	cdp_ = ( allCd_(activeSail::mainSail)  * ps->get("AM") + allCd_(activeSail::jib)  *  ps->get("AJ") ) /  ps->get("AN");

	if(mathUtils::isNotValid(cl_)) throw VPPException(HERE,"cl_ is nan");
	if(mathUtils::isNotValid(cdp_)) throw VPPException(HERE,"cdp_ is nan");

	// Call the parent method that computes the effective cd=cdp+cd0+cdI
	SailCoefficientItem::postUpdate();

}

// Plot the spline-interpolated curves based on the Larsson's sail coefficients.
// The range is set 0-180deg
void MainAndJibCoefficientItem::plotInterpolatedCoefficients( MultiplePlotWidget* pMultiPlotWidget ) const {

	// -> Instantiate a chart for cl of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pClMainPlot = new VppXYCustomPlotWidget("CL for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::mainSail]->plot(pClMainPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pClMainPlot,0,0);

	// -> Instantiate a chart for cl of Jib. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pClJibPlot = new VppXYCustomPlotWidget("CL for JIB","AWA [rad]","[-]");
	interpClVec_[activeSail::jib]->plot(pClJibPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pClJibPlot,1,0);

	// -> Instantiate a chart for cd of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pCdMainPlot = new VppXYCustomPlotWidget("CD for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail]->plot(pCdMainPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pCdMainPlot,0,1);

	// -> Instantiate a chart for cd of Jib. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pCdJibPlot = new VppXYCustomPlotWidget("CD for Jib","AWA [rad]","[-]");
	interpCdVec_[activeSail::jib]->plot(pCdJibPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pCdJibPlot,1,1);

}

void MainAndJibCoefficientItem::plot_D_InterpolatedCoefficients(MultiplePlotWidget* pMultiPlotWidget) const {

	// -> Instantiate a chart for the derivative of cl of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdClMainPlot = new VppXYCustomPlotWidget("d(CL) for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::mainSail]->plotD1(pdClMainPlot,0,toRad(180),75);
	pMultiPlotWidget->addChart(pdClMainPlot,0,0);

	// -> Instantiate a chart for the derivative of cl of Jib. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdClJibPlot = new VppXYCustomPlotWidget("d(CL) for JIB","AWA [rad]","[-]");
	interpClVec_[activeSail::jib]->plotD1(pdClJibPlot,0,toRad(180),75);
	pMultiPlotWidget->addChart(pdClJibPlot,1,0);

	// -> Instantiate a chart for the derivative of cd of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdCdMainPlot = new VppXYCustomPlotWidget("d(CD) for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail]->plotD1(pdCdMainPlot,0,toRad(180),75);
	pMultiPlotWidget->addChart(pdCdMainPlot,0,1);

	// -> Instantiate a chart for the derivative of cd of Jib. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdCdJibPlot = new VppXYCustomPlotWidget("d(CD) for Jib","AWA [rad]","[-]");
	interpCdVec_[activeSail::jib]->plotD1(pdCdJibPlot,0,toRad(180),75);
	pMultiPlotWidget->addChart(pdCdJibPlot,1,1);

}

// Plot the second derivative of the spline-interpolated
// curves based on the Larsson's sail coefficients.
// The range is set 0-180deg
void MainAndJibCoefficientItem::plot_D2_InterpolatedCoefficients(MultiplePlotWidget* pMultiPlotWidget) const {

	// -> Instantiate a chart for the second derivative of cl of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pd2ClMainPlot = new VppXYCustomPlotWidget("d2(CL) for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::mainSail]->plotD2(pd2ClMainPlot,0,toRad(180),150);
	pMultiPlotWidget->addChart(pd2ClMainPlot,0,0);

	// -> Instantiate a chart for the second derivative of cl of Jib. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pd2ClJibPlot = new VppXYCustomPlotWidget("d2(CL) for JIB","AWA [rad]","[-]");
	interpClVec_[activeSail::jib]->plotD2(pd2ClJibPlot,0,toRad(180),150);
	pMultiPlotWidget->addChart(pd2ClJibPlot,1,0);

	// -> Instantiate a chart for the second derivative of cd of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pd2CdMainPlot = new VppXYCustomPlotWidget("d2(CD) for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail]->plotD2(pd2CdMainPlot,0,toRad(180),150);
	pMultiPlotWidget->addChart(pd2CdMainPlot,0,1);

	// -> Instantiate a chart for the second derivative of cl of Jib. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pd2CdJibPlot = new VppXYCustomPlotWidget("d2(CD) for Jib","AWA [rad]","[-]");
	interpCdVec_[activeSail::jib]->plotD2(pd2CdJibPlot,0,toRad(180),150);
	pMultiPlotWidget->addChart(pd2CdJibPlot,1,1);

}

//=================================================================

// Constructor
MainAndSpiCoefficientItem::MainAndSpiCoefficientItem(WindItem* pWind) :
						SailCoefficientItem(pWind) {
	// do nothing
}

// Destructor
MainAndSpiCoefficientItem::~MainAndSpiCoefficientItem() {

}

// Implement the pure virtual method of the abstract base class
void MainAndSpiCoefficientItem::compute() {

	// Compute the sail coefficients for the main sail and the Jib
	computeForMain();
	computeForSpi();

}

// Update the item for the current step (wind velocity and angle),
// the values of the state vector x computed by the optimizer have
// already been treated by the parent
void MainAndSpiCoefficientItem::update(int vTW, int aTW) {

	// Decorate the parent class method that updates the value of awa_
	SailCoefficientItem::update(vTW,aTW);

	// create an alias for code readability
	boost::shared_ptr<SailSet> ps= pSailSet_;

	// 	Cl = ( Cl_M * AM + Cl_J * AJ ) / AN
	cl_ = ( allCl_(activeSail::mainSail)  * ps->get("AM") + allCl_(activeSail::spi) *  ps->get("AS") ) /  ps->get("AN");
	cdp_ = ( allCd_(activeSail::mainSail)  * ps->get("AM") + allCd_(activeSail::spi) *  ps->get("AS") ) /  ps->get("AN");
	if(mathUtils::isNotValid(cl_)) throw VPPException(HERE,"cl_ is nan");
	if(mathUtils::isNotValid(cdp_)) throw VPPException(HERE,"cdp_ is nan");

	// Call the parent method that computes the effective cd=cdp+cd0+cdI
	SailCoefficientItem::postUpdate();

}

// Plot the spline-interpolated curves based on the Larsson's
// sail coefficients. The range is set 0-180deg
// Fill a multiple plot
void MainAndSpiCoefficientItem::plotInterpolatedCoefficients( MultiplePlotWidget* pMultiPlotWidget ) const {

	// -> Instantiate a chart for the cl of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pClMainPlot = new VppXYCustomPlotWidget("CL for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::mainSail]->plot(pClMainPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pClMainPlot,0,0);

	// -> Instantiate a chart for the cl of Spi. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pClSpiPlot = new VppXYCustomPlotWidget("CL for SPI","AWA [rad]","[-]");
	interpClVec_[activeSail::spi]->plot(pClSpiPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pClSpiPlot,1,0);

	// -> Instantiate a chart for the cd of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pCdMainPlot = new VppXYCustomPlotWidget("CD for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail]->plot(pCdMainPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pCdMainPlot,0,1);

	// -> Instantiate a chart for the cd of Spi. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pCdSpiPlot = new VppXYCustomPlotWidget("CD for SPI","AWA [rad]","[-]");
	interpCdVec_[activeSail::spi]->plot(pCdSpiPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pCdSpiPlot,1,1);

}

void MainAndSpiCoefficientItem::plot_D_InterpolatedCoefficients(MultiplePlotWidget* pMultiPlotWidget) const {

	// -> Instantiate a chart for the derivative of cl of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdClMainPlot = new VppXYCustomPlotWidget("d(CL) for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::mainSail]->plotD1(pdClMainPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pdClMainPlot,0,0);

	// -> Instantiate a chart for the derivative of cl of Spi. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdClSpiPlot = new VppXYCustomPlotWidget("d(CL) for SPI","AWA [rad]","[-]");
	interpClVec_[activeSail::spi]->plotD1(pdClSpiPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pdClSpiPlot,1,0);

	// -> Instantiate a chart for the derivative of cd of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdCdMainPlot = new VppXYCustomPlotWidget("d(CD) for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail]->plotD1(pdCdMainPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pdCdMainPlot,0,1);

	// -> Instantiate a chart for the derivative of cd of Spi. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdCdSpiPlot = new VppXYCustomPlotWidget("d(CD) for SPI","AWA [rad]","[-]");
	interpCdVec_[activeSail::spi]->plotD1(pdCdSpiPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pdCdSpiPlot,1,1);

}

void MainAndSpiCoefficientItem::plot_D2_InterpolatedCoefficients(MultiplePlotWidget* pMultiPlotWidget) const {

	// -> Instantiate a chart for the derivative of cl of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pd2ClMainPlot = new VppXYCustomPlotWidget("d2(CL) for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::mainSail]->plotD2(pd2ClMainPlot,0,toRad(180),75);
	pMultiPlotWidget->addChart(pd2ClMainPlot,0,0);

	// -> Instantiate a chart for the derivative of cl of Spi. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pd2ClSpiPlot = new VppXYCustomPlotWidget("d2(CL) for SPI","AWA [rad]","[-]");
	interpClVec_[activeSail::spi]->plotD2(pd2ClSpiPlot,0,toRad(180),75);
	pMultiPlotWidget->addChart(pd2ClSpiPlot,1,0);

	// -> Instantiate a chart for the derivative of cd of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pd2CdMainPlot = new VppXYCustomPlotWidget("d2(CD) for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail]->plotD2(pd2CdMainPlot,0,toRad(180),75);
	pMultiPlotWidget->addChart(pd2CdMainPlot,0,1);

	// -> Instantiate a chart for the derivative of cd of Spi. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pd2CdSpiPlot = new VppXYCustomPlotWidget("d2(CD) for SPI","AWA [rad]","[-]");
	interpCdVec_[activeSail::spi]->plotD2(pd2CdSpiPlot,0,toRad(180),75);
	pMultiPlotWidget->addChart(pd2CdSpiPlot,1,1);

}


//=================================================================

// Constructor
MainJibAndSpiCoefficientItem::MainJibAndSpiCoefficientItem(WindItem* pWind) :
						SailCoefficientItem(pWind) {
	// do nothing
}

// Destructor
MainJibAndSpiCoefficientItem::~MainJibAndSpiCoefficientItem() {

}

// Implement the pure virtual method of the abstract base class
void MainJibAndSpiCoefficientItem::compute() {

	// Compute the sail coefficients for the main sail and the Jib
	computeForMain();
	computeForJib();
	computeForSpi();

}

// Update the item for the current step (wind velocity and angle),
// the values of the state vector x computed by the optimizer have
// already been treated by the parent
void MainJibAndSpiCoefficientItem::update(int vTW, int aTW) {

	// Decorate the parent class method that updates the value of awa_
	SailCoefficientItem::update(vTW,aTW);

	// create an alias for code readability
	boost::shared_ptr<SailSet> ps= pSailSet_;

	// 	Cl = ( Cl_M * AM + Cl_J * AJ ) / AN
	cl_ = ( allCl_(activeSail::mainSail)  * ps->get("AM") + allCl_(activeSail::jib)  *  ps->get("AJ") + allCl_(activeSail::spi) *  ps->get("AS") ) /  ps->get("AN");
	cdp_ = ( allCd_(activeSail::mainSail)  * ps->get("AM") + allCd_(activeSail::jib)  *  ps->get("AJ") + allCd_(activeSail::spi) *  ps->get("AS") ) /  ps->get("AN");
	if(mathUtils::isNotValid(cl_)) throw VPPException(HERE,"cl_ is nan");
	if(mathUtils::isNotValid(cdp_)) throw VPPException(HERE,"cdp_ is nan");

	// Call the parent method that computes the effective cd=cdp+cd0+cdI
	SailCoefficientItem::postUpdate();

}

// Plot the spline-interpolated curves based on the Larsson's
// sail coefficients. The range is set 0-180deg
// Fill a multiple plot
void MainJibAndSpiCoefficientItem::plotInterpolatedCoefficients( MultiplePlotWidget* pMultiPlotWidget ) const {

	// -> Instantiate a chart for the cl of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pClMainPlot = new VppXYCustomPlotWidget("CL for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::mainSail]->plot(pClMainPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pClMainPlot,0,0);

	// -> Instantiate a chart for the cl of Jib. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pClJibPlot = new VppXYCustomPlotWidget("CL for JIB","AWA [rad]","[-]");
	interpClVec_[activeSail::jib]->plot(pClJibPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pClJibPlot,1,0);

	// -> Instantiate a chart for the cl of Spi. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pClSpiPlot = new VppXYCustomPlotWidget("CL for SPI","AWA [rad]","[-]");
	interpClVec_[activeSail::spi]->plot(pClSpiPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pClSpiPlot,2,0);

	// -> Instantiate a chart for the cd of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pCdMainPlot = new VppXYCustomPlotWidget("CD for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail]->plot(pCdMainPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pCdMainPlot,0,1);

	// -> Instantiate a chart for the cd of Jib. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pCdJibPlot = new VppXYCustomPlotWidget("CD for Jib","AWA [rad]","[-]");
	interpCdVec_[activeSail::jib]->plot(pCdJibPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pCdJibPlot,1,1);

	// -> Instantiate a chart for the cd of Spi. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pCdSpiPlot = new VppXYCustomPlotWidget("CD for Spi","AWA [rad]","[-]");
	interpCdVec_[activeSail::spi]->plot(pCdSpiPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pCdSpiPlot,2,1);

}

void MainJibAndSpiCoefficientItem::plot_D_InterpolatedCoefficients(MultiplePlotWidget* pMultiPlotWidget) const {

	// -> Instantiate a chart for the first derivative of cl of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdClMainPlot = new VppXYCustomPlotWidget("d(CL) for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::mainSail]->plotD1(pdClMainPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pdClMainPlot,0,0);

	// -> Instantiate a chart for the first derivative of cl of Jib. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdClJibPlot = new VppXYCustomPlotWidget("d(CL) for JIB","AWA [rad]","[-]");
	interpClVec_[activeSail::jib]->plotD1(pdClJibPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pdClJibPlot,1,0);

	// -> Instantiate a chart for the first derivative of cl of Spi. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdClSpiPlot = new VppXYCustomPlotWidget("d(CL) for SPI","AWA [rad]","[-]");
	interpClVec_[activeSail::spi]->plotD1(pdClSpiPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pdClSpiPlot,2,0);

	// -> Instantiate a chart for the first derivative of cd of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdCdMainPlot = new VppXYCustomPlotWidget("d(CD) for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail]->plotD1(pdCdMainPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pdCdMainPlot,0,1);

	// -> Instantiate a chart for the first derivative of cd of Jib. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdCdJibPlot = new VppXYCustomPlotWidget("d(CD) for Jib","AWA [rad]","[-]");
	interpCdVec_[activeSail::jib]->plotD1(pdCdJibPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pdCdJibPlot,1,1);

	// -> Instantiate a chart for the first derivative of cd of Spi. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdCdSpiPlot = new VppXYCustomPlotWidget("d(CD) for Spi","AWA [rad]","[-]");
	interpCdVec_[activeSail::spi]->plotD1(pdCdSpiPlot,0,toRad(180),50);
	pMultiPlotWidget->addChart(pdCdSpiPlot,2,1);

}

void MainJibAndSpiCoefficientItem::plot_D2_InterpolatedCoefficients(MultiplePlotWidget* pMultiPlotWidget) const {

	// -> Instantiate a chart for the first derivative of cl of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdClMainPlot = new VppXYCustomPlotWidget("d2(CL) for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::mainSail]->plotD2(pdClMainPlot,0,toRad(180),75);
	pMultiPlotWidget->addChart(pdClMainPlot,0,0);

	// -> Instantiate a chart for the first derivative of cl of Jib. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdClJibPlot = new VppXYCustomPlotWidget("d2(CL) for JIB","AWA [rad]","[-]");
	interpClVec_[activeSail::jib]->plotD2(pdClJibPlot,0,toRad(180),75);
	pMultiPlotWidget->addChart(pdClJibPlot,1,0);

	// -> Instantiate a chart for the first derivative of cl of Spi. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdClSpiPlot = new VppXYCustomPlotWidget("d2(CL) for SPI","AWA [rad]","[-]");
	interpClVec_[activeSail::spi]->plotD2(pdClSpiPlot,0,toRad(180),75);
	pMultiPlotWidget->addChart(pdClSpiPlot,2,0);

	// -> Instantiate a chart for the first derivative of cd of Main. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdCdMainPlot = new VppXYCustomPlotWidget("d2(CD) for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail]->plotD2(pdCdMainPlot,0,toRad(180),75);
	pMultiPlotWidget->addChart(pdCdMainPlot,0,1);

	// -> Instantiate a chart for the first derivative of cd of Jib. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdCdJibPlot = new VppXYCustomPlotWidget("d2(CD) for Jib","AWA [rad]","[-]");
	interpCdVec_[activeSail::jib]->plotD2(pdCdJibPlot,0,toRad(180),75);
	pMultiPlotWidget->addChart(pdCdJibPlot,1,1);

	// -> Instantiate a chart for the first derivative of cd of Spi. Plot and add it to the multiplot
	VppXYCustomPlotWidget* pdCdSpiPlot = new VppXYCustomPlotWidget("d2(CD) for Spi","AWA [rad]","[-]");
	interpCdVec_[activeSail::spi]->plotD2(pdCdSpiPlot,0,toRad(180),75);
	pMultiPlotWidget->addChart(pdCdSpiPlot,2,1);

}

//=================================================================

// Constructor
AeroForcesItem::AeroForcesItem(SailCoefficientItem* sailCoeffItem) :
						VPPItem(sailCoeffItem->getParser(), sailCoeffItem->getSailSet() ),
						pSailCoeffs_(sailCoeffItem),
						pWindItem_(pSailCoeffs_->getWindItem()),
						lift_(0),
						drag_(0),
						fDrive_(0),
						fSide_(0),
						fHeel_(0),
						mHeel_(0) {
	// do nothing
}

// Destructor
AeroForcesItem::~AeroForcesItem() {

}

/// Update the items for the current ste§p (wind velocity and angle)
void AeroForcesItem::update(int vTW, int aTW) {

	// Gets the value of the apparent wind velocity
	double awv = pWindItem_->getAWNorm();
	if(mathUtils::isNotValid(awv)) throw VPPException(HERE,"awv is NAN!");

	double awa = pWindItem_->getAWA();
	if(mathUtils::isNotValid(awa)) throw VPPException(HERE,"awa is NAN!");

	// Updates Lift = 0.5 * phys.rho_a * V_eff.^2 .* AN .* Cl;
	// Note that the nominal area AN was scaled with cos( PHI ) and it
	// takes the meaning of a projected surface
	lift_ = 0.5 * Physic::rho_a * awv * awv * pSailSet_->get("AN") * cos( PHI_ ) * pSailCoeffs_->getCl();
	if(mathUtils::isNotValid(lift_)) throw VPPException(HERE,"lift_ is NAN!");

	// Updates Drag = 0.5 * phys.rho_a * V_eff.^2 .* AN .* Cd;
	// Note that the nominal area AN was scaled with cos( PHI ) and it
	// takes the meaning of a projected surface
	drag_ = 0.5 * Physic::rho_a * awv * awv * pSailSet_->get("AN") * cos( PHI_ ) * pSailCoeffs_->getCd();
	if(mathUtils::isNotValid(drag_)) throw VPPException(HERE,"drag_ is NAN!");

	// Updates Fdrive = lift_ * sin(awa) - D * cos(awa);
  fDrive_ = lift_ * sin( awa ) - drag_ * cos( awa );
	if(mathUtils::isNotValid(fDrive_)) throw VPPException(HERE,"fDrive_ is NAN!");

	// Updates FSide = L * cos(awa) + D * sin(awa);
	fSide_ = lift_ * cos( awa ) + drag_ * sin( awa );
	if(mathUtils::isNotValid(fSide_)) throw VPPException(HERE,"fSide_ is NAN!");

	// The righting moment arm is set as the distance between the center of sail effort and
	// the hydrodynamic center, scaled with cos(PHI)
	mHeel_ = fSide_ * ( 0.45 * pParser_->get("T") + pParser_->get("AVGFREB") + pSailSet_->get("ZCE") ) * cos( PHI_ );
	if(mathUtils::isNotValid(mHeel_)) throw VPPException(HERE,"mHeel_ is NAN!");

}

// plot the aeroForces for a fixed range
void AeroForcesItem::plot(MultiplePlotWidget* pMultiPlotWidget ) {

	// Number of points of the plots
	size_t nVelocities=60;

	// For which TWV, TWA shall we plot the aero forces/moments?
	WindIndicesDialog dg(pWindItem_);
	if (dg.exec() == QDialog::Rejected)
		return;

	// Buffer the current solution
	Eigen::VectorXd xbuf(4);
	xbuf << V_,PHI_,b_,f_;

	// Now fix the value of b_ and f_
	b_= 0.01;
	f_= 0.99;

	// Declare containers for the velocity and angle-wise data
	std::vector<QVector<double> > fN, Lift, Drag, fDrive, fSide, mHeel;
	std::vector<QString> curveLabels;

	double fNmin, fNmax;

	// Loop on heel angles : from 0 to 90 deg in steps of 15 deg
	for(int hAngle=-20; hAngle<90; hAngle+=15){

		// Convert the heeling angle into radians
		PHI_= toRad(hAngle);
		// vectors with the current boat velocity, the drive force
		// and heeling moment values
		QVector<double> x_fN, lift, drag, f_v, fs_v, m_v;
		x_fN.resize(nVelocities);
		lift.resize(nVelocities);
		drag.resize(nVelocities);
		f_v.resize(nVelocities);
		fs_v.resize(nVelocities);
		m_v.resize(nVelocities);

		// loop on the boat velocity, from 0.1 to 5m/s in step on 1 m/s
		for(size_t iTwv=0; iTwv<nVelocities; iTwv++ ) {

			// Set the value for the state variable boat velocity
			// Linearly from -1 to 1 m/s
			V_ = -1 + double(iTwv) / (nVelocities-1) * 2;

			// Declare a state vector to give the windItem
			VectorXd stateVector(4);
			stateVector << V_,PHI_,b_,f_;

			// Update the wind. For the moment fix the apparent wind velocity and angle
			// to the first values contained in the variableFiles.
			pWindItem_->updateSolution(dg.getTWV(), dg.getTWA(), stateVector);

			// Update the sail coefficients for the current wind
			pSailCoeffs_->updateSolution(dg.getTWV(), dg.getTWA(), stateVector);

			// Update 'this': compute sail forces
			update(dg.getTWV(), dg.getTWA());

			// Store velocity-wise data:
			x_fN[iTwv]= V_ / sqrt( Physic::g * pParser_->get("LWL") );	// Fn...

			if(iTwv==0)
				fNmin=x_fN[iTwv];
			if(iTwv==nVelocities-1)
				fNmax=x_fN[iTwv];

			lift[iTwv] = getLift(); // lift...
			drag[iTwv] = getDrag(); // drag...
			f_v[iTwv]= getFDrive(); // fDrive...
			fs_v[iTwv]= getFSide(); // fSide_...
			m_v[iTwv]= getMHeel();  // mHeel...

		}

		// Append the velocity-wise curve for each heel angle
		fN.push_back(x_fN);
		Lift.push_back(lift);
		Drag.push_back(drag);
		fDrive.push_back(f_v);
		fSide.push_back(fs_v);
		mHeel.push_back(m_v);

		char msg[256];
		sprintf(msg,"%dº",hAngle);
		curveLabels.push_back(msg);

	}

	char msg[256];
	sprintf(msg,"Sail Lift vs speed - twv=%2.2f [m/s], twa=%2.2fº",
			pWindItem_->getTWV(dg.getTWV()),
			mathUtils::toDeg(pWindItem_->getTWA(dg.getTWA())) );

	// Instantiate a plotter and plot Lift. We new with a raw ptr, then the
	// ownership will be assigned to the pMultiPlotWidget when adding the chart
	VppXYCustomPlotWidget* pLiftPlot= new VppXYCustomPlotWidget(msg,"Fn [-]","Lift [N]");
	for(size_t i=0; i<fN.size(); i++)
		pLiftPlot->addData(fN[i],Lift[i],curveLabels[i]);
	pLiftPlot->rescaleAxes();
	pMultiPlotWidget->addChart(pLiftPlot,0,0);

	//--

	sprintf(msg,"Sail Drag vs speed - twv=%2.2f [m/s], twa=%2.2fº",
			pWindItem_->getTWV(dg.getTWV()),
			mathUtils::toDeg(pWindItem_->getTWA(dg.getTWA())) );

	// Instantiate a plotter and plot Drag. We new with a raw ptr, then the
	// ownership will be assigned to the pMultiPlotWidget when adding the chart
	VppXYCustomPlotWidget* pDragPlot= new VppXYCustomPlotWidget(msg,"Fn [-]","Drag [N]");
	for(size_t i=0; i<fN.size(); i++)
		pDragPlot->addData(fN[i],Drag[i],curveLabels[i]);
	pDragPlot->rescaleAxes();
	pMultiPlotWidget->addChart(pDragPlot,0,1);

	//--

	sprintf(msg,"Drive force vs speed - twv=%2.2f [m/s], twa=%2.2fº",
			pWindItem_->getTWV(dg.getTWV()),
			mathUtils::toDeg(pWindItem_->getTWA(dg.getTWA())) );

	// Instantiate a plotter and plot Driving Force
	VppXYCustomPlotWidget* pDriveFPlot = new VppXYCustomPlotWidget(msg,"Fn [-]","FDrive [N]");
	for(size_t i=0; i<fN.size(); i++)
		pDriveFPlot->addData(fN[i],fDrive[i],curveLabels[i]);
	pDriveFPlot->rescaleAxes();
	pMultiPlotWidget->addChart(pDriveFPlot,1,0);

	//--

	sprintf(msg,"Side force vs speed - twv=%2.2f [m/s], twa=%2.2fº",
			pWindItem_->getTWV(dg.getTWV()),
			mathUtils::toDeg(pWindItem_->getTWA(dg.getTWA())) );

	// Instantiate a plotter and plot Driving Force
	VppXYCustomPlotWidget* pSideFPlot = new VppXYCustomPlotWidget(msg,"Fn [-]","FSide [N]");
	for(size_t i=0; i<fN.size(); i++)
		pSideFPlot->addData(fN[i],fSide[i],curveLabels[i]);
	pSideFPlot->rescaleAxes();
	pMultiPlotWidget->addChart(pSideFPlot,1,1);

	//--

	// Instantiate a plotter and plot Driving Force
	VppXYCustomPlotWidget* pMPlot = new VppXYCustomPlotWidget("Heeling moment vs boat speed","Fn [-]","mHeel [N*m]");
	for(size_t i=0; i<fN.size(); i++)
		pMPlot->addData(fN[i],mHeel[i],curveLabels[i]);
	pMPlot->rescaleAxes();
	pMultiPlotWidget->addChart(pMPlot,2,0);

	// Restore the current solution
	V_  = xbuf(0);
	PHI_= xbuf(1);
	b_  = xbuf(2);
	f_  = xbuf(3);

}

// Get the value of the side force
const double AeroForcesItem::getLift() const {
	return lift_;
}

// Get the value of the side force
const double AeroForcesItem::getDrag() const {
	return drag_;
}

// Get the value of the side force
const double AeroForcesItem::getFSide() const {
	return fSide_;
}

// Get the value of the drive force
const double AeroForcesItem::getFDrive() const {
	return fDrive_;
}

/// Get the value of the heel moment
const double AeroForcesItem::getMHeel() const {
	return mHeel_;
}

// Get a ptr to the wind item
WindItem* AeroForcesItem::getWindItem() {
	return pWindItem_;
}

// Get a ptr to the wind item - const variety
const WindItem* AeroForcesItem::getWindItem() const {
	return pWindItem_;
}

// Get a ptr to the sailCoeffs Item
SailCoefficientItem* AeroForcesItem::getSailCoeffItem() {
	return pSailCoeffs_;
}

// Get a ptr to the sailCoeffs Item - const variety
const SailCoefficientItem* AeroForcesItem::getSailCoeffItem() const {
	return pSailCoeffs_;
}
