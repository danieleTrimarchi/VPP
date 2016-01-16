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
		void update(int vTW, int aTW, const double* x);

		/// Getter for the aero forces item that stores the driving forces
		boost::shared_ptr<AeroForcesItem> getAeroForcesItem();

		/// Compute the resistance by summing up all the contributions
		double getResistance();

	private:

		/// Vector storing all the Children of VPPItem
		std::vector<boost::shared_ptr<VPPItem> > vppAeroItems_;
		std::vector<boost::shared_ptr<ResistanceItem> > vppHydroItems_;

		/// Pointer to the Aero forces Item
		boost::shared_ptr<AeroForcesItem> pAeroForcesItem_;

		/// Pointer to the Righting moment Item
		boost::shared_ptr<RightingMomentItem> pRightingMomentItem_;

};

#endif
