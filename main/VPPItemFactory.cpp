#include "VPPItemFactory.h"
#include "VPPException.h"

// Constructor
VPPItemFactory::VPPItemFactory(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet):
pParser_(pParser),
dF_(0),
dM_(0) {

	// -- INSTANTIATE THE AERO ITEMS

	// Instantiate the wind and push it back to the children vector
	boost::shared_ptr<WindItem> pWind(new WindItem(pParser_,pSailSet));
	vppAeroItems_.push_back( pWind );

	// Store a ptr to the wind
	pWind_= pWind;

	// Ask the sailSet to instantiate the relevant sail coefficients based
	// on the current sail configuration and push it back to the children vector
	boost::shared_ptr<SailCoefficientItem> pSailCoeffItem( pSailSet->sailCoefficientItemFactory(pWind.get()) );
	vppAeroItems_.push_back( pSailCoeffItem );

	// Instantiate the aero force Item and push it back to the children vector
	pAeroForcesItem_.reset(new AeroForcesItem(pSailCoeffItem.get()));
	vppAeroItems_.push_back( pAeroForcesItem_ );

	// -- INSTANTIATE THE 9 RESISTANCE ITEMS

	// Instantiate a ResiduaryResistanceItem and push it back to the children vector
	boost::shared_ptr<ResiduaryResistanceItem> pResiduaryResistance(new ResiduaryResistanceItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pResiduaryResistance );

	// Instantiate a FrictionalResistanceItem Item and push it back to the children vector
	boost::shared_ptr<FrictionalResistanceItem> pFrictionalResistanceItem(new FrictionalResistanceItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pFrictionalResistanceItem );

	// Instantiate a ViscousResistanceKeelItem Item and push it back to the children vector
	boost::shared_ptr<ViscousResistanceKeelItem> pViscousResistanceKeelItem(new ViscousResistanceKeelItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pViscousResistanceKeelItem );

	// Instantiate a ViscousResistanceKeelItem Item and push it back to the children vector
	boost::shared_ptr<ViscousResistanceRudderItem> pViscousResistanceRudderItem(new ViscousResistanceRudderItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pViscousResistanceRudderItem );

	// Instantiate a Delta_FrictionalResistance_HeelItem Item and push it back to the children vector
	boost::shared_ptr<Delta_FrictionalResistance_HeelItem> pDelta_FrictionalResistance_HeelItem(new Delta_FrictionalResistance_HeelItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pDelta_FrictionalResistance_HeelItem );

	// Instantiate a InducedResistanceItem and push it back to the children vector
	boost::shared_ptr<InducedResistanceItem> pInducedResistance(new InducedResistanceItem(pAeroForcesItem_.get()));
	vppHydroItems_.push_back( pInducedResistance );

	// Instantiate a Delta_ResiduaryResistance_HeelItem Item push it back to the children vector
	boost::shared_ptr<Delta_ResiduaryResistance_HeelItem> pDelta_ResiduaryResistance_HeelItem(new Delta_ResiduaryResistance_HeelItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pDelta_ResiduaryResistance_HeelItem );

	// Instantiate a ResiduaryResistanceKeelItem Item and push it back to the children vector
	boost::shared_ptr<ResiduaryResistanceKeelItem> pResiduaryResistanceKeelItem(new ResiduaryResistanceKeelItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pResiduaryResistanceKeelItem );

	// Instantiate a Delta_ResiduaryResistanceKeel_HeelItem Item and push it back to the children vector
	boost::shared_ptr<Delta_ResiduaryResistanceKeel_HeelItem> pDelta_ResiduaryResistanceKeel_HeelItem(new Delta_ResiduaryResistanceKeel_HeelItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pDelta_ResiduaryResistanceKeel_HeelItem );

	// ----------

	/// Instantiate a righting moment item
	pRightingMomentItem_.reset(new RightingMomentItem(pParser_,pSailSet));

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

// Returns a ptr to the variableFileParser
VariableFileParser* VPPItemFactory::getParser() {
	return pParser_;
}

/// Returns a ptr to the wind item
WindItem* VPPItemFactory::getWind() const {
	return pWind_.get();
}

// Getter for the aero forces item that stores the driving forces
const AeroForcesItem* VPPItemFactory::getAeroForcesItem() const {
	return pAeroForcesItem_.get();
}

// Compute the resistance by summing up all the contributions
double VPPItemFactory::getResistance() {

	double resistance = 0;

	// Loop on the hydro items and sum the contributions
	for(size_t iItem=0; iItem<vppHydroItems_.size(); iItem++)
		resistance += vppHydroItems_[iItem]->get();

	if(isnan(resistance))
		throw VPPException(HERE,"Resistance is NAN");

	return resistance;
}

void VPPItemFactory::computeResiduals(double& dF, double& dM) {

	// compute deltaF = (Fdrive - Rtot)
	dF_ = (pAeroForcesItem_->getFDrive() - getResistance());
	dF=dF_;

	// compute deltaM = (Mheel  - Mright)
	dM_ = (pAeroForcesItem_->getMHeel()  - pRightingMomentItem_->get());
	dM=dM_;

	//std::cout<<"dF= "<<dF<<"  dM= "<<dM<<std::endl;

}

// Get the current value for the optimizer constraint residuals dF=0 and dM=0
void VPPItemFactory::getResiduals(double& dF, double& dM) {

	dF=dF_;
	dM=dM_;

}



