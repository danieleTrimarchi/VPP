#include "VPPSailCoefficientIO.h"
#include <stdio.h>
#include <cmath>
#include "Warning.h"
#include "VPPException.h"
#include "mathUtils.h"

// Constructor
VPPSailCoefficientIO::VPPSailCoefficientIO(std::string fileName) :
FileParserBase(fileName) {

}

// Destructor
VPPSailCoefficientIO::~VPPSailCoefficientIO() {
	// make nothing
}

// Implement the pure virtual : do all is required before
// starting the parse (init)
void VPPSailCoefficientIO::preParse() {

	// Clear the coefficient container
	coeffs_.resize(0,0);
}

// Each subclass implement its own method to do something
// out of this stream
void VPPSailCoefficientIO::parseLine(string& line) {

	if(line.empty())
		return;

	// Use stringstream to read the name of the variable and its value
	std::stringstream ss(line);

	coeffs_.conservativeResize(coeffs_.rows()+1, 4);

	ss >> coeffs_( coeffs_.rows()-1, 0 ); // Fill the angle
	ss >> coeffs_( coeffs_.rows()-1, 1 );	// Fill the Coefficient for MAIN
	ss >> coeffs_( coeffs_.rows()-1, 2 ); // Fill the Coefficient for JIB
	ss >> coeffs_( coeffs_.rows()-1, 3 ); // Fill the Coefficient for SPI

	// Convert the first value of the line - the angle - from deg to rad
	coeffs_( coeffs_.rows()-1, 0 ) *= M_PI / 180.0;

	//std::cout<< "  -->> coeffs_: "<<coeffs_<<std::endl;

}

// Check that all the required variables have been
// prompted into the file. Otherwise throws
void VPPSailCoefficientIO::check() {

	// Check 1: angles and coefficients are positive
	for(size_t i=0; i<coeffs_.rows(); i++) {
		for(size_t j=0; j<coeffs_.cols(); j++) {
			if(coeffs_(i,j) < -1E-12 ){
				char msg[512];
				sprintf(msg, "Negative angles are not allowed for sail "
						"coefficient definition. Found %6.3f ", coeffs_(i,j) );
				Warning(string(msg));
			}
		}
	}

	// Check 2: angles are defined in ascending order
	for(size_t i=1; i<coeffs_.rows(); i++)
		if(coeffs_(i,0) < coeffs_(i-1,0) )
			throw VPPException(HERE,"Sail coefficient are supposedly defined by ascending angles");

}

// Printout the list of all variables we have collected
void VPPSailCoefficientIO::print(FILE* outStream/*=stdout*/) {

	// Print the header
	fprintf( outStream, "%s\n", getHeaderBegin().c_str() );

	for(size_t i=1; i<coeffs_.rows(); i++) {
		for(size_t j=0; j<coeffs_.cols(); j++)
			fprintf(outStream, "%8.6f    ", coeffs_(i,j) );
		fprintf(outStream, "\n");
	}
	// Print the footer
	fprintf( outStream, "%s\n", getHeaderEnd().c_str() );

}

// Get the cl matrix.
const Eigen::ArrayXXd* VPPSailCoefficientIO::getCoefficientMatrix() const {
	return &coeffs_;
}


//---------------------------------------------------------------------

VPP_CL_IO::VPP_CL_IO(VariableFileParser* pParser, string fileName) :
						VPPSailCoefficientIO(fileName),
						fullBattens_(pParser->get("MFLB")){

	// Pick the lift coefficient for this main (full batten or not)
	if( fullBattens_ ) {

		clMat0_ << 0, 	0, 			0, 		0   ,
				20, 1.3, 	  1.2,	0.02,
				27, 1.725,	1.5,	0.1 ,
				50, 1.5, 		0.5,	1.5 ,
				60, 1.25, 	0.4,	1.25,
				80,	0.95, 	0.3,	1.0 ,
				100,0.85, 	0.1,	0.85,
				140,0.2, 		0.05,	0.2 ,
				180,0, 			0, 		0		;

	} else {

		clMat0_ << 0, 		0, 	 	0 , 	0,
				20, 	1.2, 	1.2,   0.02,
				27, 	1.5, 	1.5,	0.1,
				50, 	1.5, 	0.5,	1.5,
				60, 	1.25, 0.4,	1.25,
				80, 	0.95,	0.3,	1.0,
				100,	0.85,	0.0,	0.85,
				140,	0.2, 	0.05,	0.2 ,
				180,	0., 	0.05,	0.1 ;
	}

	// Convert the angular values to radians
	clMat0_.col(0) *= M_PI / 180.0;

	// Copy the values for the outer world, but keep the buffer
	// that can be used in the case of user-defined coefficients
	coeffs_ = clMat0_;
}

// Dtor
VPP_CL_IO::~VPP_CL_IO() {

}

// Implement pure virtual: get the identifier for the beginning
// of a file section
const string VPP_CL_IO::getHeaderBegin() const {
	return string("==SAIL COEFFS CL==");
}

// Implement pure virtual: get the identifier for the end of a
// file section
const string VPP_CL_IO::getHeaderEnd() const {
	return string("==END SAIL COEFFS CL==");
}


//---------------------------------------------------------------------

VPP_CD_IO::VPP_CD_IO(string fileName)  :
						VPPSailCoefficientIO(fileName) {

	cdMat0_ << 0,  	0,   	0,   		0,
			15,  	0.02, 0.005, 	0.02,
			27, 	0.03,	0.02,		0.05,
			50, 	0.15,	0.25,		0.25,
			80, 	0.8, 	0.15,		0.9,
			100,	1.0, 	0.05,	 	1.2,
			140,	0.95, 0.01, 	0.8,
			180,	0.9, 	0.0, 		0.66;

	// convert the angular values to radians
	cdMat0_.col(0) *= M_PI / 180.0;

	// Copy the values for the outer world, but keep the buffer
	// that can be used in the case of user-defined coefficients
	coeffs_ = cdMat0_;

}

// Dtor
VPP_CD_IO::~VPP_CD_IO() {

}

// Implement pure virtual: get the identifier for the beginning
// of a file section
const string VPP_CD_IO::getHeaderBegin() const {
	return string("==SAIL COEFFS CD==");
}

// Implement pure virtual: get the identifier for the end of a
// file section
const string VPP_CD_IO::getHeaderEnd() const {
	return string("==END SAIL COEFFS CD==");
}
