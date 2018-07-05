#ifndef VPPRIGHTINGMOMENTITEM_H
#define VPPRIGHTINGMOMENTITEM_H

#include "VPPItem.h"

/// todo dtrimarchi : verify where this definition comes
/// from, perhaps Larsson?
class RightingMomentItem : public VPPItem {

	public:

		/// Constructor
		RightingMomentItem(VariableFileParser*, std::shared_ptr<SailSet>);

		/// Destructor
		~RightingMomentItem();

		/// Get the righting moment value
		const double get() const;

	private:

		/// Update the item for the current step (wind velocity and angle),
		/// the values of the state vector x computed by the optimizer have
		/// already been treated by the parent
		virtual void update(int vTW, int aTW);

		/// Constant parts of the two components of the righting moment, and
		/// righting moment value
		double m10_,m20_, val_;
};

#endif
