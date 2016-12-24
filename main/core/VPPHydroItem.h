#ifndef VPPHYDROITEM_H
#define VPPHYDROITEM_H

#include "VPPItem.h"
#include "VPPAeroItem.h"

#include "mathUtils.h"
using namespace mathUtils;

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

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	protected:

		/// Protected constructor (Avoid the instantiation of this class)
		ResistanceItem(VariableFileParser*, boost::shared_ptr<SailSet>);

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

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Plot the Induced Resistance curve
		void plot();

		/// Plot the effective T
		void plotTe(int twv, int twa);

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
		boost::shared_ptr<SmoothedStepFunction> pSf_;

};

//=================================================================
// Residuary Resistance: see Keuning 3.1.1.2 p112
class ResiduaryResistanceItem : public ResistanceItem {

	public:

		/// Constructor
		ResiduaryResistanceItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~ResiduaryResistanceItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Plot the Residuary Resistance vs Fn curve
		void plot(WindItem* pWind);

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Interpolator that stores the residuary resistance curve for all froude numbers
		boost::shared_ptr<SplineInterpolator> pInterpolator_;
};

/// =================================================================
/// For the definition of the change in Residuary Resistance due to heel
/// see DSYHS99 ch3.1.2.2 p116
class Delta_ResiduaryResistance_HeelItem : public ResistanceItem {

	public:

		/// Constructor
		Delta_ResiduaryResistance_HeelItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~Delta_ResiduaryResistance_HeelItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Plot the Residuary Resistance vs Fn curve
		void plot(WindItem* pWind);

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Interpolator that stores the residuary resistance curve for all froude numbers
		boost::shared_ptr<SplineInterpolator> pInterpolator_;

};

/// =================================================================
/// For the definition of the Residuary Resistance of the Keel see
/// DSYHS99 3.2.1.2 p.120 and following

class ResiduaryResistanceKeelItem : public ResistanceItem {

	public:

		/// Constructor
		ResiduaryResistanceKeelItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~ResiduaryResistanceKeelItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Plot the Residuary Resistance of the Keel versus Fn curve
		void plot();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Interpolator that stores the residuary resistance curve for all froude numbers
		boost::shared_ptr<SplineInterpolator> pInterpolator_;

};

//=================================================================

/// Express the change in Appendage Resistance due to Heel.
/// See Keuning 3.2.2 p 126
class Delta_ResiduaryResistanceKeel_HeelItem : public ResistanceItem {

	public:

		/// Constructor
		Delta_ResiduaryResistanceKeel_HeelItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~Delta_ResiduaryResistanceKeel_HeelItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Plot
		void plot(WindItem* pWind);

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Resistance coefficient
		double Ch_;

};

//=================================================================

// For the definition of the Frictional Resistance see Keuning 2.1 p108
class FrictionalResistanceItem : public ResistanceItem {

	public:

		/// Constructor
		FrictionalResistanceItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~FrictionalResistanceItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Plot the frictional resistance for a fixed range (Fn=0-0.7)
		void plot();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		double 	rN0_,  //< Velocity Independent part of the Reynolds number
		rfh0_; //< Velocity Independent part of the Frictional resistance of the bare hull

};

//=================================================================

// For the definition of the Change in Frictional Resistance due to heel
// see Keuning 3.1.2.1 p115-116
class Delta_FrictionalResistance_HeelItem : public ResistanceItem {

	public:

		/// Constructor
		Delta_FrictionalResistance_HeelItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~Delta_FrictionalResistance_HeelItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Plot the Frictional Resistance due to heel vs Fn curve
		void plot_deltaWettedArea_heel();

		/// Plot the Frictional Resistance due to heel vs Fn curve for a range of heeling angles
		void plot(WindItem* pWind);

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		//< Velocity Independent part of the Reynolds number
		double 	rN0_;

		/// Interpolator that stores the residuary resistance curve for all froude numbers
		boost::shared_ptr<SplineInterpolator> pInterpolator_;

};

//=================================================================

class ViscousResistanceKeelItem : public ResistanceItem {

	public:

		/// Constructor
		ViscousResistanceKeelItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~ViscousResistanceKeelItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Plot the viscous resistance of the keel for a fixed range (Fn=0-0.7)
		void plot();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

};

//=================================================================

class ViscousResistanceRudderItem : public ResistanceItem {

	public:

		/// Constructor
		ViscousResistanceRudderItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~ViscousResistanceRudderItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Plot the viscous resistance of the rudder for a fixed range (Fn=0-0.7)
		void plot();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

};

//=================================================================
// Class that returns a ficticious negative resistance in the case
// of negative velocities. Used to stabilize the computations (NRSolver)
class NegativeResistanceItem : public ResistanceItem {

	public:

		/// Constructor
	NegativeResistanceItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~NegativeResistanceItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Plot the negative resistance for a fixed range (Fn=0-1)
		void plot();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

};

//=================================================================
#endif
