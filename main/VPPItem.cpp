#include "VPPItem.h"

// Constructor
VPPItem::VPPItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet) :
pParser_(pParser),
pSailSet_(pSailSet),
V_(0),
PHI_(0),
b_(0),
f_(0) {

}

// Destructor
VPPItem::~VPPItem() {

}

// Returns the parser
VariableFileParser* VPPItem::getParser() const {
	return pParser_;
}

// Returns the SailSet
boost::shared_ptr<SailSet> VPPItem::getSailSet() const {
	return pSailSet_;
}

// Update the items for the current step (wind velocity and angle),
// the value of the state vector x computed by the optimizer
void VPPItem::update(int vTW, int aTW, const double* x) {

	// Update the local copy of the state variables
	V_= x[0];
	PHI_= x[1];
	b_= x[2];
	f_= x[3];

	// Now call the implementation of the pure virtual update(int,int)
	// for every child
	update(vTW,aTW);

}

void VPPItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of VPPItem "<<std::endl;
}

//=================================================================

// Constructor
VPPItemFactory::VPPItemFactory(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet) {

	//Instantiate the wind
	boost::shared_ptr<WindItem> pWind(new WindItem(pParser,pSailSet));

	// Push it back to the children vector
	vppItems_.push_back( pWind );

	// Instantiate the sail coefficients
	boost::shared_ptr<SailCoefficientItem> pSailCoeffItem(new SailCoefficientItem(pWind.get()));

	// Push it back to the children vector
	vppItems_.push_back( pSailCoeffItem );

	// ----

	// for all of the instantiated, print whoAmI:
	for(size_t iItem=0; iItem<vppItems_.size(); iItem++){
		vppItems_[iItem]->printWhoAmI();
	}
}

// Destructor
VPPItemFactory::~VPPItemFactory(){

}

// Update the VPPItems for the current step (wind velocity and angle),
// the value of the state vector x computed by the optimizer
void VPPItemFactory::update(int vTW, int aTW, const double* x) {

	// for all of the instantiated, print whoAmI:
	for(size_t iItem=0; iItem<vppItems_.size(); iItem++)
		vppItems_[iItem]->update(vTW,aTW,x);

}

//=========================================================

/// Constructor
WindItem::WindItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet) :
				VPPItem(pParser,pSailSet),
				twv_(0),
				twa_(0),
				awa_(0),
				awv_(Eigen::Vector2d::Zero()),
				pi_(M_PI / 180.0){

}

/// Destructor
WindItem::~WindItem() {

}

/// Update the items for the current step (wind velocity and angle)
void WindItem::update(int vTW, int aTW) {

	// Get a reference to the varSet
	const VarSet& v = *(pParser_->getVariables());

	// todo dtrimarchi : it this method is called n times by the optimizer, so it is
	// stupid to re-compute twv_ e twa_ at each time!

	// Update the true wind velocity
	// vmin to vmax in N steps : vMin + vTW * ( (vMax-vMin)/(nSteps-2) - 1 )
	twv_= v["V_TW_MIN"] + vTW * ( ( v["V_TW_MAX"] - v["V_TW_MIN"] ) / ( v["N_TWV"] - 2 ) - 1 );

	// Update the true wind angle: make as per the velocity
	twa_= v["ALPHA_TW_MIN"] + vTW * ( ( v["ALPHA_TW_MAX"] - v["ALPHA_TW_MIN"] ) / ( v["N_ALPHA_TW"] - 2 ) - 1 );

	// Update the apparent wind velocity vector
	awv_(0)= V_ + twv_ * cos( twa_ * pi_  );
	awv_(1)= twv_ * sin( twa_ * pi_  ) * cos( PHI_ * pi_);

	// Update the apparent wind angle - todo dtrimarchi: why do I need to
	// explicitly cast to a double for the indexer to resolve..?
	awa_= atan( double(awv_(1)/awv_(0)) );

}

void WindItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of WindItem "<<std::endl;
}

/// Returns the true wind velocity for this step
const double WindItem::getTWV() const {
	return twv_;
}

/// Returns the true wind angle for this step
const double WindItem::getTWA() const {
	return twa_;
}

/// Returns the apparent wind angle for this step
const double WindItem::getAWA() const {
	return awa_;
}

