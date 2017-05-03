#ifndef VPP_RESULT_IO_H
#define VPP_RESULT_IO_H

#include <stdio.h>
#include <iostream>
#include "string.h"
#include "Results.h"

using namespace std;

/// Class used to read and write VPP results
/// from and to file. Establish the file format .vpp
class VPPResultIO {

	public:

		/// Ctor
		VPPResultIO(VariableFileParser* pParser, ResultContainer* pResults);

		/// Dtor
		~VPPResultIO();

		/// Write results to file
		void write(string fileName=string("vppResults.vpp"));

		/// Read results from file
		void parse(string fileName=string("vppResults.vpp"));

		/// Declare the macro to allow for fixed size vector support
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

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
