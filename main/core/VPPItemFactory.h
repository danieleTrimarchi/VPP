#ifndef VPPITEMFACTORY_H
#define VPPITEMFACTORY_H

#include "VariableFileParser.h"
#include "SailSet.h"
#include "VPPAeroItem.h"
#include "VPPHydroItem.h"
#include "VPPRightingMomentItem.h"
#include "VPPDialogs.h"
#include <QtDataVisualization/QSurfaceDataProxy>
using namespace QtDataVisualization;

/// Utility class used to store all is required to
/// finalize a 3d plot: QSurfaceDataArray*, ranges
/// Use here the same convention as per the
/// QSurfaceDataArray : independent vars are x,z,
/// while y is the dependent one.
class ThreeDDataContainer {

	public:

		/// Explicit Ctor
		explicit ThreeDDataContainer(QSurfaceDataArray*);

		/// Get the underlying QSurfaceDataArray*
		QSurfaceDataArray* get() const;

		/// Get the xRange
		Eigen::Array2d getXRange() const;

		/// Get the yRange
		Eigen::Array2d getYRange() const;

		/// Get the zRange
		Eigen::Array2d getZRange() const;

		/// Set the x step
		double getDx() const;

		/// Set the z step
		double getDz() const;

		/// Set the xRange
		void setXrange(double xMin, double xMax);

		/// Set the yRange
		void setYrange(double yMin, double yMax);

		/// Set the zRange
		void setZrange(double zMin, double zMax);

		/// Set the x step
		void setDx(double dx);

		/// Set the y step
		void setDz(double dy);

		/// Assignment operator
		ThreeDDataContainer& operator=(const ThreeDDataContainer&);

		/// Labels for the axes of the three-D plot
		QString xAxisLabel_, yAxisLabel_, zAxisLabel_;

	private:

		/// Underlying data
		QSurfaceDataArray* pSurfaceDataArray_;

		/// Ranges in the three directions
		Eigen::Array2d xRange_, yRange_, zRange_;

		/// Value of dx and dy
		double xStep_, zStep_;

};

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

		/// Getter for the Delta Residuary Resistance of the keel due to heel item
		Delta_ResiduaryResistanceKeel_HeelItem* getDelta_ResiduaryResistanceKeel_HeelItem() const;

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
		std::vector<VppXYCustomPlotWidget*> plotTotalResistance(WindIndicesDialog*, StateVectorDialog*);

		/// Make a 3d plot of the optimization variables v, phi when varying the two opt
		/// parameters flat and crew
		void plotOptimizationSpace();

		/// Make a 3d plot of the optimization variables v, phi when varying the two opt
		/// parameters flat and crew. Qt 3d surface plot
		vector<ThreeDDataContainer> plotOptimizationSpace(WindIndicesDialog&, OptimVarsStateVectorDialog&);

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

		/// Pointer to the Delta Residuary Resistance due to Heel Item
		boost::shared_ptr<Delta_ResiduaryResistance_HeelItem> pDelta_ResiduaryResistance_HeelItem_;

		/// Pointer to the Delta Residuary Resistance of the keel due to Heel Item
		boost::shared_ptr<Delta_ResiduaryResistanceKeel_HeelItem> pDelta_ResiduaryResistanceKeel_HeelItem_;

		/// Pointer to the Negative Resistance
		boost::shared_ptr<NegativeResistanceItem> pNegativeResistance_;

		/// Pointer to the Righting moment Item
		boost::shared_ptr<RightingMomentItem> pRightingMomentItem_;

		/// Residuals on the residuals : dF, dM, c1 and c2
		double dF_, dM_;

};

#endif
