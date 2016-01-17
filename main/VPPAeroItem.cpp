#include "VPPAeroItem.h"
#include "mathUtils.h"
using namespace mathUtils;

/// Constructor
WindItem::WindItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet) :
				VPPItem(pParser,pSailSet),
				twv_(0),
				twa_(0),
				awa_(0),
				awv_(Eigen::Vector2d::Zero()) {

}

/// Destructor
WindItem::~WindItem() {

}

/// Update the items for the current step (wind velocity and angle)
void WindItem::update(int vTW, int aTW) {

	std::cout<<"Updating WindItem"<<std::endl;

	// Get a reference to the varSet
	const VarSet& v = *(pParser_->getVariables());

	// todo dtrimarchi : it this method is called n times by the optimizer, so it is
	// stupid to re-compute twv_ e twa_ at each time! But it is the only place where I
	// can update.

	il problema sembra essere qui, probabilmente il gioco della const ref non funziona!

	// Update the true wind velocity
	// vmin to vmax in N steps : vMin + vTW * ( (vMax-vMin)/(nSteps-2) - 1 )
	twv_= v["V_TW_MIN"] + vTW * ( ( v["V_TW_MAX"] - v["V_TW_MIN"] ) / ( v["N_TWV"] - 2 ) - 1 );
	std::cout<<"TWV= "<<twv_<<std::endl;
	// Update the true wind angle: make as per the velocity
	twa_= v["ALPHA_TW_MIN"] + vTW * ( ( v["ALPHA_TW_MAX"] - v["ALPHA_TW_MIN"] ) / ( v["N_ALPHA_TW"] - 2 ) - 1 );
	std::cout<<"TWA= "<<twa_<<std::endl;

	// Update the apparent wind velocity vector
	awv_(0)= V_ + twv_ * cos( toRad(twa_)  );
	awv_(1)= twv_ * sin( toRad(twa_) ) * cos( toRad(PHI_) );
	std::cout<<"awv_(0)= "<<awv_(0)<<std::endl;
	std::cout<<"awv_(1)= "<<awv_(1)<<std::endl;

	// Update the apparent wind angle - todo dtrimarchi: why do I need to
	// explicitly cast to a double for the indexer to resolve..?
	awa_= atan( double(awv_(1)/awv_(0)) );
	if(isnan(awa_)){
		std::cout<<"awa_ is NAN!"<<std::endl;
		throw logic_error("awa_ is NAN!");
	}

}

void WindItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of WindItem "<<std::endl;
}

// Returns the true wind velocity for this step
const double WindItem::getTWV() const {
	return twv_;
}

// Returns the true wind angle for this step
const double WindItem::getTWA() const {
	return twa_;
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

		clMat0.resize(6,4);
		clMat0.row(0) << 0, 		0, 			0, 		0   ;
		clMat0.row(1) << 27, 	1.725,	1.5,	0   ;
		clMat0.row(2) << 50, 	1.5, 		0.5,	1.5 ;
		clMat0.row(3) << 80,		0.95, 	0.3,	1.0 ;
		clMat0.row(4) << 100,	0.85, 	0.0,	0.85;
		clMat0.row(5) << 180,	0, 			0, 		0		;

	} else {

		clMat0.resize(6,4);
		clMat0.row(0) << 0, 		0, 	 	0 , 	0;
		clMat0.row(1) << 27, 	1.5, 	1.5,	0;
		clMat0.row(2) << 50, 	1.5, 	0.5,	1.5;
		clMat0.row(3) << 80, 	0.95,	0.3,	1.0;
		clMat0.row(4) << 100,	0.85,	0.0,	0.85;
		clMat0.row(5) << 180,	0, 	 	0,	 	0;

	}

		// We only dispose of one drag coeff array for the moment
		cdpMat0.resize(6,4);
		cdpMat0.row(0) << 0,  	0,   	0,   	0;
		cdpMat0.row(1) << 27, 	0.02,	0.02,	0;
		cdpMat0.row(2) << 50, 	0.15,	0.25,	0.25;
		cdpMat0.row(3) << 80, 	0.8, 	0.15,	0.9;
		cdpMat0.row(4) << 100,	1.0, 	0.0, 	1.2;
		cdpMat0.row(5) << 180,	0.9, 	0.0, 	0.66;

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

		// resize cl_ and cd_ for storing the interpolated values
		allCl_.resize(3);
		allCd_.resize(3);
}

// Destructor
SailCoefficientItem::~SailCoefficientItem() {
}

// Implement the pure virtual
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

/// Returns a ptr to the wind Item
WindItem* SailCoefficientItem::getWindItem() const {
	return pWindItem_;
}


