#include "FileParserBase.h"
#include <stdio.h>
#include <cmath>
#include "Warning.h"
#include "VPPException.h"
#include "mathUtils.h"
#include "VariableTreeModel.h"

// Constructor
FileParserBase::FileParserBase(std::string fileName) :
	fileName_(fileName) {
	// make nothing
}

// Destructor
FileParserBase::~FileParserBase() {
	// make nothing
}

// Parse the file
void FileParserBase::parse(string fileName/*=""*/) {

	/// Do all is required before starting the parse (init)
	preParse();

	// If the arg string is not empty, modify fileName_
	if( fileName != "")
		fileName_=fileName;

	// Get the file as an ifstream
	std::ifstream infile(fileName_.c_str());
	if(!infile.good()){
		char msg[256];
		sprintf(msg,"Variable file: %s  not found!", fileName_.c_str());
		throw VPPException(HERE, msg);
	}

	std::string line;

	// Searches the init of the variable section
	while(std::getline(infile,line)){
		if( line==getHeaderBegin() ){
			parseSection(infile);
			break;
		}
	}
}

void FileParserBase::parseSection(std::ifstream& infile) {

	std::string line;
	while(std::getline(infile,line)){

		// printout the line we have read
		//std::cout<<"-- Original line = "<<line<<std::endl;

		// Keep reading while we find the end marker
		if( line==getHeaderEnd() )
			break;

			// Searches for the comment char (%) in this file and erase from there
		size_t comment = line.find("%");
		if(comment != std::string::npos) {
			// erase the string from the comment onward
			line.erase(line.begin()+comment, line.end());
		}
		//std::cout<<"-- Uncommented line= "<<line<<std::endl;

		// Do nothing for empty lines
		if(line.empty())
			continue;

		// If the string is not empty, parse it, generate
		// a variable and insert it to the VarSet
		parseLine(line);

	}

	// make sure we have all the entries we need and that
	// the values are reasonable. Implemented for each
	// derived class
	check();

	// Close the variable/result file
	infile.close();

}
