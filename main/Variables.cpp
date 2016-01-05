#include "Variables.h"

// Implicit copy constructor used to cast a variable to its name
Variable::Variable(const string& varName, const double& val)
: varName_(varName),
	val_(val) {
	// make nothing
}

// Overload operator < to compare in set
bool Variable::operator < (const Variable& rhs) const {
	return varName_<rhs.varName_;
}

// Overload operator == to compare in set
bool Variable::operator==(const Variable& rhs) const {
	return varName_ == rhs.varName_;
}

// Self cast operator, returns the underlying value
Variable::operator double() const {
	return val_;
}

//=========================================

// Overload operator [] - non const variety
Variable VarSet::operator [] (string varName){
	return *(find(varName));
}

// Overload operator [] - const variety
const Variable VarSet::operator [] (string varName) const {
	return *(find(varName));
}

// Iterate in the set and printout the variables
void VarSet::print() {

	for(std::set<Variable>::iterator it= begin(); it!=end(); ++it){
		std::cout<< it->varName_ << ":  "<< it->val_ <<std::endl;
	}
}
