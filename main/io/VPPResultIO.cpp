#include "VPPResultIO.h"
#include "VPPException.h"
#include <fstream>

// Ctor
VPPResultIO::VPPResultIO(VariableFileParser* pParser, ResultContainer* pResults):
	FileParserBase("vppResults.vpp"),
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

// Implement pure virtual : do all is required before
// starting the parse (init)
void VPPResultIO::preParse() {
	// Clear the results before writing
	pResults_->initResultMatrix();
}

// Implement pure virtual : get the identifier for the
// beginning of a file section
const string VPPResultIO::getHeaderBegin() const {
	return Result::headerBegin_;
}

// Implement pure virtual : get the identifier for the end
// of a file section
const string VPPResultIO::getHeaderEnd() const {
	return Result::headerEnd_;
}

// Implement pure virtual : each subclass implement its own
// method to do something out of this stream
void VPPResultIO::parseLine(std::string& line) {

	// Get the values from the formatted line
	size_t itwv, itwa;
	double twv, twa, v, phi, b, f, df, dm;
	int discard;

	// Does this line contain a result..?
	if( sscanf(line.c_str(),"%zu %le %zu %le -- %le %le %le %le -- %le %le -- %d",
				                     &itwv, &twv, &itwa, &twa, &v, &phi, &b, &f, &df, &dm, &discard) != 11 )
		return;

	// Push the result to the stack
	pResults_->push_back( itwv, itwa, v, phi, b, f, df, dm);

	// Set the parameter discard
	if(discard)
		pResults_->remove(itwv,itwa);

	// Debug: print to screen the Result we just read
	// pResults_->get(itwv,itwa).print();

}

// Implement pure virtual : check that all the required entries
// have been prompted into the file. Otherwise throw
void VPPResultIO::check() {
 /* Make nothing */
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

	}
}
