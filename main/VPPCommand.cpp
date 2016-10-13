#include "VPPCommand.h"

// Ctor
VPPCommand::VPPCommand() {

}

// Copy constructor
VPPCommand::VPPCommand(const VPPCommand& rhs) {
	/* do nothing as there is nothing to do */
}

// Dtor
VPPCommand::~VPPCommand(){

}

//////////////////////////////////////

// Ctor
PrintVariablesCommand::PrintVariablesCommand(
		VariableFileParser* pParser,SailSet* pSails):
				pParser_(pParser),
				pSails_(pSails) {

	// Actually execute the command on construction
	pParser_->printVariables();
	pSails_->printVariables();

}

// Copy constructor
PrintVariablesCommand::PrintVariablesCommand(
		const PrintVariablesCommand& rhs):
				pParser_(rhs.pParser_),
				pSails_(rhs.pSails_) {

}

// Dtor
PrintVariablesCommand::~PrintVariablesCommand(){
	/* do nothing */
}



//////////////////////////////////////

// Ctor
PlotVPPCommand::PlotVPPCommand( VPPItemFactory* pVPPItems):
				pVppItems_(pVPPItems){

}

// Copy constructor
PlotVPPCommand::PlotVPPCommand(const PlotVPPCommand& rhs):
								pVppItems_(rhs.pVppItems_){

}

// Dtor
PlotVPPCommand::~PlotVPPCommand(){

}


// Disallow default Ctor
PlotVPPCommand::PlotVPPCommand():
								pVppItems_(0){

}


//////////////////////////////////////

// Ctor
PlotInterpolatedCoefficientsCommand::PlotInterpolatedCoefficientsCommand(
		VPPItemFactory* pVPPItems):
						PlotVPPCommand(pVPPItems) {

	// Actually execute the command on construction
	pVppItems_->getSailCoefficientItem()->plotInterpolatedCoefficients();

}

// Dtor
PlotInterpolatedCoefficientsCommand::~PlotInterpolatedCoefficientsCommand() {
	/* do nothing */
}


// Disallow default Ctor
PlotInterpolatedCoefficientsCommand::PlotInterpolatedCoefficientsCommand() :
		PlotVPPCommand() {

}

//////////////////////////////////////

// Ctor
plot_D_InterpolatedCoefficientsCommand::plot_D_InterpolatedCoefficientsCommand(
		VPPItemFactory* pVPPItems ):
						PlotVPPCommand(pVPPItems) {

	// Actually execute the command on construction
	pVppItems_->getSailCoefficientItem()->plot_D_InterpolatedCoefficients();

}

// Dtor
plot_D_InterpolatedCoefficientsCommand::~plot_D_InterpolatedCoefficientsCommand() {
	/* do nothing */
}

// Disallow default Ctor
plot_D_InterpolatedCoefficientsCommand::plot_D_InterpolatedCoefficientsCommand() :
		PlotVPPCommand() {

}

//////////////////////////////////////

// Ctor
plot_D2_InterpolatedCoefficientsCommand::plot_D2_InterpolatedCoefficientsCommand(
		VPPItemFactory* pVPPItems ):
						PlotVPPCommand(pVPPItems) {

	// Actually execute the command on construction
	pVppItems_->getSailCoefficientItem()->plot_D2_InterpolatedCoefficients();
}

// Dtor
plot_D2_InterpolatedCoefficientsCommand::~plot_D2_InterpolatedCoefficientsCommand() {
	/* do nothing */
}

// Disallow default Ctor
plot_D2_InterpolatedCoefficientsCommand::plot_D2_InterpolatedCoefficientsCommand() :
		PlotVPPCommand() {

}

//////////////////////////////////////

// Ctor
plotSailForceMomentCommand::plotSailForceMomentCommand(
		VPPItemFactory* pVPPItems ):
						PlotVPPCommand(pVPPItems) {

	// Actually execute the command on construction
	pVppItems_->getAeroForcesItem()->plot();
}

// Dtor
plotSailForceMomentCommand::~plotSailForceMomentCommand() {
	/* do nothing */
}

// Disallow default Ctor
plotSailForceMomentCommand::plotSailForceMomentCommand() :
		PlotVPPCommand() {

}

//////////////////////////////////////

