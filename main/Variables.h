#ifndef VARIABLES_H
#define VARIABLES_H

#include <string>
#include <set>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

/// Helper struct for named
/// variables
class Variable {

	public:

		/// Implicit copy constructor used to cast a variable to its name
		Variable(const string& varName = "", const double& val = 0.);

		/// Overload operator < to compare in set
		bool operator < (const Variable& rhs) const;

		/// Overload operator == to compare in set
		bool operator==(const Variable& rhs) const;

		/// Self cast operator, returns the underlying value
		operator double() const;

		/// Overload operator << to printout variable name and value
		friend ostream& operator << ( ostream& os, const Variable& var) {
			os <<"  "<< var.varName_ <<"  "<<var.val_;
			return os;
		};

		/// Name of this variable
		string varName_;

		/// Value of this variable
		double val_;

};

/// Utility class that derives from set and overloads the
/// operator [] with set::find
class VarSet : public set<Variable> {

	public:

		/// Overload operator [] - non const variety
		Variable operator [] (string varName);

		/// Overload operator [] - const variety
		const Variable operator [] (string varName) const;

		/// Iterate in the set and printout the variables
		void print();

};


#endif
