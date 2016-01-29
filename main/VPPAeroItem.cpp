#include "VPPAeroItem.h"

#include "VPPException.h"

#include "mathUtils.h"
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

	// Get the max/min wind angles from the parser
	alpha_tw_min_= pParser_->get("ALPHA_TW_MIN");
	alpha_tw_max_= pParser_->get("ALPHA_TW_MAX");
	n_alpha_tw_= pParser_->get("N_ALPHA_TW");

}

/// Destructor
WindItem::~WindItem() {

}

/// Update the items for the current step (wind velocity and angle)
void WindItem::update(int vTW, int aTW) {

	// todo dtrimarchi : it this method is called n times by the optimizer, so it is
	// stupid to re-compute twv_ e twa_ at each time! But it is the only place where I
	// can update.

	// Update the true wind velocity
	// vmin to vmax in N steps : vMin + vTW * ( (vMax-vMin)/(nSteps-2) - 1 )
	twv_= v_tw_min_ + vTW * ( ( v_tw_max_ - v_tw_min_ ) / n_twv_ );
	if(isnan(twv_)) throw VPPException(HERE,"twv_ is NAN!");
	std::cout<<"twv_= "<<twv_<<std::endl;

	// Update the true wind angle: make as per the velocity
	twa_= alpha_tw_min_ + aTW * ( ( alpha_tw_max_ - alpha_tw_min_ ) /  n_alpha_tw_ );
	if(isnan(twa_)) throw VPPException(HERE,"twa_ is NAN!");
	std::cout<<"twa_= "<<twa_<<std::endl;

	std::cout<<"V_= "<<V_<<std::endl;
	std::cout<<"PHI_= "<<PHI_<<std::endl;

	// Update the apparent wind velocity vector
	awv_(0)= V_ + twv_ * cos( toRad(twa_)  );
	if(isnan(awv_(0))) throw VPPException(HERE,"awv_(0) is NAN!");
	std::cout<<"awv_(0)= "<<awv_(0)<<std::endl;

	awv_(1)= twv_ * sin( toRad(twa_) );
	if(isnan(awv_(1))) throw VPPException(HERE,"awv_(1) is NAN!");
	std::cout<<"awv_(1)= "<<awv_(1)<<std::endl;

	// Update the apparent wind angle - todo dtrimarchi: why do I need to
	// explicitly cast to a double for the indexer to resolve..?
	awa_= toDeg( atan( awv_(1)/awv_(0) ) );
	if(isnan(awa_))	throw VPPException(HERE,"awa_ is NAN!");
	std::cout<<"awa_= "<<awa_<<std::endl;
	std::cout<<"--------------------\n"<<std::endl;

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

			interpClVec_[0] -> plot(0,180,50,"Interpolated CL for MAIN");
			interpCdVec_[0] -> plot(0,180,50,"Interpolated CD for MAIN");
			interpClVec_[1] -> plot(0,180,50,"Interpolated CL for JIB");
			interpCdVec_[1] -> plot(0,180,50,"Interpolated CD for JIB");
			interpClVec_[2] -> plot(0,180,50,"Interpolated CL for SPI");
			interpCdVec_[2] -> plot(0,180,50,"Interpolated CD for SPI");

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
	if(isnan(awa_)) throw VPPException(HERE,"awa_ is NaN");

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

	// Compute the coefficients based on the sail configuration
	compute();

}

