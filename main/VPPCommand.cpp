#include "VPPCommand.h"

// Ctor
VPPCommand::VPPCommand(const VPPItemFactory* pVPPItems):
pVPPItems_(pVPPItems){

}

// Dtor
VPPCommand::~VPPCommand(){

}


// Disallow default Ctor
VPPCommand::VPPCommand():
		pVPPItems_(0){

}

//////////////////////////////////////

// Ctor
PlotInterpolatedCoefficientsCommand::PlotInterpolatedCoefficientsCommand(
		const VPPItemFactory* pVPPItems):
			VPPCommand(pVPPItems) {

	// Actually execute the command on construction
	pVPPItems_->getSailCoefficientItem()->plotInterpolatedCoefficients();

}

// Dtor
PlotInterpolatedCoefficientsCommand::~PlotInterpolatedCoefficientsCommand() {
	/* do nothing */
}


// Disallow default Ctor
PlotInterpolatedCoefficientsCommand::PlotInterpolatedCoefficientsCommand() :
		VPPCommand() {

}

//////////////////////////////////////



