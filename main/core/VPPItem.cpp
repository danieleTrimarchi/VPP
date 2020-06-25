#include "VPPItem.h"
#include "Physics.h"
#include "mathUtils.h"
#include "VPPException.h"

// Constructor
VPPItem::VPPItem(VariableFileParser* pParser, std::shared_ptr<SailSet> pSailSet) :
pParser_(pParser),
pSailSet_(pSailSet),
//V_(0),
//PHI_(0), // Note that PHI is in deg
//b_(0),
//f_(0),
pbSize_(4){
	x_ = Eigen::VectorXd::Zero(pbSize_);
}

// Destructor
VPPItem::~VPPItem() {

}

// Returns the parser
VariableFileParser* VPPItem::getParser() const {
	return pParser_;
}

// Returns the SailSet
std::shared_ptr<SailSet> VPPItem::getSailSet() const {
	return pSailSet_;
}

// Update the items for the current step (wind velocity and angle),
// the value of the state vector x computed by the optimizer
void VPPItem::updateSolution(int vTW, int aTW, const double* x) {

	if(mathUtils::isNotValid(x[0])) throw VPPException(HERE,"x[0] is NAN!");
	if(mathUtils::isNotValid(x[1])) throw VPPException(HERE,"x[1] is NAN!");
	if(mathUtils::isNotValid(x[2])) throw VPPException(HERE,"x[2] is NAN!");
	if(mathUtils::isNotValid(x[3])) throw VPPException(HERE,"x[3] is NAN!");

	// Update the local copy of the state variables
	x_(stateVars::u)=  x[0];
	x_(stateVars::phi)= 	x[1]; // Note that PHI_ is in rad
	x_(stateVars::b)=  x[2];
	x_(stateVars::f)=  x[3];

	// Now call the implementation of the pure virtual update(int,int)
	// for every child
	update(vTW,aTW);

}

// Update the parent VPPItem for the current step (wind velocity and angle),
// the value of the state vector x computed by the optimizer. Then, call the
// update method for the children in the vppItems_ vector
void VPPItem::updateSolution(int vTW, int aTW, Eigen::VectorXd& x) {

	for(size_t i=0; i<x.size(); i++)
		if(mathUtils::isNotValid(x(i))) {
			char msg[256];
			sprintf(msg,"x(%i) is NAN!",i);
			throw VPPException(HERE,msg);
		}

	// Update the local copy of the state variables
	x_=x;

	// Now call the implementation of the pure virtual update(int,int)
	// for every child
	update(vTW,aTW);

}

