#include "VPPItemFactory.h"
#include "VPPException.h"
#include <limits>

// Constructor
VPPItemFactory::VPPItemFactory(VariableFileParser* pParser, boost::shared_ptr<SailSet> pSailSet):
pParser_(pParser),
dF_(0),
dM_(0),
c1_(0),
c2_(0) {

	// -- INSTANTIATE THE AERO ITEMS

	// Instantiate the wind and push it back to the children vector
	boost::shared_ptr<WindItem> pWind(new WindItem(pParser_,pSailSet));
	vppAeroItems_.push_back( pWind );

	// Store a ptr to the wind
	pWind_= pWind;

	// Ask the sailSet to instantiate the relevant sail coefficients based
	// on the current sail configuration and push it back to the children vector
	pSailCoeffItem_.reset( pSailSet->sailCoefficientItemFactory(pWind.get()) );
	vppAeroItems_.push_back( pSailCoeffItem_ );

	// Instantiate the aero force Item and push it back to the children vector
	pAeroForcesItem_.reset(new AeroForcesItem(pSailCoeffItem_.get()));
	vppAeroItems_.push_back( pAeroForcesItem_ );

	// -- INSTANTIATE THE 10 RESISTANCE ITEMS

	// Instantiate a FrictionalResistanceItem Item and push it back to the children vector
	// For the definition of the Frictional Resistance see Keuning 2.1 p108
	pFrictionalResistanceItem_.reset(new FrictionalResistanceItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pFrictionalResistanceItem_ );

	// Instantiate a ResiduaryResistanceItem and push it back to the children vector
	// For the definition of the Residuary Resistance: see Keuning 3.1.1.2 p112
	pResiduaryResistanceItem_.reset(new ResiduaryResistanceItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pResiduaryResistanceItem_ );

	// Instantiate a Delta_FrictionalResistance_HeelItem Item and push it back to the children vector
	// For the definition of the Change in Frictional Resistance due to heel see Keuning ch3.1.2.1 p115-116
	pDelta_FrictionalResistance_HeelItem_.reset(new Delta_FrictionalResistance_HeelItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pDelta_FrictionalResistance_HeelItem_ );

	// Instantiate a Delta_ResiduaryResistance_HeelItem Item push it back to the children vector
	// For the definition of the change in Residuary Resistance due to heel
	// see DSYHS99 ch3.1.2.2 p116
	pDelta_ResiduaryResistance_HeelItem_.reset(new Delta_ResiduaryResistance_HeelItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pDelta_ResiduaryResistance_HeelItem_ );

	// Instantiate a ViscousResistanceKeelItem Item and push it back to the children vector
	// The viscous resistance of the Keel is defined in the std way, see DSYHS99 3.2.1.1 p 119
	pViscousResistanceKeelItem_.reset(new ViscousResistanceKeelItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pViscousResistanceKeelItem_ );

	// Instantiate a ViscousResistanceKeelItem Item and push it back to the children vector
	// The viscous resistance of the Rudder is defined in the std way, see DSYHS99 ch3.2.1.1 p 119
	pViscousResistanceRudderItem_.reset(new ViscousResistanceRudderItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pViscousResistanceRudderItem_ );

	// Instantiate a ResiduaryResistanceKeelItem Item and push it back to the children vector
	// For the definition of the Residuary Resistance of the Keel see
	// DSYHS99 3.2.1.2 p.120 and following
	pResiduaryResistanceKeelItem_.reset(new ResiduaryResistanceKeelItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pResiduaryResistanceKeelItem_ );

	// Instantiate a Delta_ResiduaryResistanceKeel_HeelItem Item and push it back to the children vector
	// Express the change in Appendage Resistance due to Heel. See DSYHS99 3.2.2 p 126-127
	boost::shared_ptr<Delta_ResiduaryResistanceKeel_HeelItem> pDelta_ResiduaryResistanceKeel_HeelItem(new Delta_ResiduaryResistanceKeel_HeelItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pDelta_ResiduaryResistanceKeel_HeelItem );

	// Instantiate a InducedResistanceItem and push it back to the children vector
	// For the definition of the Induced Resistance see DSYHS99 ch4 p128
	boost::shared_ptr<InducedResistanceItem> pInducedResistance(new InducedResistanceItem(pAeroForcesItem_.get()));
	vppHydroItems_.push_back( pInducedResistance );

	// Instantiate a NegativeResistanceItem and push it back to the children vector
	// This defines the resistance in the case of negative velocities
	pNegativeResistance_.reset(new NegativeResistanceItem(pParser_,pSailSet));
	vppHydroItems_.push_back( pNegativeResistance_ );

	// ----------

	/// Instantiate a righting moment item
	pRightingMomentItem_.reset(new RightingMomentItem(pParser_,pSailSet));

}

