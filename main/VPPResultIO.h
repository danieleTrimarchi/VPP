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
		VPPResultIO(ResultContainer* pResults);

		/// Dtor
		~VPPResultIO();

		/// Write results to file
		void write(string fileName=string("vppResults.vpp"));

		/// Read results from file
		void read(string fileName=string("vppResults.vpp"));

	private:

		/// Ptr to the result container
		ResultContainer* pResults_;

};

#endif
