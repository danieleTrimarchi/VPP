#include "VPPException.h"
#include <iostream>
#include <sstream>
#include <QMessageBox>

// Constructor
VPPException::VPPException(const char* inFile, int inLine, const char* inFunction, const char* message ){

	std::ostringstream oss;
	oss<<"Error in function: "<<inFunction<<"\n in file: "<<inFile<<" line: "<<inLine<<std::endl;
	oss<<" Message: "<<message<<std::endl;
	msg=oss.str();

	std::cout<<"\n-----------------------------------------\n";
	std::cout<<msg<<std::endl;
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
	return msg.c_str();
}

////////////////////

// Constructor
NonConvergedException::NonConvergedException(const char* inFile, int inLine, const char* inFunction, const char* message ){

	std::ostringstream oss;
	oss<<"Error in function: "<<inFunction<<"\n in file: "<<inFile<<" line: "<<inLine<<std::endl;
	oss<<" Message: "<<message<<std::endl;
	msg=oss.str();

	std::cout<<"\n-----------------------------------------\n";
	std::cout<<msg<<std::endl;
	std::cout<<"\n-----------------------------------------\n";

	QMessageBox msgBox;
	msgBox.setText(QString(what()));
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.exec();

}

// Destructor
NonConvergedException::~NonConvergedException() throw() {

}

const char* NonConvergedException::what() const throw() {
	return msg.c_str();
}


////////////////////

// Constructor
NoPreviousConvergedException::NoPreviousConvergedException(const char* inFile, int inLine, const char* inFunction, const char* message ){

	std::ostringstream oss;
	oss<<"Warning: in function: "<<inFunction<<"\n in file: "<<inFile<<" line: "<<inLine<<std::endl;
	oss<<" Message: "<<message<<std::endl;
	msg=oss.str();

	std::cout<<"\n-----------------------------------------\n";
	std::cout<<msg<<std::endl;
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
	return msg.c_str();
}
