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

		/// Returns a ptr to the parser
		VariableFileParser* getParser() const;

		/// Returns a ptr to the SailSet
		boost::shared_ptr<SailSet> getSailSet() const;

	protected:

		/// Velocity value in the state vector
		double V_;

		/// Heeling angle value in the state vector
		double PHI_;

		/// b value in the sate vector (reef..?)
		double b_;

		/// Flat value in the state vector
		double f_;

		/// Ptr to the file parser with the user variables
		VariableFileParser* pParser_;

		/// Ptr to the SailSet with the sail related variables
		boost::shared_ptr<SailSet> pSailSet_;

	private:

		/// Update the items for the current step (wind velocity and angle),
		/// the value of the state vector x computed by the optimizer
		virtual void update(int vTW, int aTW)=0;

};

//=================================================================

class VPPItemFactory {

	public:

		/// Constructor
		VPPItemFactory(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		~VPPItemFactory();

		/// Update the VPPItems for the current step (wind velocity and angle),
		/// the value of the state vector x computed by the optimizer
		void update(int vTW, int aTW, const double* x);

	private:

		/// Vector storing all the Children of VPPItem
		std::vector<boost::shared_ptr<VPPItem> > vppItems_;


};

//=================================================================

class WindItem : public VPPItem {

	public:

		/// Constructor
		WindItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		virtual ~WindItem();

		/// Print the class name -> in this case WindItem
		virtual void printWhoAmI();

		/// Own methods:

		/// Returns the true wind velocity for this step
		const double getTWV() const;

		/// Returns the true wind angle for this step
		const double getTWA() const;

		/// Returns the apparent wind angle for this step
		const double getAWA() const;

		/// Returns the apparent wind velocity vector for this step
		const Eigen::Vector2d getAWV() const;

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

//=================================================================

/// Utility class used by the AeroForcesItem to store the sail coefficients
/// the class is able to interpolate the coefficients for the current apparent
/// wind angle (computed by WindItem)
class SailCoefficientItem : public VPPItem {

	public:

		/// Constructor
		SailCoefficientItem(WindItem*);

		/// Destructor
		~SailCoefficientItem();

		/// Print the class name -> in this case SailCoefficientItem
		virtual void printWhoAmI();

		/// PrintOut the coefficient matrices
		void printCoefficients();

	private:

		/// Update the item for the current step (wind velocity and angle),
		/// the values of the state vector x computed by the optimizer have
		/// already been treated by the parent
		virtual void update(int vTW, int aTW);

		/// static arrays with the sail coefficients
		Eigen::MatrixXd cl_,cd_;

		/// Ptr to the wind item
		WindItem* pWindItem_;

};

//=================================================================

class AeroForcesItem : public VPPItem {

	public:

		/// Constructor
		AeroForcesItem(WindItem*);

		/// Destructor
		virtual ~AeroForcesItem();

		/// Print the class name -> in this case AeroForcesItem
		virtual void printWhoAmI();

	private:

		/// Update the item for the current step (wind velocity and angle),
		/// the values of the state vector x computed by the optimizer have
		/// already been treated by the parent
		virtual void update(int vTW, int aTW);

		/// Store a ptr to the windItem
		WindItem* pWindItem_;

		/// The AeroForcesItem owns the sail coefficients
		boost::shared_ptr<SailCoefficientItem> pSailCoeffs_;

};

//=================================================================

#endif
