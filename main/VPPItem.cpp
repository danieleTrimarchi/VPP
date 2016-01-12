#include "VPPItem.h"

// Constructor
VPPItem::VPPItem(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet) :
pParser_(pParser),
pSailSet_(pSailSet),
V_(0),
PHI_(0), // Note that PHI is in deg
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
	PHI_= x[1]; // Note that PHI_ is in deg
	b_= x[2];
	f_= x[3];

	// Now call the implementation of the pure virtual update(int,int)
	// for every child
	update(vTW,aTW);

}

void VPPItem::printWhoAmI() {
	std::cout<<"--> WhoAmI of VPPItem "<<std::endl;
}

//=========================================================


