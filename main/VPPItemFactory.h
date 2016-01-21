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

		/// Returns a ptr to the variableFileParser
		VariableFileParser* getParser();

		/// Returns a ptr to the wind item
		WindItem* getWind() const;

		/// Getter for the aero forces item that stores the driving forces
		const AeroForcesItem* getAeroForcesItem() const;

		/// Compute the resistance by summing up all the contributions
		double getResistance();

		/// void computeResiduals todo dtrimarchi: allow for a useful interface
		void computeResiduals(double& dF, double& dM);

	private:

		/// ptr to the VariableFileParser
		VariableFileParser* pParser_;

		/// Vector storing all the Children of VPPItem
		std::vector<boost::shared_ptr<VPPItem> > vppAeroItems_;

		/// Pointer to the resistance item
		std::vector<boost::shared_ptr<ResistanceItem> > vppHydroItems_;

		/// Pointer to the Wind Item, owned by pAeroForcesItem_
		boost::shared_ptr<WindItem> pWind_;

		/// Pointer to the Aero forces Item
		boost::shared_ptr<AeroForcesItem> pAeroForcesItem_;

		/// Pointer to the Righting moment Item
		boost::shared_ptr<RightingMomentItem> pRightingMomentItem_;

};

#endif
