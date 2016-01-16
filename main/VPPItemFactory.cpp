#include "VPPItemFactory.h"

// Constructor
VPPItemFactory::VPPItemFactory(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet) {

	// -- INSTATIATE THE AERO ITEMS

	//Instantiate the wind and push it back to the children vector
	boost::shared_ptr<WindItem> pWind(new WindItem(pParser,pSailSet));
	vppAeroItems_.push_back( pWind );

	// Instantiate the sail coefficients and push it back to the children vector
	boost::shared_ptr<SailCoefficientItem> pSailCoeffItem(new SailCoefficientItem(pWind.get()));
	vppAeroItems_.push_back( pSailCoeffItem );

	// Instantiate the aero force Item and push it back to the children vector
	pAeroForcesItem_.reset(new AeroForcesItem(pSailCoeffItem.get()));
	vppAeroItems_.push_back( pAeroForcesItem_ );

	// -- INSTANTIATE THE 9 RESISTANCE ITEMS

	// Instantiate a InducedResistanceItem and push it back to the children vector
	boost::shared_ptr<InducedResistanceItem> pInducedResistance(new InducedResistanceItem(pAeroForcesItem_.get()));
	vppHydroItems_.push_back( pInducedResistance );

	// Instantiate a ResiduaryResistanceItem and push it back to the children vector
	boost::shared_ptr<ResiduaryResistanceItem> pResiduaryResistance(new ResiduaryResistanceItem(pParser,pSailSet));
	vppHydroItems_.push_back( pResiduaryResistance );

	// Instantiate a Delta_ResiduaryResistance_HeelItem Item push it back to the children vector
	boost::shared_ptr<Delta_ResiduaryResistance_HeelItem> pDelta_ResiduaryResistance_HeelItem(new Delta_ResiduaryResistance_HeelItem(pParser,pSailSet));
	vppHydroItems_.push_back( pDelta_ResiduaryResistance_HeelItem );

	// Instantiate a ResiduaryResistanceKeelItem Item and push it back to the children vector
	boost::shared_ptr<ResiduaryResistanceKeelItem> pResiduaryResistanceKeelItem(new ResiduaryResistanceKeelItem(pParser,pSailSet));
	vppHydroItems_.push_back( pResiduaryResistanceKeelItem );

	// Instantiate a Delta_ResiduaryResistanceKeel_HeelItem Item and push it back to the children vector
	boost::shared_ptr<Delta_ResiduaryResistanceKeel_HeelItem> pDelta_ResiduaryResistanceKeel_HeelItem(new Delta_ResiduaryResistanceKeel_HeelItem(pParser,pSailSet));
	vppHydroItems_.push_back( pDelta_ResiduaryResistanceKeel_HeelItem );

	// Instantiate a FrictionalResistanceItem Item and push it back to the children vector
	boost::shared_ptr<FrictionalResistanceItem> pFrictionalResistanceItem(new FrictionalResistanceItem(pParser,pSailSet));
	vppHydroItems_.push_back( pFrictionalResistanceItem );

	// Instantiate a Delta_FrictionalResistance_HeelItem Item and push it back to the children vector
	boost::shared_ptr<Delta_FrictionalResistance_HeelItem> pDelta_FrictionalResistance_HeelItem(new Delta_FrictionalResistance_HeelItem(pParser,pSailSet));
	vppHydroItems_.push_back( pDelta_FrictionalResistance_HeelItem );

	// Instantiate a ViscousResistanceKeelItem Item and push it back to the children vector
	boost::shared_ptr<ViscousResistanceKeelItem> pViscousResistanceKeelItem(new ViscousResistanceKeelItem(pParser,pSailSet));
	vppHydroItems_.push_back( pViscousResistanceKeelItem );

	// Instantiate a ViscousResistanceKeelItem Item and push it back to the children vector
	boost::shared_ptr<ViscousResistanceRudderItem> pViscousResistanceRudderItem(new ViscousResistanceRudderItem(pParser,pSailSet));
	vppHydroItems_.push_back( pViscousResistanceRudderItem );

	/// Instantiate a righting moment item
	pRightingMomentItem_.reset(new RightingMomentItem(pParser,pSailSet));

}

// Destructor
VPPItemFactory::~VPPItemFactory(){

}

// Update the VPPItems for the current step (wind velocity and angle),
// the value of the state vector x computed by the optimizer
void VPPItemFactory::update(int vTW, int aTW, const double* x) {

	// Update all of the aero items:
	for(size_t iItem=0; iItem<vppAeroItems_.size(); iItem++)
		vppAeroItems_[iItem]->update(vTW,aTW,x);

	// Update all of the hydro items:
	for(size_t iItem=0; iItem<vppHydroItems_.size(); iItem++)
		vppHydroItems_[iItem]->VPPItem::update(vTW,aTW,x);

	// Update of the righting moment item:
	pRightingMomentItem_->VPPItem::update(vTW,aTW,x);

}

// Getter for the aero forces item that stores the driving forces
boost::shared_ptr<AeroForcesItem> VPPItemFactory::getAeroForcesItem() {
	return pAeroForcesItem_;
}

// Compute the resistance by summing up all the contributions
double VPPItemFactory::getResistance() {

	double resistance = 0;

	// Loop on the hydro items and sum the contributions
	for(size_t iItem=0; iItem<vppHydroItems_.size(); iItem++)
		resistance += vppHydroItems_[iItem]->get();

	return resistance;
}

void VPPItemFactory::computeResiduals() {

	// compute deltaF = (Fdrive - Rtot)
	double dF = (pAeroForcesItem_->getFDrive() - getResistance());

	// compute deltaM = (Mheel  - Mright)
	double deltaM = (pAeroForcesItem_->getMHeel()  - pRightingMomentItem_->get());

}



