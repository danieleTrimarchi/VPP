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

#endif
