#ifndef VPP_COMMAND_H
#define VPP_COMMAND_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include "VPPItemFactory.h"
#include "VPPSolverBase.h"

/// Base class for all commands that can be issued from the terminal
/// The Command mechanism allows redoing the latest command with slighly
/// modified arguments
class VPPCommand {

	public:

		/// Ctor
		VPPCommand();

		/// Copy constructor
		VPPCommand(const VPPCommand& );

		/// Dtor
		~VPPCommand();

};

////////////////////////////////////////////////////////////////

class PrintVariablesCommand : public VPPCommand {

	public:

		/// Ctor
		PrintVariablesCommand(VariableFileParser*,SailSet*);

		/// Copy constructor
		PrintVariablesCommand(const PrintVariablesCommand& );

		/// Dtor
		~PrintVariablesCommand();

	private:

		/// Ptr to the variableFileParser
		VariableFileParser* pParser_;

		/// Ptr to the sailSet
		SailSet* pSails_;

};

////////////////////////////////////////////////////////////////

class PlotVPPCommand : public VPPCommand {

	public:

		/// Ctor
		PlotVPPCommand( VPPItemFactory*);

		/// Copy constructor
		PlotVPPCommand( const PlotVPPCommand& );

		/// Dtor
		~PlotVPPCommand();

	protected:

		/// Disallow default Ctor
		PlotVPPCommand();

		/// Ptr to the VPP items
		VPPItemFactory* pVppItems_;

};

////////////////////////////////////////////////////////////////

class PlotInterpolatedCoefficientsCommand : public PlotVPPCommand {

	public:

		/// Ctor
		PlotInterpolatedCoefficientsCommand( VPPItemFactory*);

		/// Dtor
		~PlotInterpolatedCoefficientsCommand();

	private:

		/// Disallow default Ctor
		PlotInterpolatedCoefficientsCommand();

};

////////////////////////////////////////////////////////////////

class plot_D_InterpolatedCoefficientsCommand : public PlotVPPCommand {

	public:

		/// Ctor
		plot_D_InterpolatedCoefficientsCommand( VPPItemFactory*);

		/// Dtor
		~plot_D_InterpolatedCoefficientsCommand();

	private:

		/// Disallow default Ctor
		plot_D_InterpolatedCoefficientsCommand();

};

////////////////////////////////////////////////////////////////

class plot_D2_InterpolatedCoefficientsCommand : public PlotVPPCommand {

	public:

		/// Ctor
		plot_D2_InterpolatedCoefficientsCommand( VPPItemFactory*);

		/// Dtor
		~plot_D2_InterpolatedCoefficientsCommand();

	private:

		/// Disallow default Ctor
		plot_D2_InterpolatedCoefficientsCommand();

};

////////////////////////////////////////////////////////////////

class plotSailForceMomentCommand : public PlotVPPCommand {

	public:

		/// Ctor
		plotSailForceMomentCommand( VPPItemFactory*);

		/// Dtor
		~plotSailForceMomentCommand();

	private:

		/// Disallow default Ctor
		plotSailForceMomentCommand();

};

////////////////////////////////////////////////////////////////

class plotResistanceCommand : public PlotVPPCommand {

	public:

		enum {
			totalResistance,
			frictionalResistance,
			residuaryResistance,
			inducedResistance,
			delta_frictionalResistance_heel,
			delta_residualResistance_heel,
			frictionalResistance_keel,
			frictionalResistance_rudder,
			residuaryResistance_keel,
			negativeResistance
		};

		/// Ctor
		plotResistanceCommand(int,VPPItemFactory*);

		/// Dtor
		~plotResistanceCommand();

	private:

		/// Disallow default Ctor
		plotResistanceCommand();

		int resType_;
};

////////////////////////////////////////////////////////////////

class plotPolarsCommand : public PlotVPPCommand {

	public:

		/// Ctor
		plotPolarsCommand( VPPItemFactory*, VPPSolverBase*);

		/// Dtor
		~plotPolarsCommand();

	private:

		/// Disallow default Ctor
		plotPolarsCommand();

		/// Pointer to the solver
		VPPSolverBase* pSolver_;

};

////////////////////////////////////////////////////////////////

class plotXYCommand : public PlotVPPCommand {

	public:

		/// Ctor
		plotXYCommand( VPPItemFactory*, VPPSolverBase*);

		/// Dtor
		~plotXYCommand();

	private:

		/// Disallow default Ctor
		plotXYCommand();

		/// Pointer to the solver
		VPPSolverBase* pSolver_;

};

////////////////////////////////////////////////////////////////

class plotOptimizationSpaceCommand : public PlotVPPCommand {

	public:

		/// Ctor
		plotOptimizationSpaceCommand( VPPItemFactory*);

		/// Dtor
		~plotOptimizationSpaceCommand();

	private:

		/// Disallow default Ctor
		plotOptimizationSpaceCommand();

};

////////////////////////////////////////////////////////////////

class PlotJacobianCommand : public PlotVPPCommand {

	public:

		/// Ctor
		PlotJacobianCommand( VPPItemFactory*, VPPSolverBase* );

		/// Dtor
		~PlotJacobianCommand();

	private:

		/// Disallow default Ctor
		PlotJacobianCommand();

		/// Pointer to the solver
		VPPSolverBase* pSolver_;

};

////////////////////////////////////////////////////////////////



#endif
