#include "VPPException.h"
#include <iostream>
#include <sstream>

// Constructor
VPPException::VPPException(const char* inFile, int inLine, const char* inFunction, const char* message ){

	std::ostringstream oss;
	oss<<"Error in function: "<<inFunction<<"\nin file: "<<inFile<<" line: "<<inLine<<std::endl;
	oss<<"Message: "<<message<<std::endl;
	msg=oss.str();
	std::cout<<msg<<std::endl;
}

// Destructor
VPPException::~VPPException() {

}

const char* VPPException::what() const {
	return msg.c_str();
}