// Destructor
VPPItemFactory::~VPPItemFactory(){

}

// Update the VPPItems for the current step (wind velocity and angle),
// the value of the state vector x computed by the optimizer
// TODO dtrimarchi: definitely remove the old c-style signature
void VPPItemFactory::update(int vTW, int aTW, Eigen::VectorXd& x) {

	// Update all of the aero items:
	for(size_t iItem=0; iItem<vppAeroItems_.size(); iItem++)
		vppAeroItems_[iItem]->updateSolution(vTW,aTW,x);

	// Update all of the hydro items:
	for(size_t iItem=0; iItem<vppHydroItems_.size(); iItem++)
		vppHydroItems_[iItem]->VPPItem::updateSolution(vTW,aTW,x);

	// Update of the righting moment item:
	pRightingMomentItem_->VPPItem::updateSolution(vTW,aTW,x);

}

// Update the VPPItems for the current step (wind velocity and angle),
// the value of the state vector x computed by the optimizer
void VPPItemFactory::update(int vTW, int aTW, const double* x) {

	// Update all of the aero items:
	for(size_t iItem=0; iItem<vppAeroItems_.size(); iItem++)
		vppAeroItems_[iItem]->updateSolution(vTW,aTW,x);

	// Update all of the hydro items:
	for(size_t iItem=0; iItem<vppHydroItems_.size(); iItem++)
		vppHydroItems_[iItem]->VPPItem::updateSolution(vTW,aTW,x);

	// Update of the righting moment item:
	pRightingMomentItem_->VPPItem::updateSolution(vTW,aTW,x);

}

// Returns a ptr to the variableFileParser
VariableFileParser* VPPItemFactory::getParser() {
	return pParser_;
}

/// Returns a ptr to the wind item
WindItem* VPPItemFactory::getWind() const {
	return pWind_.get();
}

/// Getter for the sail coefficients container
const SailCoefficientItem* VPPItemFactory::getSailCoefficientItem() const {
	return pSailCoeffItem_.get();
}

// Getter for the aero forces item that stores the driving forces
const AeroForcesItem* VPPItemFactory::getAeroForcesItem() const {
	return pAeroForcesItem_.get();
}

// Getter for the aero forces item that stores the driving forces
AeroForcesItem* VPPItemFactory::getAeroForcesItem() {
	return pAeroForcesItem_.get();
}

// Getter for the Residuary Resistance item
ResiduaryResistanceItem* VPPItemFactory::getResiduaryResistanceItem() const {
	return pResiduaryResistanceItem_.get();
}

// Getter for the Residuary Resistance of the keel item
ResiduaryResistanceKeelItem* VPPItemFactory::getResiduaryResistanceKeelItem() const {
	return pResiduaryResistanceKeelItem_.get();
}

// Getter for the Delta Viscous Resistance of the keel item
ViscousResistanceKeelItem* VPPItemFactory::getViscousResistanceKeelItem() const {
	return pViscousResistanceKeelItem_.get();
}

// Getter for the Frictional Resistance item
FrictionalResistanceItem* VPPItemFactory::getFrictionalResistanceItem() const {
	return pFrictionalResistanceItem_.get();
}

// Getter for the Delta Viscous Resistance of the Rudder item
ViscousResistanceRudderItem* VPPItemFactory::getViscousResistanceRudderItem() const {
	return pViscousResistanceRudderItem_.get();
}

// Getter for the Residuary Resistance due to heel item
Delta_FrictionalResistance_HeelItem* VPPItemFactory::getDelta_FrictionalResistance_HeelItem() const {
	return pDelta_FrictionalResistance_HeelItem_.get();
}

// Getter for the Delta Residuary Resistance due to heel item
Delta_ResiduaryResistance_HeelItem* VPPItemFactory::getDelta_ResiduaryResistance_HeelItem() const {
	return pDelta_ResiduaryResistance_HeelItem_.get();
}

// Getter for the negative resistance item
NegativeResistanceItem* VPPItemFactory::getNegativeResistanceItem() const {
	return pNegativeResistance_.get();
}

