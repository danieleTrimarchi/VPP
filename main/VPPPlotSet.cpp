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

	// Get the number of valid results (discard==false)
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
			t2,"Wind Speed [m/s]","Boat Heelº");

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

// Make polar plots
void VPPPlotSet::plotPolars() {

	// Instantiate the Polar Plotters for Boat velocity, Boat heel,
	// Sail flat, Crew B, dF and dM
	VPPPolarPlotter boatSpeedPolarPlotter("Boat Speed Polar Plot");
	VPPPolarPlotter boatHeelPolarPlotter("Boat Heel Polar Plot");
	VPPPolarPlotter crewBPolarPlotter("Crew B Polar Plot");
	VPPPolarPlotter sailFlatPolarPlotter("Sail Flat");

	// Instantiate the list of wind angles that will serve
	// for each velocity
	Eigen::ArrayXd windAngles;
	Eigen::ArrayXd boatVelocity;
	Eigen::ArrayXd boatHeel;
	Eigen::ArrayXd crewB;
	Eigen::ArrayXd sailFlat;

	// Loop on the wind velocities
	for(size_t iWv=0; iWv<pResults_->windVelocitySize(); iWv++) {

		// Get the number of valid results for this velocity : all minus discarded
		size_t numValidResults= pResults_->windAngleSize() - pResults_->getNumDiscardedResultsForVelocity(iWv);

		// Resize the result container for the number of valid results
		windAngles.resize(numValidResults);
		boatVelocity.resize(numValidResults);
		boatHeel.resize(numValidResults);
		crewB.resize(numValidResults);
		sailFlat.resize(numValidResults);

		// Store the wind velocity as a label for this curve
		char windVelocityLabel[256];
		sprintf(windVelocityLabel,"%3.1f", pResults_->get(iWv,0).getTWV() );
		string wVLabel(windVelocityLabel);

		// Define a tmp variable
		size_t idx=0;

		// Loop on the wind angles
		for(size_t iWa=0; iWa<pResults_->windAngleSize(); iWa++) {

			if(!pResults_->get(iWv,iWa).discard()){

				// Fill the list of wind angles
				windAngles(idx) = mathUtils::toDeg(pResults_->get(iWv,iWa).getTWA());

				// fill the list of boat speeds to an ArrayXd
				boatVelocity(idx) = pResults_->get(iWv,iWa).getX()->coeff(0);

				// fill the list of boat heel to an ArrayXd
				boatHeel(idx) = mathUtils::toDeg(pResults_->get(iWv,iWa).getX()->coeff(1));

				// fill the list of Crew B to an ArrayXd
				crewB(idx) = pResults_->get(iWv,iWa).getX()->coeff(2);

				// fill the list of Sail flat to an ArrayXd
				sailFlat(idx) = pResults_->get(iWv,iWa).getX()->coeff(3);

				// Increment counter
				idx++;

			}
		}

		// Append the angles-data to the relevant plotter
		boatSpeedPolarPlotter.append(wVLabel,windAngles,boatVelocity);
		boatHeelPolarPlotter.append(wVLabel,windAngles,boatHeel);
		crewBPolarPlotter.append(wVLabel,windAngles,crewB);
		sailFlatPolarPlotter.append(wVLabel,windAngles,sailFlat);

	}

	// Ask all plotters to plot
	boatSpeedPolarPlotter.plot();
	boatHeelPolarPlotter.plot(5);
	crewBPolarPlotter.plot();
	sailFlatPolarPlotter.plot();

}

