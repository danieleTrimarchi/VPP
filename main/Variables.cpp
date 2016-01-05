#include "Variables.h"

// Implicit copy constructor used to cast a variable to its name
Variable::Variable(const string& varName, const double& val)
: varName_(varName),
	val_(val) {

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
