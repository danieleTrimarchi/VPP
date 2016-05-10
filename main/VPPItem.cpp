#include "VPPItem.h"
#include "Physics.h"
#include "mathUtils.h"
#include "VPPException.h"

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
void VPPItem::updateSolution(int vTW, int aTW, const double* x) {

	if(mathUtils::isNotValid(x[0])) throw VPPException(HERE,"x[0] is NAN!");
	if(mathUtils::isNotValid(x[1])) throw VPPException(HERE,"x[1] is NAN!");
// TORESTORE
	//	if(mathUtils::isValid(x[2])) throw VPPException(HERE,"x[2] is NAN!");
//	if(mathUtils::isValid(x[3])) throw VPPException(HERE,"x[3] is NAN!");

	// Update the local copy of the state variables
	V_= x[0];
	PHI_= x[1]; // Note that PHI_ is in deg
// TORESTORE
	//	b_= x[2];
//	f_= x[3];


	// Now call the implementation of the pure virtual update(int,int)
	// for every child
	update(vTW,aTW);

}

// Update the parent VPPItem for the current step (wind velocity and angle),
// the value of the state vector x computed by the optimizer. Then, call the
// update method for the children in the vppItems_ vector
void VPPItem::updateSolution(int vTW, int aTW, Eigen::VectorXd& x) {

	if(mathUtils::isNotValid(x(0))) throw VPPException(HERE,"x(0) is NAN!");
	if(mathUtils::isNotValid(x(1))) throw VPPException(HERE,"x(1) is NAN!");
// TORESTORE
	//	if(mathUtils::isValid(x(2))) throw VPPException(HERE,"x(2) is NAN!");
//	if(mathUtils::isValid(x(3))) throw VPPException(HERE,"x(3) is NAN!");

	// Update the local copy of the state variables
	V_= x(0);
	PHI_= x(1); // Note that PHI_ is in deg
// TROESTORE
//	b_= x(2);
//	f_= x(3);


	// Now call the implementation of the pure virtual update(int,int)
	// for every child
	update(vTW,aTW);

}

