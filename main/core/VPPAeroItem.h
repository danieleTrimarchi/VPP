#ifndef VPPAEROITEM_H
#define VPPAEROITEM_H

#include "VPPItem.h"
#include "SailSet.h"
#include "Physics.h"
#include "boost/shared_ptr.hpp"

using namespace Physic;

/// Forward declarations
class MultiplePlotWidget;
class VppTabDockWidget;
class VPPXYChart;

class WindItem : public VPPItem {

	public:

		/// Constructor
		WindItem(VariableFileParser*, boost::shared_ptr<SailSet>);

		/// Destructor
		virtual ~WindItem();

		/// Own methods:

		/// Returns the true wind velocity for this step
		const double getTWV() const;

		/// Returns the true wind velocity for a given step
		const double getTWV(size_t iV) const;

		/// Returns the number of wind velocities defined in the variableFile
		const int getWVSize() const;

		/// Returns the true wind angle for this step
		const double getTWA() const;

		/// Returns the true wind angle [rad] for a given step
		const double getTWA(size_t iA) const;

		/// Returns the number of true wind angles
		const int getWASize() const;

		/// Returns the apparent wind angle for this step
		const double getAWA() const;

		/// Returns the apparent wind velocity vector for this step
		const Eigen::Vector2d getAWV() const;

		/// Returns the apparent wind velocity vector norm for this step
		const double getAWNorm() const;

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		/// Make this class friend of the VPPAeroItem, that requires data to plot
		friend class AeroForcesItem;

	private:

		/// Update the item for the current step (wind velocity and angle),
		/// the values of the state vector x computed by the optimizer have
		/// already been treated by the parent
		virtual void update(int vTW, int aTW);

		/// True wind velocity
		double twv_;

		/// True and apparent wind angles (wrt the boat's longitudinal axis)
		/// Expressed in radians
		double twa_, awa_;

		double v_tw_min_, v_tw_max_;
		int n_twv_,n_alpha_tw_;
		double alpha_tw_min_, alpha_tw_max_;

		// Apparent wind velocity vector (x/y components)
		Eigen::Vector2d awv_;

		/// Containers to store the values of the wind true
		/// velocity an angles requested by the user
		vector<double> vTwv_, vTwa_;

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

		/// Returns a ptr to the wind Item
		WindItem* getWindItem() const;

		/// PrintOut the coefficients for main, jib and spi.
		/// Note that these coefficients are the values interpolated
		/// for the current awa_
		void printCoefficients();

		/// Plot the spline-interpolated curves based on the Larsson's
		/// sail coefficients. The range is set 0-180deg
		/// Use PLPlot
		virtual void plotInterpolatedCoefficients() const=0;

		/// Plot the spline-interpolated curves based on the Larsson's
		/// sail coefficients. The range is set 0-180deg
		/// Fill a multiple plot
		virtual void plotInterpolatedCoefficients( MultiplePlotWidget* ) const =0;

		/// Plot the first derivative of the spline-interpolated
		/// curves based on the Larsson's sail coefficients.
		/// The range is set 0-180deg
		virtual void plot_D_InterpolatedCoefficients() const=0;

		/// Plot the second derivative of the spline-interpolated
		/// curves based on the Larsson's sail coefficients.
		/// The range is set 0-180deg
		virtual void plot_D2_InterpolatedCoefficients() const=0;

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

	protected:

		/// Update the item for the current step (wind velocity and angle),
		/// the values of the state vector x computed by the optimizer have
		/// already been treated by the parent
		virtual void update(int vTW, int aTW);

		/// Value of the apparent wind angle updated by the WindItem
		double awa_;

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

		/// Interpolator vectors -- store a cubic spline that interpolates
		/// the values of the sail coefficients
		vector< boost::shared_ptr<SplineInterpolator> > interpClVec_;
		vector< boost::shared_ptr<SplineInterpolator> > interpCdVec_;

	private:

		/// Pure virtual that makes this class abstract.
		virtual void compute() =0;

		/// Ptr to the wind item
		WindItem* pWindItem_;

};

//=================================================================

class MainOnlySailCoefficientItem : public SailCoefficientItem {

	public:

		/// Constructor
		MainOnlySailCoefficientItem(WindItem*);

		/// Destructor
		~MainOnlySailCoefficientItem();

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Plot the spline-interpolated curves based on the Larsson's
		/// sail coefficients. The range is set 0-180deg
		virtual void plotInterpolatedCoefficients() const;

		/// Plot the spline-interpolated curves based on the Larsson's
		/// sail coefficients. The range is set 0-180deg
		/// Fill a multiple plot
		virtual void plotInterpolatedCoefficients( MultiplePlotWidget* ) const;

		/// Plot the first derivative of the spline-interpolated
		/// curves based on the Larsson's sail coefficients.
		/// The range is set 0-180deg
		virtual void plot_D_InterpolatedCoefficients() const;