/// Returns the apparent wind velocity vector for this step
const Eigen::Vector2d WindItem::getAWV() const {
	return awv_;
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

	// Pick the lift coefficient for this main (full batten or not)
	if( pParser_->get("MFBL") ) {

		clMat0_.resize(6,4);
		clMat0_.row(0) << 0, 		0, 			0, 		0   ;
		clMat0_.row(1) << 27, 	1.725,	1.5,	0   ;
		clMat0_.row(2) << 50, 	1.5, 		0.5,	1.5 ;
		clMat0_.row(3) << 80,		0.95, 	0.3,	1.0 ;
		clMat0_.row(4) << 100,	0.85, 	0.0,	0.85;
		clMat0_.row(5) << 180,	0, 			0, 		0		;

	} else {

		clMat0_.resize(6,4);
		clMat0_.row(0) << 0, 		0, 	 	0 , 	0;
		clMat0_.row(1) << 27, 	1.5, 	1.5,	0;
		clMat0_.row(2) << 50, 	1.5, 	0.5,	1.5;
		clMat0_.row(3) << 80, 	0.95,	0.3,	1.0;
		clMat0_.row(4) << 100,	0.85,	0.0,	0.85;
		clMat0_.row(5) << 180,	0, 	 	0,	 	0;

	}

		// We only dispose of one drag coeff array for the moment
		cdpMat0_.resize(6,4);
		cdpMat0_.row(0) << 0,  	0,   	0,   	0;
		cdpMat0_.row(1) << 27, 	0.02,	0.02,	0;
		cdpMat0_.row(2) << 50, 	0.15,	0.25,	0.25;
		cdpMat0_.row(3) << 80, 	0.8, 	0.15,	0.9;
		cdpMat0_.row(4) << 100,	1.0, 	0.0, 	1.2;
		allCd_.row(5) << 180,	0.9, 	0.0, 	0.66;

		// resize cl_ and cd_ for storing the interpolated values
		allCl_.resize(3);
		allCd_.resize(3);
}

// Destructor
SailCoefficientItem::~SailCoefficientItem() {

}

// Implement the pure virtual. Called by the children as a decorator
void SailCoefficientItem::update(int vTW, int aTW) {

	// Update the local copy of the the apparent wind angle
	awa_= pWindItem_->getAWA();

	// create aliases for code readability
	VariableFileParser* p = pParser_;
	boost::shared_ptr<SailSet> s= pSailSet_;

	// Update the Aspect Ratio
	double h;
	if(awa_ < 45)  // todo dtrimarchi: verify that awa is in deg... Which I doubt!
		// h = mast height above deck + Average freeboard
		h= p->get("EHM") + p->get("AVGFREB");
	else
		// h = mast height above deck
		h= p->get("EHM");

	// Compute the aspect ratio for this awa_
	ar_ = 1.1 * h * h / s->get("AN");

	// Compute cd0
	cd0_= 1.13 * ( (p->get("B") * p->get("AVGFREB")) + (p->get("AVGFREB")*p->get("AVGFREB") ) ) / s->get("AN");

}

// Called by the children as a decorator for update.
// Do all is required after cl and cdp have been computed
void SailCoefficientItem::postUpdate() {

	// Reduce cl with the flattening factor of the state vector
	cl_ = cl_ * f_;

	// Compute the induced resistance
	cdI_ = cl_ * cl_ * ( 1. / (M_PI * ar_) + 0.005 );

	// Compute the total sail drag coefficient now
	cd_ = cdp_ + cd0_ + cdI_;
}

void SailCoefficientItem::computeForMain() {

	Eigen::ArrayXd x, y;

	// Interpolate the values of the sail coefficients for the MainSail
	x=clMat0_.col(0);
	y=clMat0_.col(1);
	allCl_(0) = interpolator_.interpolate(awa_,x,y);

	x=cdpMat0_.col(0);
	y=cdpMat0_.col(1);
	allCd_(0) = interpolator_.interpolate(awa_,x,y);

}

void SailCoefficientItem::computeForJib() {

	Eigen::ArrayXd x, y;

	// Interpolate the values of the sail coefficients for the Jib
	x=clMat0_.col(0);
	y=clMat0_.col(2);
	allCl_(1) = interpolator_.interpolate(awa_,x,y);

	x=cdpMat0_.col(0);
	y=cdpMat0_.col(2);
	allCd_(1) = interpolator_.interpolate(awa_,x,y);

}

void SailCoefficientItem::computeForSpi() {

	Eigen::ArrayXd x, y;

	// Interpolate the values of the sail coefficients for the Spi
	x=clMat0_.col(0);
	y=clMat0_.col(3);
	allCl_(2) = interpolator_.interpolate(awa_,x,y);

	x=cdpMat0_.col(0);
	y=cdpMat0_.col(3);
	allCd_(2) = interpolator_.interpolate(awa_,x,y);

}

// Returns the current value of the lift coefficient for
// the current sail
const double SailCoefficientItem::getCl() const {
	return cl_;
}

// Returns the current value of the lift coefficient
const double SailCoefficientItem::getCd() const {
	return cdp_;
}

// Print the class name -> in this case SailCoefficientItem
void SailCoefficientItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of SailCoefficientItem "<<std::endl;

	printCoefficients();
}

