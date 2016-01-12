#include "VPPItemFactory.h"
#include "VPPAeroItem.h"

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

	// Instantiate the aero force Item
	boost::shared_ptr<AeroForcesItem> pAeroForcesItem(new AeroForcesItem(pSailCoeffItem.get()));

	// Push it back to the children vector
	vppItems_.push_back( pAeroForcesItem );

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
