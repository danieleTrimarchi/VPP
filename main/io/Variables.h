#ifndef VARIABLES_H
#define VARIABLES_H

#include <string>
#include <set>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "VariableTreeModel.h"

using namespace std;

/// Helper struct for named
/// variables
class Variable {

	public:

		/// Implicit copy constructor used to cast a variable to its name
		Variable(const string& varName = "", const double& val = 0.);

		/// Assignment operator
		Variable& operator = ( const Variable rhs );

		/// Assignment operator
		Variable& operator = ( const double val );

		/// Overload operator < to compare in set
		bool operator < (const Variable& rhs) const;

		/// Overload operator == to compare in set
		bool operator==(const Variable& rhs) const;

		/// Inverse comparison operator
		bool operator!=(const Variable& rhs) const;

		/// Self cast operator, returns the underlying value
		/// See https://msdn.microsoft.com/en-us/library/wwywka61.aspx
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
		Variable& operator [] (string varName);

		/// Overload operator [] - const variety
		const Variable& operator [] (string varName) const;

		/// Iterate in the set and printout the variables
		void print(FILE* outStream=stdout);

		/// Populate the tree model that will be used to
		/// visualize the variables in the UI
		void populate(VariableTreeModel*);

		/// Comparison operator
		bool operator == (const VarSet&);

		/// Inverse comparison operator
		bool operator != (const VarSet&);

		/// Header of variable section in an input or result file
		static const string headerBegin_, headerEnd_;

};


#endif
