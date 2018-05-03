#include "VPPException.h"
#include <iostream>
#include <sstream>
#include <QtWidgets/QMessageBox>

// Constructor using c_str()
VPPException::VPPException(const char* inFile, int inLine, const char* inFunction, const char* message ) {
	std::ostringstream oss;
	oss<<"Error in function: "<<inFunction<<"\n in file: "<<inFile<<" line: "<<inLine<<std::endl;
	oss<<" Message: "<<message<<std::endl;
	msg_=oss.str();

	std::cout<<"\n-----------------------------------------\n";
	std::cout<<msg_<<std::endl;
	std::cout<<"\n-----------------------------------------\n";

	QMessageBox msgBox;
	msgBox.setText(QString(what()));
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.exec();
}

// Constructor using ostringstream
VPPException::VPPException(const char* inFile, int inLine, const char* inFunction, std::ostringstream message ){
	std::ostringstream oss;
	oss<<"Error in function: "<<inFunction<<"\n in file: "<<inFile<<" line: "<<inLine<<std::endl;
	oss<<" Message: "<<message.str()<<std::endl;
	msg_=oss.str();

	std::cout<<"\n-----------------------------------------\n";
	std::cout<<msg_<<std::endl;
	std::cout<<"\n-----------------------------------------\n";

	QMessageBox msgBox;
	msgBox.setText(QString(what()));
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.exec();

}

// Destructor
VPPException::~VPPException() throw() {

}

const char* VPPException::what() const throw() {
	return msg_.c_str();
}

////////////////////

// Constructor
NonConvergedException::NonConvergedException(const char* inFile, int inLine, const char* inFunction, const char* message ){

	std::ostringstream oss;
	oss<<"Error in function: "<<inFunction<<"\n in file: "<<inFile<<" line: "<<inLine<<std::endl;
	oss<<" Message: "<<message<<std::endl;
	msg_=oss.str();

	std::cout<<"\n-----------------------------------------\n";
	std::cout<<msg_<<std::endl;
	std::cout<<"\n-----------------------------------------\n";

}

// Destructor
NonConvergedException::~NonConvergedException() throw() {

}

const char* NonConvergedException::what() const throw() {
	return msg_.c_str();
}


////////////////////

// Constructor
NoPreviousConvergedException::NoPreviousConvergedException(const char* inFile, int inLine, const char* inFunction, const char* message ){

	std::ostringstream oss;
	oss<<"Warning: in function: "<<inFunction<<"\n in file: "<<inFile<<" line: "<<inLine<<std::endl;
	oss<<" Message: "<<message<<std::endl;
	msg_=oss.str();

	std::cout<<"\n-----------------------------------------\n";
	std::cout<<msg_<<std::endl;
	std::cout<<"\n-----------------------------------------\n";

	QMessageBox msgBox;
	msgBox.setText(QString(what()));
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.exec();

}

// Destructor
NoPreviousConvergedException::~NoPreviousConvergedException() throw() {

}

const char* NoPreviousConvergedException::what() const throw() {
	return msg_.c_str();
}
