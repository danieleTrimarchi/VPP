#ifndef VPPHYDROITEM_H
#define VPPHYDROITEM_H

#include "VPPItem.h"
#include "VPPAeroItem.h"

// todo dtrimarchi : we are not considering the resistance
// contribution of the rudder. Add it!

class ResistanceItem : public VPPItem {

	public:

		/// Destructor
		~ResistanceItem();

		// Get the value of the resistance for this ResistanceItem
		const double get() const;

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

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

//=================================================================

class InducedResistanceItem : public ResistanceItem {

	public:

		/// Constructor
		InducedResistanceItem(AeroForcesItem*);

		/// Destructor
		~InducedResistanceItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Pointer to the aerodynamic forces item
		AeroForcesItem* pAeroForcesItem_;

		/// Instantiate some utility arrays
		Eigen::MatrixXd coeffA_, coeffB_;
		Eigen::VectorXd vectA_;
		Eigen::ArrayXd phiD_,Tegeo_;
};

//=================================================================

class ResiduaryResistanceItem : public ResistanceItem {

	public:

		/// Constructor
		ResiduaryResistanceItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~ResiduaryResistanceItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Interpolator that stores the residuary resistance curve for all froude numbers
		boost::shared_ptr<SplineInterpolator> pInterpolator_;
};

//=================================================================

class Delta_ResiduaryResistance_HeelItem : public ResistanceItem {

	public:

		/// Constructor
		Delta_ResiduaryResistance_HeelItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~Delta_ResiduaryResistance_HeelItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Interpolator that stores the residuary resistance curve for all froude numbers
		boost::shared_ptr<SplineInterpolator> pInterpolator_;

};

//=================================================================

class ResiduaryResistanceKeelItem : public ResistanceItem {

	public:

		/// Constructor
		ResiduaryResistanceKeelItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~ResiduaryResistanceKeelItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

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

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Resistance coefficient
		double Ch_;

};

//=================================================================

class FrictionalResistanceItem : public ResistanceItem {

	public:

		/// Constructor
		FrictionalResistanceItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~FrictionalResistanceItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		double 	rN0_,  //< Velocity Independent part of the Reynolds number
		rfh0_; //< Velocity Independent part of the Frictional resistance of the bare hull

};

//=================================================================

class Delta_FrictionalResistance_HeelItem : public ResistanceItem {

	public:

		/// Constructor
		Delta_FrictionalResistance_HeelItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~Delta_FrictionalResistance_HeelItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

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

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

};

//=================================================================

#endif
