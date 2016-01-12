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

		/// Returns the current value of the lift coefficient for
		/// the current sail
		const double getCl() const;

		/// Returns the current value of the lift coefficient
		const double getCd() const;

		/// Print the class name -> in this case SailCoefficientItem
		virtual void printWhoAmI();

		/// PrintOut the coefficient matrices
		void printCoefficients();

	protected:

		/// Update the item for the current step (wind velocity and angle),
		/// the values of the state vector x computed by the optimizer have
		/// already been treated by the parent
		virtual void update(int vTW, int aTW);

		/// Value of the apparent wind angle updated by the WindItem
		double awa_;

		/// Arrays with the sail coefficients
		Eigen::ArrayXXd clMat0_,cdpMat0_;

		/// Compute the coefficients for the MainSail
		void computeForMain();

		/// Compute the coefficients for the Jib
		void computeForJib();

		/// Compute the coefficients for the Spinnaker
		void computeForSpi();

		/// Called after the children's update as a decorator
		/// Computes the actual cl (decremented with the flat)
		/// and the effective cd = cdp + cd0 + cdI
		void postUpdate();

		/// Current values of the lift and drag coefficients for Main,
		/// Jib and Spi. The values are updated by update and interpolated
		/// with the current awa computed by the WindItem with the actual
		/// boat velocity from the state vector of the optimizer
		Eigen::Vector3d allCl_, allCd_;

		/// Current lift/drag coefficient values for the full sailset
		double 	cl_, //< Lift coefficient
						ar_, //<
						cdp_,//<
						cdI_, //< Induced SailSet Resistance
						cd0_, //<
						cd_; //< Drag coefficient

	private:

		/// Ptr to the wind item
		WindItem* pWindItem_;

		/// Interpolator used to return the value of the sailcoefficients
		/// Interpolated on the current apparent wind angle
		Interpolator interpolator_;

};

//=================================================================

class MainOnlySailCoefficientItem : public SailCoefficientItem {

	public:

		/// Constructor
		MainOnlySailCoefficientItem(WindItem*);

		/// Destructor
		~MainOnlySailCoefficientItem();


	private:

		/// Update the item for the current step (wind velocity and angle),
		/// the values of the state vector x computed by the optimizer have
		/// already been treated by the parent
		virtual void update(int vTW, int aTW);

};

//=================================================================

class MainAndJibCoefficientItem : public SailCoefficientItem {

	public:

		/// Constructor
		MainAndJibCoefficientItem(WindItem*);

		/// Destructor
		~MainAndJibCoefficientItem();


	private:

		/// Update the item for the current step (wind velocity and angle),
		/// the values of the state vector x computed by the optimizer have
		/// already been treated by the parent
		virtual void update(int vTW, int aTW);

};

//=================================================================

class MainAndSpiCoefficientItem : public SailCoefficientItem {

	public:

		/// Constructor
		MainAndSpiCoefficientItem(WindItem*);

		/// Destructor
		~MainAndSpiCoefficientItem();


	private:

		/// Update the item for the current step (wind velocity and angle),
		/// the values of the state vector x computed by the optimizer have
		/// already been treated by the parent
		virtual void update(int vTW, int aTW);

};

//=================================================================

class MainJibAndSpiCoefficientItem : public SailCoefficientItem {

	public:

		/// Constructor
		MainJibAndSpiCoefficientItem(WindItem*);

		/// Destructor
		~MainJibAndSpiCoefficientItem();


	private:

		/// Update the item for the current step (wind velocity and angle),
		/// the values of the state vector x computed by the optimizer have
		/// already been treated by the parent
		virtual void update(int vTW, int aTW);

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

		/// Define SailType - used to query for the force coefficients
		enum sailType {
			Main,
			Jib,
			Spi
		};

		/// Store a ptr to the windItem
		WindItem* pWindItem_;

		/// The AeroForcesItem owns the sail coefficients
		boost::shared_ptr<SailCoefficientItem> pSailCoeffs_;

};

//=================================================================

#endif