		/// Plot the second derivative of the spline-interpolated
		/// curves based on the Larsson's sail coefficients.
		/// The range is set 0-180deg
		virtual void plot_D2_InterpolatedCoefficients() const;

	private:

		/// Implement the pure virtual method of the abstract base class
		virtual void compute();

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

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Plot the spline-interpolated curves based on the Larsson's
		/// sail coefficients. The range is set 0-180deg
		virtual void plotInterpolatedCoefficients() const;

		/// Plot the spline-interpolated curves based on the Larsson's
		/// sail coefficients. The range is set 0-180deg
		/// Fill a multiple plot
		virtual void plotInterpolatedCoefficients( MultiplePlotWidget* ) const;

		/// Plot the first derivative of the spline-interpolated
		/// curves based on the Larsson's sail coefficients.
		/// The range is set 0-180deg
		virtual void plot_D_InterpolatedCoefficients() const;

		/// Plot the second derivative of the spline-interpolated
		/// curves based on the Larsson's sail coefficients.
		/// The range is set 0-180deg
		virtual void plot_D2_InterpolatedCoefficients() const;

	private:

		/// Implement the pure virtual method of the abstract base class
		virtual void compute();

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

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Plot the spline-interpolated curves based on the Larsson's
		/// sail coefficients. The range is set 0-180deg
		virtual void plotInterpolatedCoefficients() const;

		/// Plot the spline-interpolated curves based on the Larsson's
		/// sail coefficients. The range is set 0-180deg
		/// Fill a multiple plot
		virtual void plotInterpolatedCoefficients( MultiplePlotWidget* ) const;

		/// Plot the first derivative of the spline-interpolated
		/// curves based on the Larsson's sail coefficients.
		/// The range is set 0-180deg
		virtual void plot_D_InterpolatedCoefficients() const;

		/// Plot the second derivative of the spline-interpolated
		/// curves based on the Larsson's sail coefficients.
		/// The range is set 0-180deg
		virtual void plot_D2_InterpolatedCoefficients() const;

	private:

		/// Implement the pure virtual method of the abstract base class
		virtual void compute();

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

		/// Print the class name - implement the pure virtual of VPPItem
		virtual void printWhoAmI();

		/// Plot the spline-interpolated curves based on the Larsson's
		/// sail coefficients. The range is set 0-180deg
		virtual void plotInterpolatedCoefficients() const;

		/// Plot the spline-interpolated curves based on the Larsson's
		/// sail coefficients. The range is set 0-180deg
		/// Fill a multiple plot
		virtual void plotInterpolatedCoefficients( MultiplePlotWidget* ) const;

		/// Plot the first derivative of the spline-interpolated
		/// curves based on the Larsson's sail coefficients.
		/// The range is set 0-180deg
		virtual void plot_D_InterpolatedCoefficients() const;

		/// Plot the second derivative of the spline-interpolated
		/// curves based on the Larsson's sail coefficients.
		/// The range is set 0-180deg
		virtual void plot_D2_InterpolatedCoefficients() const;

	private:

		/// Implement the pure virtual method of the abstract base class
		virtual void compute();

		/// Update the item for the current step (wind velocity and angle),
		/// the values of the state vector x computed by the optimizer have
		/// already been treated by the parent
		virtual void update(int vTW, int aTW);

};

//=================================================================

class AeroForcesItem : public VPPItem {

	public:

		/// Constructor
		AeroForcesItem(SailCoefficientItem* pSailCoeffItem);

		/// Destructor
		virtual ~AeroForcesItem();

		/// Get the value of the lift
		const double getLift() const;

		/// Get the value of the drag
		const double getDrag() const;

		/// Get the value of the side force
		const double getFSide() const;

		/// Get the value of the drive force
		const double getFDrive() const;

		/// Get the value of the heel moment
		const double getMHeel() const;

		/// Get a ptr to the wind item
		WindItem* getWindItem();

		/// Get a ptr to the wind item - const variety
		const WindItem* getWindItem() const;

		/// Get a ptr to the sailCoeffs Item
		SailCoefficientItem* getSailCoeffItem();

		/// Get a ptr to the sailCoeffs Item - const variety
		const SailCoefficientItem* getSailCoeffItem() const;

		/// plot the aeroForces for a fixed range
		void plot();

		/// Print the class name -> in this case AeroForcesItem
		virtual void printWhoAmI();

	private:

		/// Update the item for the current step (wind velocity and angle),
		/// the values of the state vector x computed by the optimizer have
		/// already been treated by the parent
		virtual void update(int vTW, int aTW);

		/// The AeroForcesItem owns the sail coefficients
		SailCoefficientItem* pSailCoeffs_;

		/// Ptr to the WindItem that knows the twv
		WindItem* pWindItem_;

		double lift_, drag_,		//< Lift and Drag
		fDrive_, fHeel_,	//< Drive and Heel forces, ie lift and drag projected to the boat's route
		fSide_, 					//< Side force, or fHeel projected on the sea plane
		mHeel_;						//< Heel moment due to fHeel
};


#endif