// Ctor
plotResistanceCommand::plotResistanceCommand(
		int resType,
		VPPItemFactory* pVPPItems ):
				resType_(resType),
				PlotVPPCommand(pVPPItems) {

	// Actually execute the command on construction
	switch( resType_ )
	{
		case totalResistance :
			pVppItems_->plotTotalResistance();
			break;

		case frictionalResistance :
			pVppItems_->getFrictionalResistanceItem()->plot();
			break;

		case residuaryResistance:
			pVppItems_->getResiduaryResistanceItem()->plot(pVppItems_->getWind());
			break;

		case inducedResistance:
			pVppItems_->getInducedResistanceItem()->plot();
			break;

		case delta_frictionalResistance_heel:
			pVppItems_->getDelta_FrictionalResistance_HeelItem()->plot();
			break;

		case delta_residualResistance_heel:
			pVppItems_->getDelta_ResiduaryResistance_HeelItem()->plot(pVppItems_->getWind());
			break;

		case frictionalResistance_keel:
			pVppItems_->getViscousResistanceKeelItem()->plot(); //-> this does not plot

			break;

		case frictionalResistance_rudder:
			pVppItems_->getViscousResistanceRudderItem()->plot(); //-> this does not plot
			break;

		case residuaryResistance_keel:
			pVppItems_->getResiduaryResistanceKeelItem()->plot();
			break;

		case negativeResistance:
			pVppItems_->getNegativeResistanceItem()->plot();
			break;

		default:
			char msg[256];
			sprintf(msg,"The value of resType: \"%d\" is not supported",resType_);
			throw std::logic_error(msg);
		}


}

// Dtor
plotResistanceCommand::~plotResistanceCommand() {
	/* do nothing */
}

// Disallow default Ctor
plotResistanceCommand::plotResistanceCommand() :
		PlotVPPCommand(),
		resType_(totalResistance){

}

//////////////////////////////////////

// Ctor
plotOptimizationSpaceCommand::plotOptimizationSpaceCommand( VPPItemFactory* pVPPItems):
						PlotVPPCommand(pVPPItems) {
	// Actually execute the command on construction
	pVppItems_->plotOptimizationSpace();

}

// Dtor
plotOptimizationSpaceCommand::~plotOptimizationSpaceCommand() {
	/* do nothing */
}


// Disallow default Ctor
plotOptimizationSpaceCommand::plotOptimizationSpaceCommand() {
	/* do nothing */
}

//////////////////////////////////////

// Ctor
plotPolarsCommand::plotPolarsCommand( VPPItemFactory* pVPPItems, VPPSolverBase* pSolver):
						PlotVPPCommand(pVPPItems),
						pSolver_(pSolver) {

	// Actually execute the command on construction
	pSolver_->plotPolars();

}

// Dtor
plotPolarsCommand::~plotPolarsCommand() {
	/* do nothing */
}


// Disallow default Ctor
plotPolarsCommand::plotPolarsCommand() :
	PlotVPPCommand(),
		pSolver_(0) {
	/* do nothing */
}

//////////////////////////////////////

// Ctor
plotXYCommand::plotXYCommand( VPPItemFactory* pVPPItems, VPPSolverBase* pSolver):
						PlotVPPCommand(pVPPItems),
						pSolver_(pSolver) {

	// Actually execute the command on construction
	std::cout<<"Please enter the index of the wind angle: \n";
	int idx;
	cin >> idx;
	pSolver_->plotXY(idx);

}

// Dtor
plotXYCommand::~plotXYCommand() {
	/* do nothing */
}


// Disallow default Ctor
plotXYCommand::plotXYCommand() :
	PlotVPPCommand(),
		pSolver_(0) {
	/* do nothing */
}


//////////////////////////////////////

// Ctor
PlotJacobianCommand::PlotJacobianCommand(
		VPPItemFactory* pVPPItems, VPPSolverBase* pSolver ):
						PlotVPPCommand(pVPPItems),
						pSolver_(pSolver) {

	// Actually execute the command on construction
	pSolver_->plotJacobian();
}

// Dtor
PlotJacobianCommand::~PlotJacobianCommand() {
	/* do nothing */
}

// Disallow default Ctor
PlotJacobianCommand::PlotJacobianCommand() :
		PlotVPPCommand(),
			pSolver_(0) {

}

//////////////////////////////////////


