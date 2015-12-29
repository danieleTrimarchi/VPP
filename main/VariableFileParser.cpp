#include "VariableFileParser.h"

// Constructor
VariableFileParser::VariableFileParser(std::string fileName) :
	fileName_(fileName) {
	// make nothing
}

// Destructor
VariableFileParser::~VariableFileParser() {
	// make nothing
}

// Parse the file
void VariableFileParser::parse() {

	// Get the file as an ifstream
	std::ifstream infile(fileName_.c_str());
	std::string line;
	while(std::getline(infile,line)){

		// printout the line we have read
		//std::cout<<" Original line = "<<line<<std::endl;

		// searches for the comment char (%) in this file and erase from there
		size_t comment = line.find("%");
		if(comment != std::string::npos) {
			// erase the string from the comment onward
			line.erase(line.begin()+comment, line.end());
		}
		//std::cout<<" Uncommented line= "<<line<<std::endl;

		// if the string is not empty, attempt reading the variable
		if(!line.empty()){

			// use stringstream to read the name of the variable and its value
			std::stringstream ss(line);

			variable newVariable;
			ss >> newVariable.varName_;
			ss >> newVariable.val_;
			std::cout<< "Read: "<<newVariable<<std::endl;

			variables_.insert(newVariable);
		}
	}



}