// Getter for the righting moment item
RightingMomentItem* VPPItemFactory::getRightingMomentItem() const {
	return pRightingMomentItem_.get();
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

void VPPItemFactory::getResiduals(double& dF, double& dM) {

	// compute deltaF = (Fdrive - Rtot)
	dF_ = (pAeroForcesItem_->getFDrive() - getResistance());
	dF=dF_;

	// compute deltaM = (Mheel  - Mright)
	dM_ = (pAeroForcesItem_->getMHeel()  - pRightingMomentItem_->get());
	dM=dM_;

	//std::cout<<"dF= "<<dF<<"  dM= "<<dM<<std::endl;

}

Eigen::VectorXd VPPItemFactory::getResiduals(int vTW, int aTW, Eigen::VectorXd& x) {

	// update the items with the state vector
	update(vTW, aTW, x);

	// compute deltaF = (Fdrive + Rtot). Remember that FDrive is supposedly
	// positive, while the resistance is always negative
	dF_ = (pAeroForcesItem_->getFDrive() - getResistance());

	// compute deltaM = (Mheel + Mright). Remember that mHeel is Positive,
	// while righting moment is negative (right hand rule)
	dM_ = (pAeroForcesItem_->getMHeel() - pRightingMomentItem_->get());

	//std::cout<<"dF= "<<dF_<<"  dM= "<<dM_<<std::endl;

// TORESTORE
//	// Container for the residuals and their derivatives :
//	// dF  dF/dv  dF/dPhi  dF/db  dF/df
//	// dM  dM/dv  dM/dPhi  dM/db  dM/df
//	Eigen::Array2Xd rsd(2,5);
//
//	// Fill the first column of rsd
//	rsd(0,0)=dF_;
//	rsd(1,0)=dM_;
//
//	// Instantiate a buffer container for the state variables (limits cancellation)
//	// todo dtrimarchi: step1, copy with a proper C utility. step2, pass to vectors
//	Eigen::VectorXd xbuf(x.size());
//
//	// Compute the the derivatives for the additional (optimization) equations
//	for(size_t iVar=0; iVar<4; iVar++) {
//
//		// Init the buffer vector with the values of the state vector
//		xbuf=x;
//
//		// Compute the 'optimal' eps
//		double eps= x(iVar) * std::sqrt( std::numeric_limits<double>::epsilon() );
//
//		// Set var = var+eps:
//		xbuf(iVar) = x(iVar) + eps;
//
//		// update the items with the state vector
//		update(vTW, aTW, xbuf);
//
//		// Get the residuals
//		double dFp = (pAeroForcesItem_->getFDrive() - getResistance());
//		double dMp = (pAeroForcesItem_->getMHeel()  - pRightingMomentItem_->get());
//
//		// Set var = var-2eps:
//		xbuf(iVar) = x(iVar) - eps;
//
//		// update the items with the state vector
//		update(vTW, aTW, xbuf);
//
//		// Get the residuals
//		double dFm = (pAeroForcesItem_->getFDrive() - getResistance());
//		double dMm = (pAeroForcesItem_->getMHeel()  - pRightingMomentItem_->get());
//
//		// Compute dF/dv and dM/dv:
//		rsd(0,iVar+1) = ( dFp - dFm ) / (2*eps);
//		rsd(1,iVar+1) = ( dMp - dMm ) / (2*eps);
//
//	}
//
//	// update the items with the state initial state vector
//	update(vTW, aTW, x);
//
//	// Compute the value of c1 = (Fb MPhi-FPhi Mb)/(Fv MPhi-FPhi Mv)
//	c1_= 	( rsd(0,3) * rsd(1,2) - rsd(0,2) * rsd(1,3) ) /
//				( rsd(0,1) * rsd(1,2) - rsd(0,2) * rsd(1,0) );
//
//	// Compute the value of c2 = (Ff MPhi-FPhi Mv)/(Fv MPhi-FPhi Mv)
//	c2_= 	( rsd(0,4) * rsd(1,2) - rsd(0,2) * rsd(1,1) ) /
//				( rsd(0,1) * rsd(1,2) - rsd(0,2) * rsd(1,0) );

	// Returns the results in a reasonable Eigen-style shape
	return getResiduals();

}

// Get the current value for the optimizer constraint residuals dF=0 and dM=0
Eigen::VectorXd VPPItemFactory::getResiduals() {
// TORESTORE
//	Eigen::VectorXd ret(4);
//	ret << dF_,dM_,c1_,c2_;
		Eigen::VectorXd ret(2);
		ret << dF_,dM_;
	return ret;
}


