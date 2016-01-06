#ifndef VPPITEM_H
#define VPPITEM_H

#include <vector>
#include "boost/shared_ptr.hpp"
#include <Eigen/Core>

#include "VariableFileParser.h"
#include "SailSet.h"

using namespace std;
using namespace Eigen;

/// Generic base class for all the VPP items
class VPPItem {

	public:

		/// Constructor
		VPPItem(VariableFileParser*, boost::shared_ptr<SailSet> );

		/// Destructor
		virtual ~VPPItem();

		/// Update the parent VPPItem for the current step (wind velocity and angle),
		/// the value of the state vector x computed by the optimizer. Then, call the
		/// update method for the children in the vppItems_ vector
		void update(int vTW, int aTW, const double* x);

		/// Print the class name -> in this case VPPItem
		virtual void printWhoAmI();

	protected:

		/// Returns the parser
		VariableFileParser* getParser();

		/// Returns the SailSet
		boost::shared_ptr<SailSet> getSailSet();

		/// Velocity value in the state vector
		double V_;

		/// Heeling angle value in the state vector
		double PHI_;

		/// b value in the sate vector (reef..?)
		double b_;

		/// Flat value in the state vector
		double f_;

	private:

		/// Update the items for the current step (wind velocity and angle),
		/// the value of the state vector x computed by the optimizer
		void update(int vTW, int aTW) =0;

		/// Ptr to the file parser with the user variables
		VariableFileParser* pParser_;

		/// Ptr to the SailSet with the sail related variables
		boost::shared_ptr<SailSet> pSailSet_;

		/// Vector of VPPItem ptrs
		vector<boost::shared_ptr<VPPItem> > vppItems_;

};

//=================================================================

class WindItem : public VPPItem {

	public:

		/// Constructor
		WindItem();

		/// Destructor
		virtual ~WindItem();

		/// Print the class name -> in this case WindItem
		virtual void printWhoAmI();

	private:

		/// Update the item for the current step (wind velocity and angle),
		/// the values of the state vector x computed by the optimizer have
		/// already been treated by the parent
		virtual void update(int vTW, int aTW);

		/// True wind velocity
		double twv_;

		/// True and apparent wind angles (wrt the boat's longitudinal axis)
		double twa_, awa_;

		// Apparent wind velocity vector (x/y components)
		Eigen::Vector2d awv_;

		/// Pi in radians
		double pi_;

};


#endif
