#ifndef VPPHYDROITEM_H
#define VPPHYDROITEM_H

#include "VPPItem.h"
#include "VPPAeroItem.h"

class ResistanceItem : public VPPItem {

	public:

		/// Destructor
		~ResistanceItem();

	protected:

		/// Protected constructor (Avoid the instantiation of this class)
		ResistanceItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// drag the pure virtual method of the parent class one step down
		virtual void update(int vTW, int aTW);

		/// Froude number
		double fN_;

};

//=================================================================

class InducedResistanceItem : public ResistanceItem {

	public:

		/// Constructor
		InducedResistanceItem(AeroForcesItem*);

		/// Destructor
		~InducedResistanceItem();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Pointer to the aerodynamic forces item
		AeroForcesItem* pAeroForcesItem_;

		/// Value of the induced resistance
		double ri_;

		/// Instantiate some utility arrays
		Eigen::ArrayXXd coeffA_, coeffB_;
	  Eigen::ArrayXd phiD_, vectA_, Tegeo_;

};

//=================================================================

class ResiduaryResistanceItem : public ResistanceItem {

	public:

		/// Constructor
		ResiduaryResistanceItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~ResiduaryResistanceItem();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Value of the induced resistance
		double rrh_;

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

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Value of the delta induced resistance due to heel
		double dRRH_;

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

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Value of the induced resistance
		double rrk_;

		/// Interpolator that stores the residuary resistance curve for all froude numbers
		boost::shared_ptr<SplineInterpolator> pInterpolator_;
};

//=================================================================

class Delta_ResiduaryResistanceKeel_HeelItem : public ResistanceItem {

	public:

		/// Constructor
		Delta_ResiduaryResistanceKeel_HeelItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~Delta_ResiduaryResistanceKeel_HeelItem();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Resistance coefficient
		double Ch_;

		/// Value of the delta induced resistance due to heel
		double dRRKH_;

};

//=================================================================

class FrictionalResistanceItem : public ResistanceItem {

	public:

		/// Constructor
		FrictionalResistanceItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~FrictionalResistanceItem();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		double 	rN0_,  //< Velocity Independent part of the Reynolds number
						rfh0_, //< Velocity Independent part of the Frictional resistance of the bare hull
						rv_;   //< Frictional resistance value

};

//=================================================================

class Delta_FrictionalResistance_HeelItem : public ResistanceItem {

	public:

		/// Constructor
		Delta_FrictionalResistance_HeelItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~Delta_FrictionalResistance_HeelItem();

	private:

		/// Implement pure virtual method of the parent class
		virtual void update(int vTW, int aTW);

		/// Value of the delta frictional resistance due to heel
		double dFRKH_;

		//< Velocity Independent part of the Reynolds number
		double 	rN0_;

		/// Interpolator that stores the residuary resistance curve for all froude numbers
		boost::shared_ptr<SplineInterpolator> pInterpolator_;

};

//=================================================================

#endif
