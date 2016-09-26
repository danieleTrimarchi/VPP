#include "VPPResultIO.h"
#include "VPPException.h"
#include <fstream>

// Ctor
VPPResultIO::VPPResultIO(ResultContainer* pResults):
pResults_(pResults) {

}

// Dtor
VPPResultIO::~VPPResultIO(){

}

// Write results to file
void VPPResultIO::write(string fileName){

	// Open a file
	FILE* outFile= fopen(fileName.c_str(), "w");

	// Ask the results to print to the file
	pResults_->print(outFile);

}

// Read results from file
void VPPResultIO::read(string fileName){

	// Verify the extension of the file (.vpp)
	// -> throw if the extension is not correct

	// Clear the results
	pResults_->clear();

	// Get the file as an ifstream
	std::ifstream infile(fileName.c_str());
	if(!infile.good())
		throw VPPException(HERE, "Variable file not found!");

	std::string line;
	while(std::getline(infile,line)){

		// Searches for the comment char (%) in this file and erase from there
		size_t comment = line.find("%");
		if(comment != std::string::npos) {
			// Erase the string from the comment onward
			line.erase(line.begin()+comment, line.end());
		}

		// if the string is not empty, attempt reading the variable
		if(!line.empty()){

			// Use stringstream to read the name of the variable and its value
			std::stringstream ss(line);

			// Get the values from the formatted line
			size_t itwv, itwa;
			double twv, twa, v, phi, b, f, df, dm;
			int discard;
			if (sscanf(line.c_str(),"%zu %le %zu %le -- %le %le %le %le -- %le %le -- %d",
					                     &itwv, &twv, &itwa, &twa, &v, &phi, &b, &f, &df, &dm, &discard) == 11 ){

				// Push the result to the stack
				pResults_->push_back(	itwv, itwa, v, phi, b, f, df, dm);

				// Set the parameter discard
				if(discard)
					pResults_->remove(itwv,itwa);

				// Debug: print to screen the Result just read
				pResults_->get(itwv,itwa).print();

			} else
					throw VPPException(HERE,"Unknown format!");
		}
	}
}
