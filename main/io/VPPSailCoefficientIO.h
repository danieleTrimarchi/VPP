#ifndef VPPSAILCOEFFICIENT_IO_H
#define VPPSAILCOEFFICIENT_IO_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "Eigen/Core"

#include "VariableFileParser.h"

using namespace std;

/// File parser able to read and store the sail coeffs
/// read from file to a container set (abstract class)
class VPPSailCoefficientIO : public FileParserBase {

	public:

		/// Constructor
		VPPSailCoefficientIO();

		/// Destructor
		virtual ~VPPSailCoefficientIO();

		/// Check that all the required variables have been
		/// prompted into the file. Otherwise throws
		virtual void check();

		/// Printout the list of all the sail coeffs we have collected
		void print(FILE* outStream=stdout);

		/// Get the coefficient matrix.
		const Eigen::ArrayXXd* getCoefficientMatrix() const;

	protected:

		/// Implement the pure virtual : do all is required before
		/// starting the parse (init)
		virtual size_t preParse();

		/// Implement pure virtual: get the identifier for the beginning
		/// of a file section
		virtual const string getHeaderBegin() const =0;

		/// Implement pure virtual: get the identifier for the end of a
		/// file section
		virtual const string getHeaderEnd() const =0;

		/// Assign the value of the initial sail coeffs to the
		/// current coeffs_ matrix
		virtual void restoreDefaultCoefficients() =0;

		/// Each subclass implement its own method to do something
		/// out of this stream
		virtual void parseLine(string&);

		/// Dynamically resizable container used to store the
		/// values of the sail coefficients
		Eigen::ArrayXXd coeffs_;

};

///---------------------------------------------------------------------

/// Class used to define and store the sail LIFT coefficients
/// The class is able to read the coefficients from a formatted
/// file, where the relevant section of the file is identified by
/// a specific header begin and end
class VPP_CL_IO : public VPPSailCoefficientIO {

	public:

		/// Ctor
		VPP_CL_IO(VariableFileParser*);

		/// Dtor
		~VPP_CL_IO();

	protected:

		/// Implement pure virtual: get the identifier for the beginning
		/// of a file section
		virtual const string getHeaderBegin() const;

		/// Implement pure virtual: get the identifier for the end of a
		/// file section
		virtual const string getHeaderEnd() const;

		/// Implement pure virtual: Assign the value of the initial
		/// sail coeffs to the current coeffs_ matrix
		virtual void restoreDefaultCoefficients();

	private:

		/// Use sail coefficients for a full batten sail
		bool fullBattens_;

		/// This arrays store the default sail coeffs
		/// They are sent over to the VPPAeroItem if the
		/// user has not defined custom coefficients
		/// via file. clMat0_FB stands for 'Full Battens'
		Eigen::Array<double,9,4> clMat0_;

};

///---------------------------------------------------------------------

/// Class used to define and store the sail DRAG coefficients
/// The class is able to read the coefficients from a formatted
/// file, where the relevant section of the file is identified by
/// a specific header begin and end
class VPP_CD_IO : public VPPSailCoefficientIO {

	public:

		/// Ctor
		VPP_CD_IO();

		/// Dtor
		~VPP_CD_IO();

	protected:

		/// Implement pure virtual: get the identifier for the beginning
		/// of a file section
		virtual const string getHeaderBegin() const;

		/// Implement pure virtual: get the identifier for the end of a
		/// file section
		virtual const string getHeaderEnd() const;

		/// Implement pure virtual: Assign the value of the initial
		/// sail coeffs to the current coeffs_ matrix
		virtual void restoreDefaultCoefficients();

	private:

		/// This arrays store the default sail coeffs
		/// They are sent over to the VPPAeroItem if the
		/// user has not defined custom coefficients
		/// via file. clMat0_FB stands for 'Full Battens'
		Eigen::Array<double,8,4> cdMat0_;

};


#endif

