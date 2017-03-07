#include "VPPAeroItem.h"

#include "IOUtils.h"
#include "VPPException.h"
#include "mathUtils.h"
#include "multiplePlotWidget.h"
#include "VppTabDockWidget.h"
#include "VPPXYChart.h"
#include "VppXYCustomPlotWidget.h"

using namespace mathUtils;

/// Constructor
WindItem::WindItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet) :
								VPPItem(pParser,pSailSet),
								twv_(0),
								twa_(0),
								awa_(0),
								awv_(Eigen::Vector2d::Zero()) {

	// Get the max/min wind velocities from the parser
	v_tw_min_= pParser_->get("V_TW_MIN");
	v_tw_max_= pParser_->get("V_TW_MAX");
	n_twv_= pParser_->get("N_TWV");

	// Fill the values of the wind true velocities
	vTwv_.resize(n_twv_);

	// reset and compute the delta for awv
	double delta=0;
	if(n_twv_>1)
		delta=( ( v_tw_max_ - v_tw_min_ ) / (n_twv_-1) );

	for(size_t i=0; i<n_twv_; i++)
		vTwv_[i]= v_tw_min_ + i * delta;

	// Get the max/min wind angles [rad] from the parser
	alpha_tw_min_= pParser_->get("ALPHA_TW_MIN");
	alpha_tw_max_= pParser_->get("ALPHA_TW_MAX");
	n_alpha_tw_= pParser_->get("N_ALPHA_TW");

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

void WindItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of WindItem "<<std::endl;
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

	// Init static members with values from: Hazer Cl-Cd coefficients, 1999
	// Cols: rwa_, Main_Cl, Jib_Cl, Spi_Cl
	Eigen::ArrayXXd clMat0,cdpMat0;

	// Pick the lift coefficient for this main (full batten or not)
	if( pParser_->get("MFLB") ) {

		clMat0.resize(9,4);
		clMat0.row(0) << 0, 	0, 			0, 		0   ;
		clMat0.row(1) << 20, 	1.3, 	  1.2,	0.02;
		clMat0.row(2) << 27, 	1.725,	1.5,	0.1 ;
		clMat0.row(3) << 50, 	1.5, 		0.5,	1.5 ;
		clMat0.row(4) << 60, 	1.25, 	0.4,	1.25;
		clMat0.row(5) << 80,	0.95, 	0.3,	1.0 ;
		clMat0.row(6) << 100,	0.85, 	0.1,	0.85;
		clMat0.row(7) << 140,	0.2, 		0.05,	0.2 ;
		clMat0.row(8) << 180,	0, 			0, 		0		;

	} else {

		clMat0.resize(9,4);
		clMat0.row(0) << 0, 	0, 	 	0 , 	0;
		clMat0.row(1) << 20, 	1.2, 	1.2,   0.02;
		clMat0.row(2) << 27, 	1.5, 	1.5,	0.1;
		clMat0.row(3) << 50, 	1.5, 	0.5,	1.5;
		clMat0.row(4) << 60, 	1.25, 0.4,	1.25;
		clMat0.row(5) << 80, 	0.95,	0.3,	1.0;
		clMat0.row(6) << 100,	0.85,	0.0,	0.85;
		clMat0.row(7) << 140,	0.2, 		0.05,	0.2 ;
		clMat0.row(8) << 180,	0., 		0.05,	0.1 ;

	}

	// Convert the angular values to radians
	clMat0.col(0) *= M_PI / 180.0;

	// We only dispose of one drag coeff array for the moment
	cdpMat0.resize(8,4);
	cdpMat0.row(0) << 0,  	0,   	0,   	0;
	cdpMat0.row(1) << 15,  	0.02, 0.005, 0.02;
	cdpMat0.row(2) << 27, 	0.03,	0.02,	0.05;
	cdpMat0.row(3) << 50, 	0.15,	0.25,	0.25;
	cdpMat0.row(4) << 80, 	0.8, 	0.15,	0.9;
	cdpMat0.row(5) << 100,	1.0, 	0.05, 1.2;
	cdpMat0.row(6) << 140,	0.95, 	0.01, 0.8;
	cdpMat0.row(7) << 180,	0.9, 	0.0, 	0.66;

	// Convert the angular values to radians
	cdpMat0.col(0) *= M_PI / 180.0;

	// Reset the interpolator vectors before filling them
	interpClVec_.clear();
	interpCdVec_.clear();

	Eigen::ArrayXd x, y;
	x=clMat0.col(0);
	// Interpolate the values of the sail coefficients for the MainSail
	for(size_t i=1; i<4; i++){
		y=clMat0.col(i);
		interpClVec_.push_back( boost::shared_ptr<SplineInterpolator>( new SplineInterpolator(x,y) ) );
	}
	x=cdpMat0.col(0);
	// Interpolate the values of the sail coefficients for the MainSail
	for(size_t i=1; i<4; i++){
		y=cdpMat0.col(i);
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

/// Returns a ptr to the wind Item
WindItem* SailCoefficientItem::getWindItem() const {
	return pWindItem_;
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

// Print the class name -> in this case SailCoefficientItem
void SailCoefficientItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of SailCoefficientItem "<<std::endl;

	printCoefficients();
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

// Print the class name - implement the pure virtual of VPPItem
void MainOnlySailCoefficientItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of MainOnlySailCoefficientItem "<<std::endl;
}

void MainOnlySailCoefficientItem::plotInterpolatedCoefficients() const {

	interpClVec_[activeSail::mainSail] -> plot(0,toRad(180),50,"Interpolated CL for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail] -> plot(0,toRad(180),50,"Interpolated CD for MAIN","AWA [rad]","[-]");

}

// Plot the spline-interpolated curves based on the Larsson's
// sail coefficients. The range is set 0-180deg
// Fill a multiple plot
void MainOnlySailCoefficientItem::plotInterpolatedCoefficients( MultiplePlotWidget* multiPlotWidget ) const {

	// -> Instantiate a XYPlotWidget for cl. Set title and axis title
	VPPXYChart clPlot("Interpolated CL for MAIN","AWA [rad]","[-]");

	// Ask the interpolator for vectors
	interpClVec_[activeSail::mainSail]->plot(clPlot,0,toRad(180),50);

	multiPlotWidget->addChart(clPlot,0,0);

	//---

	// -> same for cd
	//VPPXYChart cdPlot("Interpolated CD for MAIN","AWA [rad]","[-]");

	// -> push the charts to the multiplot
	//interpCdVec_[activeSail::mainSail] -> plot(0,toRad(180),50,cdPlot);
}

void MainOnlySailCoefficientItem::plot_D_InterpolatedCoefficients() const {

	interpClVec_[activeSail::mainSail] -> plotD1(0,toRad(180),50,"Interpolated CL for MAIN - first derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail] -> plotD1(0,toRad(180),50,"Interpolated CD for MAIN - first derivative","AWA [rad]","[-]");

}

void MainOnlySailCoefficientItem::plot_D2_InterpolatedCoefficients() const {

	interpClVec_[activeSail::mainSail] -> plotD2(0,toRad(180),50,"Interpolated CL for MAIN - second derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail] -> plotD2(0,toRad(180),50,"Interpolated CD for MAIN - second derivative","AWA [rad]","[-]");

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

// Print the class name - implement the pure virtual of VPPItem
void MainAndJibCoefficientItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of MainAndJibCoefficientItem "<<std::endl;
}

void MainAndJibCoefficientItem::plotInterpolatedCoefficients() const {

	interpClVec_[activeSail::mainSail] -> plot(0,toRad(180),50,"Interpolated CL for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail] -> plot(0,toRad(180),50,"Interpolated CD for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::jib] -> plot(0,toRad(180),50,"Interpolated CL for JIB","AWA [rad]","[-]");
	interpCdVec_[activeSail::jib] -> plot(0,toRad(180),50,"Interpolated CD for JIB","AWA [rad]","[-]");

}

// Plot the spline-interpolated curves based on the Larsson's sail coefficients.
// The range is set 0-180deg
void MainAndJibCoefficientItem::plotInterpolatedCoefficients( MultiplePlotWidget* multiPlotWidget ) const {

	// -> Instantiate a VppXYCustomPlotWidget for cl of Main. Set title and axis title
	VppXYCustomPlotWidget* pClMainPlot = new VppXYCustomPlotWidget("Interpolated CL for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::mainSail]->plot(pClMainPlot,0,toRad(180),50);

	// Add it to the multiplot widget
	multiPlotWidget->addChart(pClMainPlot,0,0);

	// --

	// -> Instantiate a VppXYCustomPlotWidget for cl of Jib. Set title and axis title
	VppXYCustomPlotWidget* pClJibPlot = new VppXYCustomPlotWidget("Interpolated CL for JIB","AWA [rad]","[-]");
	interpClVec_[activeSail::jib]->plot(pClJibPlot,0,toRad(180),50);

	// Add it to the multiplot widget
	multiPlotWidget->addChart(pClJibPlot,1,0);

	// --

	// -> Instantiate a VppXYCustomPlotWidget for cd of Main. Set title and axis title
	VppXYCustomPlotWidget* pCdMainPlot = new VppXYCustomPlotWidget("Interpolated CD for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail]->plot(pCdMainPlot,0,toRad(180),50);

	// Add it to the multiplot widget
	multiPlotWidget->addChart(pClMainPlot,0,1);

	// --

	// -> Instantiate a VppXYCustomPlotWidget for cd of Jib. Set title and axis title
	VppXYCustomPlotWidget* pCdJibPlot = new VppXYCustomPlotWidget("Interpolated CD for Jib","AWA [rad]","[-]");
	interpCdVec_[activeSail::jib]->plot(pCdJibPlot,0,toRad(180),50);

	// Add it to the multiplot widget
	multiPlotWidget->addChart(pCdJibPlot,1,1);

}

void MainAndJibCoefficientItem::plot_D_InterpolatedCoefficients() const {

	interpClVec_[activeSail::mainSail] -> plotD1(0,toRad(180),50,"Interpolated CL for MAIN - first derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail] -> plotD1(0,toRad(180),50,"Interpolated CD for MAIN - first derivative","AWA [rad]","[-]");
	interpClVec_[activeSail::jib] -> plotD1(0,toRad(180),50,"Interpolated CL for JIB - first derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::jib] -> plotD1(0,toRad(180),50,"Interpolated CD for JIB - first derivative","AWA [rad]","[-]");

}

void MainAndJibCoefficientItem::plot_D2_InterpolatedCoefficients() const {

	interpClVec_[activeSail::mainSail] -> plotD2(0,toRad(180),50,"Interpolated CL for MAIN - second derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail] -> plotD2(0,toRad(180),50,"Interpolated CD for MAIN - second derivative","AWA [rad]","[-]");
	interpClVec_[activeSail::jib] -> plotD2(0,toRad(180),50,"Interpolated CL for JIB - second derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::jib] -> plotD2(0,toRad(180),50,"Interpolated CD for JIB - second derivative","AWA [rad]","[-]");

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

// Print the class name - implement the pure virtual of VPPItem
void MainAndSpiCoefficientItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of MainAndSpiCoefficientItem "<<std::endl;
}

void MainAndSpiCoefficientItem::plotInterpolatedCoefficients() const {

	interpClVec_[activeSail::mainSail] -> plot(0,toRad(180),50,"Interpolated CL for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail] -> plot(0,toRad(180),50,"Interpolated CD for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::spi] -> plot(0,toRad(180),50,"Interpolated CL for SPI","AWA [rad]","[-]");
	interpCdVec_[activeSail::spi] -> plot(0,toRad(180),50,"Interpolated CD for SPI","AWA [rad]","[-]");

}

// Plot the spline-interpolated curves based on the Larsson's
// sail coefficients. The range is set 0-180deg
// Fill a multiple plot
void MainAndSpiCoefficientItem::plotInterpolatedCoefficients( MultiplePlotWidget* multiPlotWidget ) const {

}

void MainAndSpiCoefficientItem::plot_D_InterpolatedCoefficients() const {

	interpClVec_[activeSail::mainSail] -> plotD1(0,toRad(180),50,"Interpolated CL for MAIN - first derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail] -> plotD1(0,toRad(180),50,"Interpolated CD for MAIN - first derivative","AWA [rad]","[-]");
	interpClVec_[activeSail::spi] -> plotD1(0,toRad(180),50,"Interpolated CL for SPI - first derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::spi] -> plotD1(0,toRad(180),50,"Interpolated CD for SPI - first derivative","AWA [rad]","[-]");

}

void MainAndSpiCoefficientItem::plot_D2_InterpolatedCoefficients() const {

	interpClVec_[activeSail::mainSail] -> plotD2(0,toRad(180),50,"Interpolated CL for MAIN - second derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail] -> plotD2(0,toRad(180),50,"Interpolated CD for MAIN - second derivative","AWA [rad]","[-]");
	interpClVec_[activeSail::spi] -> plotD2(0,toRad(180),50,"Interpolated CL for SPI - second derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::spi] -> plotD2(0,toRad(180),50,"Interpolated CD for SPI - second derivative","AWA [rad]","[-]");

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

// Print the class name - implement the pure virtual of VPPItem
void MainJibAndSpiCoefficientItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of MainJibAndSpiCoefficientItem "<<std::endl;
}

void MainJibAndSpiCoefficientItem::plotInterpolatedCoefficients() const {

	interpClVec_[activeSail::mainSail] -> plot(0,toRad(180),50,"Interpolated CL for MAIN","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail] -> plot(0,toRad(180),50,"Interpolated CD for MAIN","AWA [rad]","[-]");
	interpClVec_[activeSail::jib] -> plot(0,toRad(180),50,"Interpolated CL for JIB","AWA [rad]","[-]");
	interpCdVec_[activeSail::jib] -> plot(0,toRad(180),50,"Interpolated CD for JIB","AWA [rad]","[-]");
	interpClVec_[activeSail::spi] -> plot(0,toRad(180),50,"Interpolated CL for SPI","AWA [rad]","[-]");
	interpCdVec_[activeSail::spi] -> plot(0,toRad(180),50,"Interpolated CD for SPI","AWA [rad]","[-]");

}

// Plot the spline-interpolated curves based on the Larsson's
// sail coefficients. The range is set 0-180deg
// Fill a multiple plot
void MainJibAndSpiCoefficientItem::plotInterpolatedCoefficients( MultiplePlotWidget* multiPlotWidget ) const {

}

void MainJibAndSpiCoefficientItem::plot_D_InterpolatedCoefficients() const {

	interpClVec_[activeSail::mainSail] -> plotD1(0,toRad(180),50,"Interpolated CL for MAIN - first derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail] -> plotD1(0,toRad(180),50,"Interpolated CD for MAIN - first derivative","AWA [rad]","[-]");
	interpClVec_[activeSail::jib] -> plotD1(0,toRad(180),50,"Interpolated CL for JIB - first derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::jib] -> plotD1(0,toRad(180),50,"Interpolated CD for JIB - first derivative","AWA [rad]","[-]");
	interpClVec_[activeSail::spi] -> plotD1(0,toRad(180),50,"Interpolated CL for SPI - first derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::spi] -> plotD1(0,toRad(180),50,"Interpolated CD for SPI - first derivative","AWA [rad]","[-]");

}

void MainJibAndSpiCoefficientItem::plot_D2_InterpolatedCoefficients() const {

	interpClVec_[activeSail::mainSail] -> plotD2(0,toRad(180),50,"Interpolated CL for MAIN - second derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::mainSail] -> plotD2(0,toRad(180),50,"Interpolated CD for MAIN - second derivative","AWA [rad]","[-]");
	interpClVec_[activeSail::jib] -> plotD2(0,toRad(180),50,"Interpolated CL for JIB - second derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::jib] -> plotD2(0,toRad(180),50,"Interpolated CD for JIB - second derivative","AWA [rad]","[-]");
	interpClVec_[activeSail::spi] -> plotD2(0,toRad(180),50,"Interpolated CL for SPI - second derivative","AWA [rad]","[-]");
	interpCdVec_[activeSail::spi] -> plotD2(0,toRad(180),50,"Interpolated CD for SPI - second derivative","AWA [rad]","[-]");

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
void AeroForcesItem::plot() {

	// Number of points of the plots
	size_t nVelocities=60;

	// For which TWV, TWA shall we plot the aero forces/moments?
	size_t twv=0, twa=0;
	IOUtils io(pWindItem_);
	io.askUserWindIndexes(twv, twa);

	// Buffer the current solution
	Eigen::VectorXd xbuf(4);
	xbuf << V_,PHI_,b_,f_;

	// Now fix the value of b_ and f_
	b_= 0.01;
	f_= 0.99;

	// Declare containers for the velocity and angle-wise data
	std::vector<ArrayXd> v, Lift, Drag, fDrive, fSide, mHeel;
	std::vector<string> curveLabels;

	// Loop on heel angles : from 0 to 90 deg in steps of 15 deg
	for(int hAngle=-20; hAngle<90; hAngle+=15){

		// Convert the heeling angle into radians
		PHI_= toRad(hAngle);
		// vectors with the current boat velocity, the drive force
		// and heeling moment values
		ArrayXd x_v, lift, drag, f_v, fs_v, m_v;
		x_v.resize(nVelocities);
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
			pWindItem_->updateSolution(twv, twa, stateVector);

			// Update the sail coefficients for the current wind
			pSailCoeffs_->updateSolution(twv, twa, stateVector);

			// Update 'this': compute sail forces
			update(twv, twa);

			// Store velocity-wise data:
			x_v(iTwv)= V_ / sqrt( Physic::g * pParser_->get("LWL") );	// Fn...
			lift(iTwv) = getLift(); // lift...
			drag(iTwv) = getDrag(); // drag...
			f_v(iTwv)= getFDrive(); // fDrive...
			fs_v(iTwv)= getFSide(); // fSide_...
			m_v(iTwv)= getMHeel();  // mHeel...

		}

		// Append the velocity-wise curve for each heel angle
		v.push_back(x_v);
		Lift.push_back(lift);
		Drag.push_back(drag);
		fDrive.push_back(f_v);
		fSide.push_back(fs_v);
		mHeel.push_back(m_v);

		char msg[256];
		sprintf(msg,"%dº",hAngle);
		curveLabels.push_back(msg);

	}

	// Instantiate a plotter and plot Lift
	VPPPlotter liftPlotter;
	for(size_t i=0; i<v.size(); i++)
		liftPlotter.append(curveLabels[i],v[i],Lift[i]);

	char msg[256];
	sprintf(msg,"plot Sail Lift vs boat speed - "
			"twv=%2.2f [m/s], twa=%2.2fº",
			pWindItem_->getTWV(twv),
			mathUtils::toDeg(pWindItem_->getTWA(twa)) );
	liftPlotter.plot("Fn [-]","Lift [N]", msg);


	// Instantiate a plotter and plot Drag
	VPPPlotter dragPlotter;
	for(size_t i=0; i<v.size(); i++)
		dragPlotter.append(curveLabels[i],v[i],Drag[i]);

	sprintf(msg,"plot Sail Drag vs boat speed - "
			"twv=%2.2f [m/s], twa=%2.2fº",
			pWindItem_->getTWV(twv),
			mathUtils::toDeg(pWindItem_->getTWA(twa)) );
	dragPlotter.plot("Fn [-]","Drag [N]", msg);

	// Instantiate a plotter and plot fDrive
	VPPPlotter fPlotter;
	for(size_t i=0; i<v.size(); i++)
		fPlotter.append(curveLabels[i],v[i],fDrive[i]);

	sprintf(msg,"plot drive force vs boat speed - "
			"twv=%2.2f [m/s], twa=%2.2fº",
			pWindItem_->getTWV(twv),
			mathUtils::toDeg(pWindItem_->getTWA(twa)) );
	fPlotter.plot("Fn [-]","Fdrive [N]", msg);

	// Instantiate a plotter and plot fSide
	VPPPlotter fSidePlotter;
	for(size_t i=0; i<v.size(); i++)
		fSidePlotter.append(curveLabels[i],v[i],fSide[i]);

	sprintf(msg,"plot side force vs boat speed - "
			"twv=%2.2f [m/s], twa=%2.2fº",
			pWindItem_->getTWV(twv),
			mathUtils::toDeg(pWindItem_->getTWA(twa)) );
	fSidePlotter.plot("Fn [-]","Fside [N]", msg);

	// Instantiate a plotter and plot mHeel
	VPPPlotter mPlotter;
	for(size_t i=0; i<v.size(); i++)
		mPlotter.append(curveLabels[i],v[i],mHeel[i]);
	mPlotter.plot("Fn [-]","mHeel [N*m]","plot heeling moment vs boat speed");

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

void AeroForcesItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of AeroForcesItem "<<std::endl;
}
