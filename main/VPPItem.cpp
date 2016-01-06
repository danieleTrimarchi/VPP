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
	vppItems_.push_back(boost::shared_ptr(new WindItem()) );
}

// Destructor
VPPItem::~VPPItem() {

}

// Returns the parser
VariableFileParser* VPPItem::getParser() {
	return pParser_;
}

// Returns the SailSet
boost::shared_ptr<SailSet> VPPItem::getSailSet() {
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


void VPPItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of VPPItem "<<std::endl;
}


//=========================================================

/// Constructor
WindItem::WindItem() :
		twv_(0),
		twa_(0),
		awa_(0),
		awv_(Eigen::Vector2d::Zero()),
		pi_(M_PI / 180.0) {

}

/// Destructor
WindItem::~WindItem() {

}

/// Update the items for the current step (wind velocity and angle)
void WindItem::update(int vTW, int aTW) {

	// Get a reference to the varSet
	const VarSet& v = *(pParser_->getVariables());

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


