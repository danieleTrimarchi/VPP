#include "VPPItemFactory.h"
#include "VPPAeroItem.h"
#include "VPPHydroItem.h"

// Constructor
VPPItemFactory::VPPItemFactory(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet) {

	// -- INSTATIATE THE AERO ITEMS

	//Instantiate the wind and push it back to the children vector
	boost::shared_ptr<WindItem> pWind(new WindItem(pParser,pSailSet));
	vppItems_.push_back( pWind );

	// Instantiate the sail coefficients and push it back to the children vector
	boost::shared_ptr<SailCoefficientItem> pSailCoeffItem(new SailCoefficientItem(pWind.get()));
	vppItems_.push_back( pSailCoeffItem );

	// Instantiate the aero force Item and push it back to the children vector
	boost::shared_ptr<AeroForcesItem> pAeroForcesItem(new AeroForcesItem(pSailCoeffItem.get()));
	vppItems_.push_back( pAeroForcesItem );

	// -- INSTANTIATE THE 9 RESISTANCE ITEMS

	// Instantiate a InducedResistanceItem and push it back to the children vector
	boost::shared_ptr<InducedResistanceItem> pInducedResistance(new InducedResistanceItem(pAeroForcesItem.get()));
	vppItems_.push_back( pInducedResistance );

	// Instantiate a ResiduaryResistanceItem and push it back to the children vector
	boost::shared_ptr<ResiduaryResistanceItem> pResiduaryResistance(new ResiduaryResistanceItem(pParser,pSailSet));
	vppItems_.push_back( pResiduaryResistance );

	// Instantiate a Delta_ResiduaryResistance_HeelItem Item push it back to the children vector
	boost::shared_ptr<Delta_ResiduaryResistance_HeelItem> pDelta_ResiduaryResistance_HeelItem(new Delta_ResiduaryResistance_HeelItem(pParser,pSailSet));
	vppItems_.push_back( pDelta_ResiduaryResistance_HeelItem );

	// Instantiate a ResiduaryResistanceKeelItem Item and push it back to the children vector
	boost::shared_ptr<ResiduaryResistanceKeelItem> pResiduaryResistanceKeelItem(new ResiduaryResistanceKeelItem(pParser,pSailSet));
	vppItems_.push_back( pResiduaryResistanceKeelItem );

	// Instantiate a Delta_ResiduaryResistanceKeel_HeelItem Item and push it back to the children vector
	boost::shared_ptr<Delta_ResiduaryResistanceKeel_HeelItem> pDelta_ResiduaryResistanceKeel_HeelItem(new Delta_ResiduaryResistanceKeel_HeelItem(pParser,pSailSet));
	vppItems_.push_back( pDelta_ResiduaryResistanceKeel_HeelItem );

	// Instantiate a FrictionalResistanceItem Item and push it back to the children vector
	boost::shared_ptr<FrictionalResistanceItem> pFrictionalResistanceItem(new FrictionalResistanceItem(pParser,pSailSet));
	vppItems_.push_back( pFrictionalResistanceItem );

	// Instantiate a Delta_FrictionalResistance_HeelItem Item and push it back to the children vector
	boost::shared_ptr<Delta_FrictionalResistance_HeelItem> pDelta_FrictionalResistance_HeelItem(new Delta_FrictionalResistance_HeelItem(pParser,pSailSet));
	vppItems_.push_back( pDelta_FrictionalResistance_HeelItem );

	// Instantiate a ViscousResistanceKeelItem Item and push it back to the children vector
	boost::shared_ptr<ViscousResistanceKeelItem> pViscousResistanceKeelItem(new ViscousResistanceKeelItem(pParser,pSailSet));
	vppItems_.push_back( pViscousResistanceKeelItem );

	// Instantiate a ViscousResistanceKeelItem Item and push it back to the children vector
	boost::shared_ptr<ViscousResistanceRudderItem> pViscousResistanceRudderItem(new ViscousResistanceRudderItem(pParser,pSailSet));
	vppItems_.push_back( pViscousResistanceRudderItem );

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
