#ifndef VPPHYDROITEM_H
#define VPPHYDROITEM_H

#include "VPPItem.h"
#include "VPPAeroItem.h"
#include "mathUtils.h"
#include "VppXYCustomPlotWidget.h"
#include "MultiplePlotWidget.h"

using namespace mathUtils;

/// Forward declarations
class WindIndicesDialog;
class StateVectorDialog;

// TODO dtrimarchi : we are not considering the resistance
// contribution of the rudder. Add it!
class ResistanceItem : public VPPItem {

	public:

		/// Destructor
		~ResistanceItem();

		// Get the value of the resistance for this ResistanceItem
		const double get() const;

		/// Convert a velocity [m/s] to a Fn[-]
		double convertToFn( double velocity );

		/// Convert a Fn[-] to a velocity [m/s]
		double convertToVelocity( double Fn );

		/// Each resistance component knows how to generate a widget
		/// to visualize itself in a plot
		virtual std::vector<VppXYCustomPlotWidget*> plot(WindIndicesDialog* wd =0, StateVectorDialog* =0) =0;

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	protected:

		/// Protected constructor (Avoid the instantiation of this class)
		ResistanceItem(VariableFileParser*, std::shared_ptr<SailSet>);

		/// drag the pure virtual method of the parent class one step down
		virtual void update(int vTW, int aTW);

		/// Froude number
		double fN_;

		/// Value of the resistance
		double res_;

};

///=================================================================
/// For the definition of the Induced Resistance see DSYHS99 ch4 p128
class InducedResistanceItem : public ResistanceItem {

	public:

		/// Constructor
		InducedResistanceItem(AeroForcesItem*);

		/// Destructor
		~InducedResistanceItem();

		/// Implement pure virtual of the parent class
		/// Each resistance component knows how to generate a widget
		/// to visualize itself in a plot
		virtual std::vector<VppXYCustomPlotWidget*> plot(WindIndicesDialog* wd =0, StateVectorDialog* =0);

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Pointer to the aerodynamic forces item
		AeroForcesItem* pAeroForcesItem_;

		/// Instantiate some utility arrays
		Eigen::MatrixXd coeffA_, coeffB_;
		Eigen::VectorXd vectA_;
		Eigen::ArrayXd phiD_,Tegeo_;

		/// Variables to be used to set a lower bound to the velocity
		/// ( Parabolic fitting in 0 -> V|(Fn=0.1)  )
		double vf_, a_, c_, v_;

		/// Smoothed step function used to smooth the values of the induced
		/// resistance in the neighborhood of 0
		std::shared_ptr<SmoothedStepFunction> pSf_;

};

//=================================================================
// Residuary Resistance Base: see Keuning 3.1.1.2 p112
class ResiduaryResistanceItemBase : public ResistanceItem {

	public:

		/// Constructor
		ResiduaryResistanceItemBase(VariableFileParser*, std::shared_ptr<SailSet>);

		/// Destructor
		~ResiduaryResistanceItemBase();

		/// Implement pure virtual of the parent class
		/// Each resistance component knows how to generate a widget
		/// to visualize itself in a plot
		virtual std::vector<VppXYCustomPlotWidget*> plot(WindIndicesDialog* wd =0, StateVectorDialog* =0);

	protected:

		/// Interpolator that stores the residuary resistance curve for all froude numbers
		std::shared_ptr<SplineInterpolator> pInterpolator_;

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

};

//=================================================================
// Residuary Resistance: see Keuning 3.1.1.2 p112
class ResiduaryResistanceItem : public ResiduaryResistanceItemBase {

	public:

		/// Constructor
		ResiduaryResistanceItem(VariableFileParser*, std::shared_ptr<SailSet>);

		/// Destructor
		~ResiduaryResistanceItem();


};

//=================================================================

/// Express the change in Appendage Resistance due to Heel.
/// See Keuning 3.2.2 p 126
class DeltaResistanceItemBase : public ResistanceItem {

	public:

		/// Constructor
		DeltaResistanceItemBase(VariableFileParser*, std::shared_ptr<SailSet>);

		/// Destructor
		virtual ~DeltaResistanceItemBase();

		/// Implement pure virtual of the parent class
		/// Each resistance component knows how to generate a widget
		/// to visualize itself in a plot
		virtual std::vector<VppXYCustomPlotWidget*> plot(WindIndicesDialog* wd, StateVectorDialog*);

};

