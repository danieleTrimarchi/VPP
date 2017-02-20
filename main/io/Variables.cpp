#include "Variables.h"
#include "VPPException.h"
#include "utility"

// Implicit copy constructor used to cast a variable to its name
Variable::Variable(const string& varName, const double& val)
: varName_(varName),
	val_(val) {
	// make nothing
}

// Assignment operator
Variable& Variable::operator = ( const Variable rhs ) {
	varName_= rhs.varName_;
	val_= rhs.val_;

	return *this;
}

/// Assignment operator
Variable& Variable::operator = ( const double val ) {
	val_= val;

	return *this;
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
Variable& VarSet::operator [] (string varName){

	VarSet::iterator it = find(varName);
	if(it == VarSet::end() ) {
		char msg[256];
		sprintf(msg,"Cannot find variable named: %s",varName.c_str());
		throw VPPException(HERE,msg);
	}

	// The set only has const iterators, because  alter variables already
	// in the set might alter the order of the set and thus invalidate it.
	// Here, we force to return a non-const reference because we will never
	// modify the key of the object in the set, but its value. This is
	// guaranteed by the automatic cast to double of class Variable.
	return const_cast<Variable&>(*it);
}

// Overload operator [] - const variety
const Variable& VarSet::operator [] (string varName) const {
	VarSet::iterator it = find(varName);
	if(it == VarSet::end() ) {
		char msg[256];
		sprintf(msg,"Cannot find variable named: %s",varName.c_str());
		throw VPPException(HERE,msg);
	}
	return *it;
}

// Iterate in the set and printout the variables
void VarSet::print() {

	for(std::set<Variable>::iterator it= begin(); it!=end(); ++it){
		std::cout<< it->varName_ << ":  "<< it->val_ <<std::endl;
	}
}

// Populate the tree model that will be used to
// visualize the variables in the UI
void VarSet::populate(VariableTreeModel* pTreeModel) {

	for(std::set<Variable>::iterator it= begin(); it!=end(); ++it){

		QList<QVariant> data;
		data << it->varName_.c_str() << it->val_;

		pTreeModel->append( data );
	}

}