/// PrintOut the coefficient matrices
void SailCoefficientItem::printCoefficients() {

	std::cout<<"\n=== Sail Coefficients: ============\n "<<std::endl;
	std::cout<<"Cl= \n"<<clMat0_<<std::endl;
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

// Update the item for the current step (wind velocity and angle),
// the values of the state vector x computed by the optimizer have
// already been treated by the parent
void MainOnlySailCoefficientItem::update(int vTW, int aTW) {

	// Decorate the parent class method that updates the value of awa_
	SailCoefficientItem::update(vTW,aTW);

	// Compute the sail coefficients for the main sail
	computeForMain();

	// In this case the lift/drag coeffs are just what was computed for main
	cl_ = allCl_(0);
	cdp_ = allCd_(0);

	// Call the parent method that computes the effective cd=cdp+cd0+cdI
	SailCoefficientItem::postUpdate();

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

// Update the item for the current step (wind velocity and angle),
// the values of the state vector x computed by the optimizer have
// already been treated by the parent
void MainAndJibCoefficientItem::update(int vTW, int aTW) {

	// Decorate the parent class method that updates the value of awa_
	SailCoefficientItem::update(vTW,aTW);

	// Compute the sail coefficients for the main sail and the Jib
	computeForMain();
	computeForJib();

	// create an alias for code readability
	boost::shared_ptr<SailSet> ps= pSailSet_;

	// 	Cl = ( Cl_M * AM + Cl_J * AJ ) / AN
	cl_ = ( allCl_(0) * ps->get("AM") + allCl_(1) *  ps->get("AJ") ) /  ps->get("AN");
	cdp_ = ( allCd_(0) * ps->get("AM") + allCd_(1) *  ps->get("AJ") ) /  ps->get("AN");

	// Call the parent method that computes the effective cd=cdp+cd0+cdI
	SailCoefficientItem::postUpdate();

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

// Update the item for the current step (wind velocity and angle),
// the values of the state vector x computed by the optimizer have
// already been treated by the parent
void MainAndSpiCoefficientItem::update(int vTW, int aTW) {

	// Decorate the parent class method that updates the value of awa_
	SailCoefficientItem::update(vTW,aTW);

	// Compute the sail coefficients for the main sail and the Jib
	computeForMain();
	computeForSpi();

	// create an alias for code readability
	boost::shared_ptr<SailSet> ps= pSailSet_;

	// 	Cl = ( Cl_M * AM + Cl_J * AJ ) / AN
	cl_ = ( allCl_(0) * ps->get("AM") + allCl_(2) *  ps->get("AS") ) /  ps->get("AN");
	cdp_ = ( allCd_(0) * ps->get("AM") + allCd_(2) *  ps->get("AS") ) /  ps->get("AN");

	// Call the parent method that computes the effective cd=cdp+cd0+cdI
	SailCoefficientItem::postUpdate();

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

// Update the item for the current step (wind velocity and angle),
// the values of the state vector x computed by the optimizer have
// already been treated by the parent
void MainJibAndSpiCoefficientItem::update(int vTW, int aTW) {

	// Decorate the parent class method that updates the value of awa_
	SailCoefficientItem::update(vTW,aTW);

	// Compute the sail coefficients for the main sail and the Jib
	computeForMain();
	computeForJib();
	computeForSpi();

	// create an alias for code readability
	boost::shared_ptr<SailSet> ps= pSailSet_;

	// 	Cl = ( Cl_M * AM + Cl_J * AJ ) / AN
	cl_ = ( allCl_(0) * ps->get("AM") + allCl_(1) *  ps->get("AJ") + allCl_(2) *  ps->get("AS") ) /  ps->get("AN");
	cdp_ = ( allCd_(0) * ps->get("AM") + allCd_(1) *  ps->get("AJ") + allCd_(2) *  ps->get("AS") ) /  ps->get("AN");

	// Call the parent method that computes the effective cd=cdp+cd0+cdI
	SailCoefficientItem::postUpdate();

}

//=================================================================

// Constructor
AeroForcesItem::AeroForcesItem(WindItem* pWindItem) :
		VPPItem(pWindItem->getParser(), pWindItem->getSailSet() ),
		pWindItem_(pWindItem) {

	// Instantiate the sail coefficients
	pSailCoeffs_= boost::shared_ptr<SailCoefficientItem>(new SailCoefficientItem(pWindItem_));
}

// Destructor
AeroForcesItem::~AeroForcesItem() {

}

/// Update the items for the current step (wind velocity and angle)
void AeroForcesItem::update(int vTW, int aTW) {


}

void AeroForcesItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of AeroForcesItem "<<std::endl;
}

