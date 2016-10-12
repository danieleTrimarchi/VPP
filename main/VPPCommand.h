#ifndef VPP_COMMAND_H
#define VPP_COMMAND_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include "VPPItemFactory.h"

/// Base class for all commands that can be issued from the terminal
/// The Command mechanism allows redoing the latest command with slighly
/// modified arguments
class VPPCommand {

	public:

		/// Ctor
		VPPCommand(const VPPItemFactory*);

		/// Copy constructor
		implementit!

		/// Dtor
		~VPPCommand();

	protected:

		/// Disallow default Ctor
		VPPCommand();

	private:

		/// Ptr to the VPP items
		const VPPItemFactory* pVPPItems_;
};


class PlotInterpolatedCoefficientsCommand : public VPPCommand {

	public:

		/// Ctor
		PlotInterpolatedCoefficientsCommand(const VPPItemFactory*);

		/// Dtor
		~PlotInterpolatedCoefficientsCommand();

	private:

		/// Disallow default Ctor
		PlotInterpolatedCoefficientsCommand();

};


#endif
