#include "VPPItemFactory.h"
#include "VPPException.h"
#include "mathUtils.h"
#include <limits>
#include "IOUtils.h"
#include "NRSolver.h"

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
	// => THIS IS THE GUY !
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
	pInducedResistanceItem_.reset(new InducedResistanceItem(pAeroForcesItem_.get()));
	vppHydroItems_.push_back( pInducedResistanceItem_ );

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

// Getter for the Residuary Resistance item
InducedResistanceItem* VPPItemFactory::getInducedResistanceItem() const {
	return pInducedResistanceItem_.get();
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

	if(mathUtils::isNotValid(resistance))
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

	// Update the items with the state vector
	update(vTW, aTW, x);

	// Compute deltaF = (Fdrive + Rtot). Remember that FDrive is supposedly
	// positive, while the resistance is always negative
	dF_ = (pAeroForcesItem_->getFDrive() - getResistance());

	// Compute deltaM = (Mheel + Mright). Remember that mHeel is Positive,
	// while righting moment is negative (right hand rule)
	dM_ = (pAeroForcesItem_->getMHeel() - pRightingMomentItem_->get());

	// Returns the results in a reasonable Eigen-style shape
	return getResiduals();

}

// Get the current value for the optimizer constraint residuals dF=0 and dM=0
Eigen::VectorXd VPPItemFactory::getResiduals() {

	Eigen::VectorXd ret(2);
	ret << dF_,dM_;
	return ret;

}

// Plot the total resistance over a fixed range Fn=0-1
void VPPItemFactory::plotTotalResistance(){

	// Ask the user for twv and twa
	size_t twv=0, twa=0;
	IOUtils io(pWind_.get());
	io.askUserWindIndexes(twv, twa);

	// Init the state vector
	Eigen::VectorXd stateVector;
	io.askUserStateVector(stateVector);

	// Ask a scaling factor (1 does nothing) to viz closer to the origin
	double plotScaling= io.askUserDouble("please enter a plot scaling factor. default is 1");

	// Define the number of velocities and angles
	// ( the angles are incremented of 10!)
	size_t nVelocities=40, nAngles=80;

	// Instantiate containers for the curve labels, the
	// Fn and the resistance
	std::vector<string> curveLabels;
	std::vector<ArrayXd> froudeNb, totRes;

	// Loop on the heel angles
	for(int iAngle=0; iAngle<nAngles+1; iAngle+=20){

		// Compute the value of PHI
		stateVector(1) = mathUtils::toRad(iAngle-20);
		//std::cout<<"PHI= "<<stateVector(1)<<std::endl;

		// declare some tmp containers
		vector<double> fn, res;

		// Loop on the velocities
		for(size_t v=0; v<nVelocities; v++){

			// Set a fictitious velocity (Fn=-0.3-0.7)
			stateVector(0)= plotScaling * ( -0.1 + ( 1./nVelocities * v ) ) * sqrt(Physic::g * pParser_->get("LWL"));

			// Update all the Items - not just the hydro as indRes requires up-to-date fHeel!
			update(twv,twa,stateVector);

			// Fill the vectors to be plot
			fn.push_back( stateVector(0)/sqrt(Physic::g * pParser_->get("LWL") ) );
			res.push_back( getResistance() );

		}

		// Now transform fn and res to ArrayXd and push_back to vector
		ArrayXd tmpFn(fn.size());
		ArrayXd tmpRes(fn.size());
		for(size_t j=0; j<fn.size(); j++){
			tmpFn(j)=fn[j];
			tmpRes(j)=res[j];
		}

		froudeNb.push_back(tmpFn);
		totRes.push_back(tmpRes);

		char msg[256];
		sprintf(msg,"%3.1fº", mathUtils::toDeg(stateVector(1)));
		curveLabels.push_back(msg);

	}

	// Instantiate a plotter and plot
	VPPPlotter fPlotter;
	for(size_t i=0; i<froudeNb.size(); i++)
		fPlotter.append(curveLabels[i],froudeNb[i],totRes[i]);

	char msg[256];
	sprintf(msg,"plot TOTAL Resistance vs boat speed - "
			"twv=%2.2f [m/s], twa=%2.2fº",
			pAeroForcesItem_->getWindItem()->getTWV(twv),
			mathUtils::toDeg(pAeroForcesItem_->getWindItem()->getTWA(twa)) );
	fPlotter.plot("Fn [-]","Total Resistance [N]", msg);

}

// Make a 3d plot of the optimization variables v, phi when varying the two opt
// parameters flat and crew
void VPPItemFactory::plotOptimizationSpace() {

	// Instantiate a IOUtil
	IOUtils io(pWind_.get());

	// Instantiate and ask for twv, twa, and the state vector
	size_t twv, twa;
	Eigen::VectorXd x;
	io.askUserWindIndexes(twv, twa);
	io.askUserStateVector(x);

	// Instantiate a NRSolver
	NRSolver nrSolver(this, 4, 2);

	// Set the number of values for flat and crew -> x, y
	size_t nFlat=15, nCrew=15;

	// Instantiate the result matrices : v and phi
	Eigen::ArrayXd flat(nFlat), crew(nCrew);
	Eigen::MatrixXd u(nCrew,nFlat);
	Eigen::MatrixXd phi(nCrew,nFlat);

	// Get the bounds for crew and flat
	double dCrew= ( pParser_->get("B_MAX")-pParser_->get("B_MIN") ) / (nCrew-1);
	double dFlat= ( pParser_->get("F_MAX")-pParser_->get("F_MIN") ) / (nFlat-1);

	// Loop on nFlat
	for(size_t iFlat=0; iFlat<nFlat; iFlat++){

		// set this flat
		flat(iFlat)= pParser_->get("F_MIN")  + dFlat * iFlat;

		//	loop on nCrew
		for(size_t iCrew=0; iCrew<nCrew; iCrew++){

			// set this crew
			crew(iCrew)= pParser_->get("B_MIN")  + dCrew * iCrew;

			//			set flat, crew in the state vector
			x(2) = crew(iCrew);
			x(3) = flat(iFlat);

			// 			run NRSolver -> v, phi
			x.block(0,0,2,1)= nrSolver.run(twv, twa,x).block(0,0,2,1);

			//			store v, phi in MatrixXds
			u(iCrew,iFlat) = x(0);
			phi(iCrew,iFlat) = x(1);

		}
	}

	VPPMagnitudeColoredCountourPlotter3d(crew, flat, u, "velocity opt", "crew", "flat" );
	VPPMagnitudeColoredCountourPlotter3d(crew, flat, phi, "phi opt", "crew", "flat" );

}

