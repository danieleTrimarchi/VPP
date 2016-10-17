#ifndef VPPITEMFACTORY_H
#define VPPITEMFACTORY_H

#include "VariableFileParser.h"
#include "SailSet.h"
#include "VPPAeroItem.h"
#include "VPPHydroItem.h"
#include "VPPRightingMomentItem.h"

/// Factory class used to instantiate and own all
/// of the VPPItems requested to compute the VPP run
class VPPItemFactory {

	public:

		/// Constructor
		VPPItemFactory(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~VPPItemFactory();

		/// Update the VPPItems for the current step (wind velocity and angle),
		/// the value of the state vector x computed by the optimizer
		/// TODO dtrimarchi: definitely remove the old c-style signature
		void update(int vTW, int aTW, Eigen::VectorXd& xv);

		/// Update the VPPItems for the current step (wind velocity and angle),
		/// the value of the state vector x computed by the optimizer
		/// TODO dtrimarchi: definitely remove this old c-style signature
		void update(int vTW, int aTW, const double* x);

		/// Returns a ptr to the variableFileParser
		VariableFileParser* getParser();

		/// Returns a ptr to the wind item
		WindItem* getWind() const;

		/// Getter for the sail coefficients container
		const SailCoefficientItem* getSailCoefficientItem() const;

		/// Getter for the sail coefficients container - non const version
		SailCoefficientItem* getSailCoefficientItem();

		/// Getter for the aero forces item that stores the driving forces
		const AeroForcesItem* getAeroForcesItem() const;

		/// Getter for the aero forces item that stores the driving forces
		/// Non const version
		AeroForcesItem* getAeroForcesItem();

		/// Getter for the Residuary Resistance item
		ResiduaryResistanceItem* getResiduaryResistanceItem() const;

		/// Getter for the Induced Resistance item
		InducedResistanceItem* getInducedResistanceItem() const;

		/// Getter for the Residuary Resistance of the Keel item
		ResiduaryResistanceKeelItem* getResiduaryResistanceKeelItem() const;

		/// Getter for the Frictional Resistance item
		FrictionalResistanceItem* getFrictionalResistanceItem() const;

		/// Getter for the Delta Viscous Resistance of the keel item
		ViscousResistanceKeelItem* getViscousResistanceKeelItem() const;

		/// Getter for the Delta Viscous Resistance of the Rudder item
		ViscousResistanceRudderItem* getViscousResistanceRudderItem() const;

		/// Getter for the Delta Frictional Resistance due to heel item
		Delta_FrictionalResistance_HeelItem* getDelta_FrictionalResistance_HeelItem() const;

		/// Getter for the Delta Residuary Resistance due to heel item
		Delta_ResiduaryResistance_HeelItem* getDelta_ResiduaryResistance_HeelItem() const;

		/// Getter for the negative resistance item
		NegativeResistanceItem* getNegativeResistanceItem() const;

		/// Getter for the righting moment item
		RightingMomentItem* getRightingMomentItem() const;

		/// Compute the resistance by summing up all the contributions
		double getResistance();

		/// Compute the force/moment residuals for the optimizer constraints dF=0 and dM=0
		void getResiduals(double& dF, double& dM);

		/// Compute the force/moment residuals and also the residuals of the additional
		/// equations c1=0 and c2=0. Do not require updates to be operated previously
		Eigen::VectorXd getResiduals(int vTW, int aTW, VectorXd& x);

		/// Get the current value for the optimizer constraint residuals dF=0 and dM=0
		/// and for c1 and c2
		Eigen::VectorXd getResiduals();

		/// Plot the total resistance over a fixed range Fn=0-1
		void plotTotalResistance();

		/// Make a 3d plot of the optimization variables v, phi when varying the two opt
		/// parameters flat and crew
		void plotOptimizationSpace();

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	private:

		/// Ptr to the VariableFileParser
		VariableFileParser* pParser_;

		/// Ptr to the SailCoefficientItem
		boost::shared_ptr<SailCoefficientItem> pSailCoeffItem_;

		/// Vector storing all the Children of VPPItem
		std::vector<boost::shared_ptr<VPPItem> > vppAeroItems_;

		/// Pointer to the resistance item
		std::vector<boost::shared_ptr<ResistanceItem> > vppHydroItems_;

		/// Pointer to the Wind Item, owned by pAeroForcesItem_
		boost::shared_ptr<WindItem> pWind_;

		/// Pointer to the Aero forces Item
		boost::shared_ptr<AeroForcesItem> pAeroForcesItem_;

		/// Pointer to the Residuary Resistance Item
		boost::shared_ptr<ResiduaryResistanceItem> pResiduaryResistanceItem_;

		/// Pointer to the Residuary Resistance of the Keel Item
		boost::shared_ptr<ResiduaryResistanceKeelItem> pResiduaryResistanceKeelItem_;

		/// Pointer to the Induced Resistance Item
		boost::shared_ptr<InducedResistanceItem> pInducedResistanceItem_;

		/// Pointer to the Frictional Resistance Item
		boost::shared_ptr<FrictionalResistanceItem> pFrictionalResistanceItem_;

		/// Pointer to the Viscous Resistance of the keel Item
		boost::shared_ptr<ViscousResistanceKeelItem> pViscousResistanceKeelItem_;

		/// Pointer to the Viscous Resistance of the rudder Item
		boost::shared_ptr<ViscousResistanceRudderItem> pViscousResistanceRudderItem_;

		/// Pointer to the Frictional Resistance due to Heel Item
		boost::shared_ptr<Delta_FrictionalResistance_HeelItem> pDelta_FrictionalResistance_HeelItem_;

		/// Pointer to the Residuary Resistance due to Heel Item
		boost::shared_ptr<Delta_ResiduaryResistance_HeelItem> pDelta_ResiduaryResistance_HeelItem_;

		/// Pointer to the Negative Resistance
		boost::shared_ptr<NegativeResistanceItem> pNegativeResistance_;

		/// Pointer to the Righting moment Item
		boost::shared_ptr<RightingMomentItem> pRightingMomentItem_;

		/// Residuals on the residuals : dF, dM, c1 and c2
		double dF_, dM_;

};

#endif
