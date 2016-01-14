#ifndef VPPHYDROITEM_H
#define VPPHYDROITEM_H

#include "VPPItem.h"
#include "VPPAeroItem.h"

class ResistanceItem : public VPPItem {

	public:

		/// Constructor
		ResistanceItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~ResistanceItem();

	private:

		/// drag the pure virtual method of the parent class one step down
		virtual void update(int vTW, int aTW) =0;


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

#endif