void SailCoefficientItem::computeForMain() {

	Eigen::ArrayXd x, y;

	// Interpolate the values of the sail coefficients for the MainSail
	allCl_(0) = interpClVec_[0]->interpolate(awa_);
	allCd_(0) = interpCdVec_[0]->interpolate(awa_);

}

void SailCoefficientItem::computeForJib() {

	Eigen::ArrayXd x, y;

	// Interpolate the values of the sail coefficients for the Jib
	allCl_(1) = interpClVec_[1]->interpolate(awa_);
	allCd_(1) = interpCdVec_[1]->interpolate(awa_);

}

void SailCoefficientItem::computeForSpi() {

	Eigen::ArrayXd x, y;

	// Interpolate the values of the sail coefficients for the Spi
	allCl_(1) = interpClVec_[2]->interpolate(awa_);
	allCd_(2) = interpCdVec_[2]->interpolate(awa_);

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

// Print the class name - implement the pure virtual of VPPItem
void MainOnlySailCoefficientItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of MainOnlySailCoefficientItem "<<std::endl;
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

// Print the class name - implement the pure virtual of VPPItem
void MainAndJibCoefficientItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of MainAndJibCoefficientItem "<<std::endl;
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

// Print the class name - implement the pure virtual of VPPItem
void MainAndSpiCoefficientItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of MainAndSpiCoefficientItem "<<std::endl;
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

// Print the class name - implement the pure virtual of VPPItem
void MainJibAndSpiCoefficientItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of MainJibAndSpiCoefficientItem "<<std::endl;
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
		fHeel_(0),
		fSide_(0),
		mHeel_(0) {
	// do nothing
}

// Destructor
AeroForcesItem::~AeroForcesItem() {

}

/// Update the items for the current step (wind velocity and angle)
void AeroForcesItem::update(int vTW, int aTW) {

	// Gets the value of the apparent wind velocity
	double awv = pWindItem_->getAWNorm();
	if(isnan(awv)){
		std::cout<<"awv is NAN!"<<std::endl;
		throw logic_error("awv is NAN!");
	}

	double awa = pWindItem_->getAWA();
	if(isnan(awa)){
		std::cout<<"awa is NAN!"<<std::endl;
		throw logic_error("awa is NAN!");
	}

	// Updates Lift = 0.5 * phys.rho_a * V_eff.^2 .* AN .* Cl;
	lift_ = 0.5 * Physic::rho_a * awv * awv * pSailSet_->get("AN") * pSailCoeffs_->getCl();
	if(isnan(lift_)){
		std::cout<<"lift_ is NAN!"<<std::endl;
		throw logic_error("lift_ is NAN!");
	}

	// Updates Drag = 0.5 * phys.rho_a * V_eff.^2 .* AN .* Cd;
	drag_ = 0.5 * rho_a * awv * awv * pSailSet_->get("AN") * pSailCoeffs_->getCd();
	if(isnan(drag_)){
		std::cout<<"drag_ is NAN!"<<std::endl;
		throw logic_error("drag_ is NAN!");
	}

	// Updates Fdrive = lift_ * sin(alfa_eff) - D * cos(alfa_eff);
	fDrive_ = lift_ * sin(awa) - drag_ * cos(awa);
	if(isnan(fDrive_)){
		std::cout<<"fDrive_ is NAN!"<<std::endl;
		throw logic_error("fDrive_ is NAN!");
	}

	// Updates Fheel = L * cos(alfa_eff) + D * sin(alfa_eff);
	fHeel_ = lift_ * cos(awa) + drag_ * sin(awa);
	if(isnan(fHeel_)){
		std::cout<<"fHeel_ is NAN!"<<std::endl;
		throw logic_error("fHeel_ is NAN!");
	}

	// Updates Mheel = Fheel*(ZCE + geom.T - geom.ZCBK);
	// todo dtrimarchi: verify the original comment
	// attenzione: il centro di spinta della deriva e' stato messo nel centro
	// di galleggiamento. l'ipotesi e' corretta?
	mHeel_ = fHeel_ * ( pSailSet_->get("ZCE") + pParser_->get("T") - pParser_->get("ZCBK") );
	if(isnan(mHeel_)){
		std::cout<<"mHeel_ is NAN!"<<std::endl;
		throw logic_error("mHeel_ is NAN!");
	}
	// Updates Fside_ = Fheel*cos(phi*pi/180). Note PHI_ is in degrees
	fSide_ = fHeel_ * cos( toRad(PHI_) );
	if(isnan(fSide_)){
		std::cout<<"fSide is NAN!"<<std::endl;
		throw logic_error("fSide is NAN!");
	}
}

/// Get the value of the side force
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

void AeroForcesItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of AeroForcesItem "<<std::endl;
}
