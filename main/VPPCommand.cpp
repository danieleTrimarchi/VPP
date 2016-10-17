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

// Implement pure virtual : redo the command with modified specs
void PrintVariablesCommand::redo() {

	// Just redo the action defined in the constructor
	pParser_->printVariables();
	pSails_->printVariables();

}

//////////////////////////////////////

// Ctor
PlotVPPCommand::PlotVPPCommand( VPPItemFactory* pVPPItems):
						pVppItems_(pVPPItems) {

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

// Implement the pure virtual : re-plot asking the user new bound.
void PlotInterpolatedCoefficientsCommand::redo() {

	// Instantiate some IOUtils
	IOUtils io(pVppItems_->getWind());

	// Ask the user the angles the plot should be bounded with
	double angle0= io.askUserDouble("  Please enter the value of angle0");
	double angleEnd= io.askUserDouble("  Please enter the value of angleEnd");

	// Execute the command with non-default arguments
	pVppItems_->getSailCoefficientItem()->plotInterpolatedCoefficients(angle0,angleEnd);

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

// Implement the pure virtual : re-plot asking the user new bound.
void plot_D_InterpolatedCoefficientsCommand::redo() {

	// Instantiate some IOUtils
	IOUtils io(pVppItems_->getWind());

	// Ask the user the angles the plot should be bounded with
	double angle0= io.askUserDouble("  Please enter the value of angle0");
	double angleEnd= io.askUserDouble("  Please enter the value of angleEnd");

	// Execute the command with non-default arguments
	pVppItems_->getSailCoefficientItem()->plot_D_InterpolatedCoefficients(angle0,angleEnd);

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

// Implement the pure virtual : re-plot asking the user new bound.
void plot_D2_InterpolatedCoefficientsCommand::redo() {

	// Instantiate some IOUtils
	IOUtils io(pVppItems_->getWind());

	// Ask the user the angles the plot should be bounded with
	double angle0= io.askUserDouble("  Please enter the value of angle0");
	double angleEnd= io.askUserDouble("  Please enter the value of angleEnd");

	// Execute the command with non-default arguments
	pVppItems_->getSailCoefficientItem()->plot_D2_InterpolatedCoefficients(angle0,angleEnd);

}
//////////////////////////////////////

// Ctor
plotSailForceMomentCommand::plotSailForceMomentCommand(
		VPPItemFactory* pVPPItems ):
								PlotVPPCommand(pVPPItems),
								pParser_(pVppItems_->getParser()),
								pWindItem_(pVppItems_->getWind()) {

	// Number of points of the plots
	size_t nVelocities=60;

	// For which TWV, TWA shall we plot the aero forces/moments?
	size_t twv=0, twa=0;
	IOUtils io(pVppItems_->getWind());
	io.askUserWindIndexes(twv, twa);

	// Declare containers for the velocity and angle-wise data
	std::vector<ArrayXd> v, Lift, Drag, fDrive, fSide, mHeel;
	std::vector<string> curveLabels;

	// Loop on heel angles : from 0 to 90 deg in steps of 15 deg
	for(size_t hAngle=0; hAngle<90; hAngle+=15){

		// Convert the heeling angle into radians
		double phi= toRad(hAngle);

		// vectors with the current boat velocity, the drive force
		// and heeling moment values
		ArrayXd x_v, lift, drag, f_v, fs_v, m_v;
		x_v.resize(nVelocities);
		lift.resize(nVelocities);
		drag.resize(nVelocities);
		f_v.resize(nVelocities);
		fs_v.resize(nVelocities);
		m_v.resize(nVelocities);

		// Now fix the value of b_ and f_
		std::cout<<"Warning, setting b=0.01 and f=0.99 for the plot"<<std::endl;
		double b= 0.01;
		double f= 0.99;

		// loop on the boat velocity, from 0.1 to 5m/s in step on 1 m/s
		for(size_t iTwv=0; iTwv<nVelocities; iTwv++ ) {

			// Set the value for the state variable boat velocity
			// Linearly from -1 to 1 m/s
			double v = -1 + double(iTwv) / (nVelocities-1) * 2;

			VectorXd stateVector(4);
			stateVector << v,phi,b,f;

			pVppItems_->getAeroForcesItem()->updateSolution(twv,twa,stateVector);

			// Update the sail coefficients for the current wind
			pVppItems_->getSailCoefficientItem()->updateSolution(twv, twa, stateVector);

			// Update the aero forces
			pVppItems_->getAeroForcesItem()->updateSolution(twv, twa, stateVector);

			// Store velocity-wise data:
			x_v(iTwv)= v / sqrt( Physic::g * pParser_->get("LWL") );	// Fn...
			lift(iTwv) = pVppItems_->getAeroForcesItem()->getLift(); // lift...
			drag(iTwv) = pVppItems_->getAeroForcesItem()->getDrag(); // drag...
			f_v(iTwv)= pVppItems_->getAeroForcesItem()->getFDrive(); // fDrive...
			fs_v(iTwv)= pVppItems_->getAeroForcesItem()->getFSide(); // fSide_...
			m_v(iTwv)= pVppItems_->getAeroForcesItem()->getMHeel();  // mHeel...

		}

		// Append the velocity-wise curve for each heel angle
		v.push_back(x_v);
		Lift.push_back(lift);
		Drag.push_back(drag);
		fDrive.push_back(f_v);
		fSide.push_back(fs_v);
		mHeel.push_back(m_v);

		char msg[256];
		sprintf(msg,"heel=%d deg",hAngle);
		curveLabels.push_back(msg);
	}

	// Instantiate a plotter and plot Lift
	VPPPlotter liftPlotter;
	for(size_t i=0; i<v.size(); i++)
		liftPlotter.append(curveLabels[i],v[i],Lift[i]);

	char msg[256];
	sprintf(msg,"plot Sail Lift vs boat speed - "
			"twv=%2.2f [m/s], twa=%2.2f [deg]",
			pWindItem_->getTWV(twv),
			mathUtils::toDeg(pWindItem_->getTWA(twa)) );
	liftPlotter.plot("Fn [-]","Lift [N]", msg);


	// Instantiate a plotter and plot Drag
	VPPPlotter dragPlotter;
	for(size_t i=0; i<v.size(); i++)
		dragPlotter.append(curveLabels[i],v[i],Drag[i]);

	sprintf(msg,"plot Sail Drag vs boat speed - "
			"twv=%2.2f [m/s], twa=%2.2f [deg]",
			pWindItem_->getTWV(twv),
			mathUtils::toDeg(pWindItem_->getTWA(twa)) );
	dragPlotter.plot("Fn [-]","Drag [N]", msg);

	// Instantiate a plotter and plot fDrive
	VPPPlotter fPlotter;
	for(size_t i=0; i<v.size(); i++)
		fPlotter.append(curveLabels[i],v[i],fDrive[i]);

	sprintf(msg,"plot drive force vs boat speed - "
			"twv=%2.2f [m/s], twa=%2.2f [deg]",
			pWindItem_->getTWV(twv),
			mathUtils::toDeg(pWindItem_->getTWA(twa)) );
	fPlotter.plot("Fn [-]","Fdrive [N]", msg);

	// Instantiate a plotter and plot fSide
	VPPPlotter fSidePlotter;
	for(size_t i=0; i<v.size(); i++)
		fSidePlotter.append(curveLabels[i],v[i],fSide[i]);

	sprintf(msg,"plot side force vs boat speed - "
			"twv=%2.2f [m/s], twa=%2.2f [deg]",
			pWindItem_->getTWV(twv),
			mathUtils::toDeg(pWindItem_->getTWA(twa)) );
	fSidePlotter.plot("Fn [-]","Fside [N]", msg);

	// Instantiate a plotter and plot mHeel
	VPPPlotter mPlotter;
	for(size_t i=0; i<v.size(); i++)
		mPlotter.append(curveLabels[i],v[i],mHeel[i]);
	mPlotter.plot("Fn [-]","mHeel [N*m]","plot heeling moment vs boat speed");

}


// Dtor
plotSailForceMomentCommand::~plotSailForceMomentCommand() {
	/* do nothing */
}

// Disallow default Ctor
plotSailForceMomentCommand::plotSailForceMomentCommand() :
				PlotVPPCommand(),
				pParser_(0),
				pWindItem_(0){

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

// Implement the pure virtual : re-plot asking the user new bounds
void plotResistanceCommand::redo() {

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


