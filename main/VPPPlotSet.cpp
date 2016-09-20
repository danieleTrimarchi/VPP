#include "VPPPlotSet.h"
#include "mathUtils.h"

using namespace mathUtils;

// Ctor
VPPPlotSet::VPPPlotSet(ResultContainer* pVppResult):
pResults_(pVppResult) {

}

// Dtor
VPPPlotSet::~VPPPlotSet() {

}

// Make XY plots for a given angle
void VPPPlotSet::plotXY(size_t iWa) {

	// This can't be correct, because it does NOT take into account the 'discard'
	size_t numValidResults = pResults_->getNumValidResultsForAngle(iWa);

	// Prepare the data for the plotter
	Eigen::ArrayXd windSpeeds(numValidResults);
	Eigen::ArrayXd boatVelocity(numValidResults);
	Eigen::ArrayXd boatHeel(numValidResults);
	Eigen::ArrayXd boatFlat(numValidResults);
	Eigen::ArrayXd boatB(numValidResults);
	Eigen::ArrayXd dF(numValidResults);
	Eigen::ArrayXd dM(numValidResults);

	// Loop on all results but only plot the valid ones
	size_t idx=0;
	for(size_t iWv=0; iWv<pResults_->windVelocitySize(); iWv++) {

		if(!pResults_->get(iWv,iWa).discard()) {

			windSpeeds(idx)  = pResults_->get(iWv,iWa).getTWV();
			boatVelocity(idx)= pResults_->get(iWv,iWa).getX()->coeff(0);
			boatHeel(idx)    = mathUtils::toDeg(pResults_->get(iWv,iWa).getX()->coeff(1));
			boatB(idx)    	 = pResults_->get(iWv,iWa).getX()->coeff(2);
			boatFlat(idx)    = pResults_->get(iWv,iWa).getX()->coeff(3);
			dF(idx)          = pResults_->get(iWv,iWa).getdF();
			dM(idx)          = pResults_->get(iWv,iWa).getdM();
			idx++;

		}
	}

	char title[256];
	sprintf(title,"AWA= %4.2f", toDeg(pResults_->getWind()->getTWA(iWa)) );

	// Instantiate a plotter for the velocity
	VPPPlotter plotter;
	string t=string("Boat Speed")+string(title);
	plotter.plot(windSpeeds,boatVelocity,windSpeeds,boatVelocity,
			t,"Wind Speed [m/s]","Boat Speed [m/s]");

	// Instantiate a plotter for the heel
	VPPPlotter plotter2;
	string t2=string("Boat Heel")+string(title);
	plotter2.plot(windSpeeds,boatHeel,windSpeeds,boatHeel,
			t2,"Wind Speed [m/s]","Boat Heel [deg]");

	// Instantiate a plotter for the Flat
	VPPPlotter plotter3;
	string t3=string("Sail FLAT")+string(title);
	plotter3.plot(windSpeeds,boatFlat,windSpeeds,boatFlat,
			t3,"Wind Speed [m/s]","Sail FLAT [-]");

	// Instantiate a plotter for the position of the movable crew B
	VPPPlotter plotter4;
	string t4=string("Crew position")+string(title);
	plotter4.plot(windSpeeds,boatB,windSpeeds,boatB,
			t4,"Wind Speed [m/s]","Position of the movable crew [m]");

	// Instantiate a plotter for the residuals
	VPPPlotter plotter5;
	string t5=string("Residuals")+string(title);
	plotter5.plot(windSpeeds,dF,windSpeeds,dM,
			t5,"Wind Speed [m/s]","Residuals [N,N*m]");
}
