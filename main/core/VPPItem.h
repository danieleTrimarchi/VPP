#ifndef VPPITEM_H
#define VPPITEM_H

#include <vector>
#include "boost/shared_ptr.hpp"
#include <Eigen/Core>

#include "VariableFileParser.h"
#include "SailSet.h"
#include "Interpolator.h"

using namespace std;
using namespace Eigen;

/// Forward declarations
class SailSet;

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
		void updateSolution(int vTW, int aTW, const double* x);

		/// Update the parent VPPItem for the current step (wind velocity and angle),
		/// the value of the state vector x computed by the optimizer. Then, call the
		/// update method for the children in the vppItems_ vector
		void updateSolution(int vTW, int aTW, Eigen::VectorXd& x);

		/// Print the class name -> in this case VPPItem
		virtual void printWhoAmI()=0;

		/// Returns a ptr to the parser
		VariableFileParser* getParser() const;

		/// Returns a ptr to the SailSet
		boost::shared_ptr<SailSet> getSailSet() const;

	protected:

		/// Velocity value in the state vector
		double V_;

		/// Heeling angle value in the state vector in RADIANS
		double PHI_;

		/// b value in the sate vector (Crew position)
		double b_;

		/// Flat value in the state vector
		double f_;

		/// Size of the pb, or the number of state variables
		size_t pbSize_;

		/// Ptr to the file parser with the user variables
		VariableFileParser* pParser_;

		/// Ptr to the SailSet with the sail related variables
		boost::shared_ptr<SailSet> pSailSet_;

	private:

		/// Update the items for the current step (wind velocity and angle),
		/// the value of the state vector x computed by the optimizer
		virtual void update(int vTW, int aTW)=0;

};


#endif