/// =================================================================
/// For the definition of the change in Residuary Resistance due to heel
/// see DSYHS99 ch3.1.2.2 p116
class Delta_ResiduaryResistance_HeelItem : public DeltaResistanceItemBase {

	public:

		/// Constructor
		Delta_ResiduaryResistance_HeelItem(VariableFileParser*, std::shared_ptr<SailSet>);

		/// Destructor
		virtual ~Delta_ResiduaryResistance_HeelItem();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Interpolator that stores the residuary resistance curve for all froude numbers
		std::shared_ptr<SplineInterpolator> pInterpolator_;

};

/// =================================================================
/// For the definition of the Residuary Resistance of the Keel see
/// DSYHS99 3.2.1.2 p.120 and following

class ResiduaryResistanceKeelItem : public ResiduaryResistanceItemBase {

	public:

		/// Constructor
		ResiduaryResistanceKeelItem(VariableFileParser*, std::shared_ptr<SailSet>);

		/// Destructor
		~ResiduaryResistanceKeelItem();

};

//=================================================================

/// Express the change in Appendage Resistance due to Heel.
/// See Keuning 3.2.2 p 126
class Delta_ResiduaryResistanceKeel_HeelItem : public DeltaResistanceItemBase {

	public:

		/// Constructor
		Delta_ResiduaryResistanceKeel_HeelItem(VariableFileParser*, std::shared_ptr<SailSet>);

		/// Destructor
		~Delta_ResiduaryResistanceKeel_HeelItem();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Resistance coefficient
		double Ch_;

};

//=================================================================

// For the definition of the Viscous Resistance see Keuning 2.1 p108
class ViscousResistanceItemBase : public ResistanceItem {

	public:

		/// Constructor
		ViscousResistanceItemBase(VariableFileParser*, std::shared_ptr<SailSet>);

		/// Destructor
		~ViscousResistanceItemBase();

		/// Implement pure virtual of the parent class
		/// Each resistance component knows how to generate a widget
		/// to visualize itself in a plot
		virtual std::vector<VppXYCustomPlotWidget*> plot(WindIndicesDialog* wd =0, StateVectorDialog* =0);

	protected:

		std::string plotTitle_;

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW)=0;

};

//=================================================================

// For the definition of the Viscous Resistance see Keuning 2.1 p108
class ViscousResistanceItem : public ViscousResistanceItemBase {

	public:

		/// Constructor
		ViscousResistanceItem(VariableFileParser*, std::shared_ptr<SailSet>);

		/// Destructor
		~ViscousResistanceItem();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		double 	rN0_,  //< Velocity Independent part of the Reynolds number
		rfh0_; //< Velocity Independent part of the viscous resistance of the bare hull

};

//=================================================================

// For the definition of the Change in Viscous Resistance due to heel
// see Keuning 3.1.2.1 p115-116
class Delta_ViscousResistance_HeelItem : public DeltaResistanceItemBase {

	public:

		/// Constructor
		Delta_ViscousResistance_HeelItem(VariableFileParser*, std::shared_ptr<SailSet>);

		/// Destructor
		~Delta_ViscousResistance_HeelItem();

		/// Plot the Viscous Resistance due to heel vs Fn curve
		std::vector<VppXYCustomPlotWidget*> plot_deltaWettedArea_heel();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		//< Velocity Independent part of the Reynolds number
		double 	rN0_;

		/// Interpolator that stores the residuary resistance curve for all froude numbers
		std::shared_ptr<SplineInterpolator> pInterpolator_;

};

//=================================================================

class ViscousResistanceKeelItem : public ViscousResistanceItemBase {

	public:

		/// Constructor
		ViscousResistanceKeelItem(VariableFileParser*, std::shared_ptr<SailSet>);

		/// Destructor
		~ViscousResistanceKeelItem();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

};

//=================================================================

class ViscousResistanceRudderItem : public ViscousResistanceItemBase {

	public:

		/// Constructor
		ViscousResistanceRudderItem(VariableFileParser*, std::shared_ptr<SailSet>);

		/// Destructor
		~ViscousResistanceRudderItem();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

};

//=================================================================
// Class that returns a ficticious negative resistance in the case
// of negative velocities. Used to stabilize the computations (NRSolver)
class NegativeResistanceItem : public ViscousResistanceItemBase {

	public:

		/// Constructor
	NegativeResistanceItem(VariableFileParser*, std::shared_ptr<SailSet>);

		/// Destructor
		~NegativeResistanceItem();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

};

//=================================================================

#endif