// Called by the children as a decorator for update.
// Do all is required after cl and cdp have been computed
void SailCoefficientItem::postUpdate() {

	// Reduce cl with the flattening factor of the state vector
	cl_ = cl_ * f_;
	if(isnan(cl_)) throw VPPException(HERE,"cl_ is nan");

	// Compute the induced resistance
	cdI_ = cl_ * cl_ * ( 1. / (M_PI * ar_) + 0.005 );

	// Compute the total sail drag coefficient now
	cd_ = cdp_ + cd0_ + cdI_;
	if(isnan(cd_)) throw VPPException(HERE,"cd_ is nan");

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
	cl_ = ( allCl_(0) * ps->get("AM") + allCl_(1) *  ps->get("AJ") ) /  ps->get("AN");
	cdp_ = ( allCd_(0) * ps->get("AM") + allCd_(1) *  ps->get("AJ") ) /  ps->get("AN");

    std::cout<< "ps->get(AM)= "<<ps->get("AM")<<std::endl;
    std::cout<< "ps->get(AJ)= "<<ps->get("AJ")<<std::endl;
    std::cout<< "ps->get(AN)= "<<ps->get("AN")<<std::endl;

	if(isnan(cl_)) throw VPPException(HERE,"cl_ is nan");
	if(isnan(cdp_)) throw VPPException(HERE,"cdp_ is nan");

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
	cl_ = ( allCl_(0) * ps->get("AM") + allCl_(2) *  ps->get("AS") ) /  ps->get("AN");
	cdp_ = ( allCd_(0) * ps->get("AM") + allCd_(2) *  ps->get("AS") ) /  ps->get("AN");
	if(isnan(cl_)) throw VPPException(HERE,"cl_ is nan");
	if(isnan(cdp_)) throw VPPException(HERE,"cdp_ is nan");

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
	cl_ = ( allCl_(0) * ps->get("AM") + allCl_(1) *  ps->get("AJ") + allCl_(2) *  ps->get("AS") ) /  ps->get("AN");
	cdp_ = ( allCd_(0) * ps->get("AM") + allCd_(1) *  ps->get("AJ") + allCd_(2) *  ps->get("AS") ) /  ps->get("AN");
	if(isnan(cl_)) throw VPPException(HERE,"cl_ is nan");
	if(isnan(cdp_)) throw VPPException(HERE,"cdp_ is nan");

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
	if(isnan(awv)) throw VPPException(HERE,"awv is NAN!");

	double awa = pWindItem_->getAWA();
	if(isnan(awa)) throw VPPException(HERE,"awa is NAN!");


	// Updates Lift = 0.5 * phys.rho_a * V_eff.^2 .* AN .* Cl;
	lift_ = 0.5 * Physic::rho_a * awv * awv * pSailSet_->get("AN") * pSailCoeffs_->getCl();
	if(isnan(lift_)) throw VPPException(HERE,"lift_ is NAN!");


	// Updates Drag = 0.5 * phys.rho_a * V_eff.^2 .* AN .* Cd;
	drag_ = 0.5 * rho_a * awv * awv * pSailSet_->get("AN") * pSailCoeffs_->getCd();
	if(isnan(drag_)) throw VPPException(HERE,"drag_ is NAN!");


	// Updates Fdrive = lift_ * sin(alfa_eff) - D * cos(alfa_eff);
	fDrive_ = lift_ * sin(awa) - drag_ * cos(awa);
	if(isnan(fDrive_)) throw VPPException(HERE,"fDrive_ is NAN!");


	// Updates Fheel = L * cos(alfa_eff) + D * sin(alfa_eff);
	fHeel_ = lift_ * cos(awa) + drag_ * sin(awa);
	if(isnan(fHeel_)) throw VPPException(HERE,"fHeel_ is NAN!");


	// Updates Mheel = Fheel*(ZCE + geom.T - geom.ZCBK);
	// todo dtrimarchi: verify the original comment
	// attenzione: il centro di spinta della deriva e' stato messo nel centro
	// di galleggiamento. l'ipotesi e' corretta?
	mHeel_ = fHeel_ * ( pSailSet_->get("ZCE") + pParser_->get("T") - pParser_->get("ZCBK") );
	if(isnan(mHeel_)) throw VPPException(HERE,"mHeel_ is NAN!");

	// Updates Fside_ = Fheel*cos(phi*pi/180). Note PHI_ is in degrees
	fSide_ = fHeel_ * cos( toRad(PHI_) );
	if(isnan(fSide_)) throw VPPException(HERE,"fSide is NAN!");

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
