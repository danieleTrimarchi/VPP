#include "VPPException.h"
#include <iostream>
#include <sstream>

// Constructor
VPPException::VPPException(const char* inFile, int inLine, const char* inFunction, const char* message ){

	std::ostringstream oss;
	oss<<"Error in function: "<<inFunction<<"\n in file: "<<inFile<<" line: "<<inLine<<std::endl;
	oss<<" Message: "<<message<<std::endl;
	msg=oss.str();
	std::cout<<msg<<std::endl;
}

// Destructor
VPPException::~VPPException() throw() {

}

const char* VPPException::what() const throw() {
	return msg.c_str();
}

////////////////////

// Constructor
NonConvergedException::NonConvergedException(const char* inFile, int inLine, const char* inFunction, const char* message ){

	std::ostringstream oss;
	oss<<"Error in function: "<<inFunction<<"\n in file: "<<inFile<<" line: "<<inLine<<std::endl;
	oss<<" Message: "<<message<<std::endl;
	msg=oss.str();
	std::cout<<msg<<std::endl;
}

// Destructor
NonConvergedException::~NonConvergedException() throw() {

}

const char* NonConvergedException::what() const throw() {
	return msg.c_str();
}


