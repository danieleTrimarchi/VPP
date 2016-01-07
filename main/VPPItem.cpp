#include "VPPItem.h"

// Constructor
VPPItem::VPPItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet) :
pParser_(pParser),
pSailSet_(pSailSet),
V_(0),
PHI_(0),
b_(0),
f_(0) {

	// Instantiate the wind
	boost::shared_ptr<WindItem> pWind(new WindItem(pParser_,pSailSet_));

	// Push it back to the children vector
	vppItems_.push_back( pWind );

	// Instantiate the sail coefficients
	boost::shared_ptr<SailCoefficientItem> pSailCoeffItem(new SailCoefficientItem(pWind.get()));

	// Push it back to the children vector
	vppItems_.push_back( pSailCoeffItem );

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

	// Loop on the children and call update
	for(size_t iChild=0; iChild<vppItems_.size(); iChild++)
		vppItems_[iChild]->update(vTW,aTW);

}

// Implement the pure virtual to allow instantiating this class
void VPPItem::update(int vTW, int aTW) {
	// make nothing
}

void VPPItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of VPPItem "<<std::endl;
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
				pWindItem_(pWindItem) {

	// Init static members with values from: Hazer Cl-Cd coefficients, 1999
	// Cols: rwa_, Main_Cl, Jib_Cl, Spi_Cl

	// Pick the lift coefficient for this main (full batten or not)
	if( pParser_->get("MFBL") ) {

		cl_.resize(6,4);
		cl_.row(0) << 0, 		0, 			0, 		0   ;
		cl_.row(1) << 27, 	1.725,	1.5,	0   ;
		cl_.row(2) << 50, 	1.5, 		0.5,	1.5 ;
		cl_.row(3) << 80,		0.95, 	0.3,	1.0 ;
		cl_.row(4) << 100,	0.85, 	0.0,	0.85;
		cl_.row(5) << 180,	0, 			0, 		0		;

	} else {

		cl_.resize(6,4);
		cl_.row(0) << 0, 		0, 	 	0 , 	0;
		cl_.row(1) << 27, 	1.5, 	1.5,	0;
		cl_.row(2) << 50, 	1.5, 	0.5,	1.5;
		cl_.row(3) << 80, 	0.95,	0.3,	1.0;
		cl_.row(4) << 100,	0.85,	0.0,	0.85;
		cl_.row(5) << 180,	0, 	 	0,	 	0;

	}

	// We only dispose of one drag coeff array for the moment
	cd_.resize(6,4);
	cl_.row(0) << 0,  	0,   	0,   	0;
	cl_.row(1) << 27, 	0.02,	0.02,	0;
	cl_.row(2) << 50, 	0.15,	0.25,	0.25;
	cl_.row(3) << 80, 	0.8, 	0.15,	0.9;
	cl_.row(4) << 100,	1.0, 	0.0, 	1.2;
	cl_.row(5) << 180,	0.9, 	0.0, 	0.66;

}

// Destructor
SailCoefficientItem::~SailCoefficientItem() {

}

void SailCoefficientItem::update(int vTW, int aTW) {

	/// Get the current value of the apparent wind angle
	double awa= pWindItem_->getAWA();

}

// Print the class name -> in this case SailCoefficientItem
void SailCoefficientItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of SailCoefficientItem "<<std::endl;
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

