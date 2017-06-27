#include "VPPResultIO.h"
#include "VPPException.h"
#include <fstream>

// Ctor
VPPResultIO::VPPResultIO(VariableFileParser* pParser, ResultContainer* pResults):
	pParser_(pParser),
	pResults_(pResults) {

}

// Dtor
VPPResultIO::~VPPResultIO(){

}

// Write results to file
void VPPResultIO::write(string fileName/*=string("vppResults.vpp")*/){

	std::cout<<"Saving the analysis results to file "<<fileName<<std::endl;

	// Open a file
	FILE* outFile= fopen(fileName.c_str(), "w");

	// Ask the parser to print the variables to the file
	pParser_->print(outFile);

	// Ask the results to print to the file
	pResults_->print(outFile);

	// close the file and make sure the buffer has been flushed
	fclose(outFile);

}


// Read results from file. The result file is formatted output, it has the
// variables : format "%s : %8.6f
// results   : format "%zu %le %zu %le -- %le %le %le %le -- %le %le -- %d"
// We attempt to read both
void VPPResultIO::parse(string fileName){

	// Get the file as an ifstream
	std::ifstream infile(fileName.c_str());
	if(!infile.good()){
		char msg[256];
		sprintf(msg,"Result file: %s  not found!", fileName.c_str());
		throw VPPException(HERE, msg);
	}

	// Clear the results before writing
	pResults_->initResultMatrix();

	std::string line;
	while(std::getline(infile,line)){

		std::cout<<"Got line: "<<line<<std::endl;
		if(line==Result::headerBegin_){
			std::cout<<"Line IS headerBegin!"<<std::endl;
			parseSection(infile);
			break;
		}
	}
}

void VPPResultIO::parseSection(std::ifstream& infile){

	std::string line;
	while(std::getline(infile,line)){

		// printout the line we have read
		//std::cout<<" Original line = "<<line<<std::endl;

		// Keep reading while we find the end marker
		if(line==Result::headerEnd_)
			break;

		// Searches for the comment char (%) in this file and erase from there
		size_t comment = line.find("%");
		if(comment != std::string::npos) {
			// Erase the string from the comment onward
			line.erase(line.begin()+comment, line.end());
		}

		// Do nothing for empty lines
		if(line.empty())
			continue;

		// Use stringstream to read the name of the variable and its value
		std::stringstream ss(line);

		// Get the values from the formatted line
		size_t itwv, itwa;
		double twv, twa, v, phi, b, f, df, dm;
		int discard;

		// Does this line contain a result..?
		if( sscanf(line.c_str(),"%zu %le %zu %le -- %le %le %le %le -- %le %le -- %d",
					                     &itwv, &twv, &itwa, &twa, &v, &phi, &b, &f, &df, &dm, &discard) != 11 )
			break;

		// Push the result to the stack
		pResults_->push_back( itwv, itwa, v, phi, b, f, df, dm);

		// Set the parameter discard
		if(discard)
			pResults_->remove(itwv,itwa);

		// Debug: print to screen the Result we just read
		// pResults_->get(itwv,itwa).print();

	}
}
