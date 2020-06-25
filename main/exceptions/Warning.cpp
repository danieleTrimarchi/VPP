#include "Warning.h"
#include <iostream>
#include <QMessageBox>

// Constructor
Warning::Warning(string msg) {

	cout<<"\n========== VPP WARNING : ==================="<<endl;
	cout<<"---> "<<msg<<"\n"<<std::endl;

	QMessageBox msgBox;
	msgBox.setText(QString(msg.c_str()));
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setWindowTitle("Warning");
	msgBox.exec();

}

// Disallow default constructor
Warning::Warning() {

}

// Destructor
Warning::~Warning(){
	//do nothing
}


