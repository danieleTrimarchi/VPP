#ifndef VPP_RESULT_IO_H
#define VPP_RESULT_IO_H

#include <stdio.h>
#include <iostream>
#include "string.h"
#include "Results.h"

using namespace std;

/// Class used to read and write VPP results
/// from and to file. Establish the file format .vpp
class VPPResultIO : public FileParserBase {

	public:

		/// Ctor
		VPPResultIO(VariableFileParser* pParser, ResultContainer* pResults);

		/// Dtor
		~VPPResultIO();

		/// Write results to file
		void write(string fileName=string("vppResults.vpp"));

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	protected:

		/// Implement pure virtual : do all is required before
		/// starting the parse (init)
		virtual void preParse();

		/// Implement pure virtual : get the identifier for the
		/// beginning of a file section
		virtual const string getHeaderBegin() const;

		/// Implement pure virtual : get the identifier for the end
		/// of a file section
		virtual const string getHeaderEnd() const;

		/// Implement pure virtual : each subclass implement its own
		/// method to do something out of this stream
		virtual void parseLine(std::string&);

		/// Implement pure virtual : check that all the required entries
		/// have been prompted into the file. Otherwise throw
		virtual void check();

	private:

		/// Once the result section has been identified, parse it
		/// until the end. Exit when the section terminator is found
		void parseSection(std::ifstream& infile);

		/// Ptr to the parser that knows all of the variables
		VariableFileParser* pParser_;

		/// Ptr to the result container
		ResultContainer* pResults_;

};

#endif
