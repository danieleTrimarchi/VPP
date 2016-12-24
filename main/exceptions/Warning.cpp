#include "Warning.h"
#include <iostream>

// Constructor
Warning::Warning(string msg) {

	cout<<"\n========== VPP WARNING : ==================="<<endl;
	cout<<"---> "<<msg<<"\n"<<std::endl;

}

// Disallow default constructor
Warning::Warning() {

}

// Destructor
Warning::~Warning(){
	//do nothing
}


