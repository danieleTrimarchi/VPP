#include "Variables.h"
#include "VPPException.h"
#include "utility"

// Init static members
// XML tag used to describe the internal name of an item
std::string Variable::variableNameTag_("VariableName");

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

	//std::cout<<"\tNAME= "<<varName_<<"  "<<rhs.varName_<<std::endl;
	//std::cout<<"\tVALUE= "<<val_<<"  "<<rhs.val_<<std::endl;
	return (varName_ == rhs.varName_ &&
			val_==rhs.val_);
}

// Inverse comparison operator
bool Variable::operator!=(const Variable& rhs) const {
	return !(*this==rhs);
}

// Self cast operator, returns the underlying value
Variable::operator double() const {
	return val_;
}

//=========================================

const string VarSet::headerBegin_=string("==VARIABLES==");
const string VarSet::headerEnd_=string("==END VARIABLES==");

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
void VarSet::print(FILE* outStream/*=stdout*/) {

	// Print the header begin
	fprintf( outStream, "%s\n", headerBegin_.c_str() );

	// Print the variables
	for(std::set<Variable>::iterator it= begin(); it!=end(); ++it)
		fprintf(outStream, "%s  %8.6f \n", it->varName_.c_str(), it->val_ );

	// Print the header end
	fprintf( outStream, "%s\n", headerEnd_.c_str() );

}

// Populate the tree model that will be used to
// visualize the variables in the UI
void VarSet::populate(VariableTreeModel* pTreeModel) {

	for(std::set<Variable>::iterator it= begin(); it!=end(); ++it)
		pTreeModel->append(it->varName_.c_str(),it->val_);

}

// Comparison operator
bool VarSet::operator == (const VarSet& rhs) {

	// First comparison on the size of the set
	if(size() != rhs.size())
		return false;

	// Compare value by value. Return false if any item is found to be
	// different
	for(std::set<Variable>::iterator 	it= begin(), itrhs= rhs.begin();
																	it==end(),		itrhs==end();
																	it++, 				itrhs++){
		if(*it!=*itrhs){
			return false;
		}
	}

	// No differences found: return true
	return true;
}

// Inverse comparison operator
bool VarSet::operator != (const VarSet& rhs) {
	return !(*this==rhs);
}

