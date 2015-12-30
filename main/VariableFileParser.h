#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <string>
#include <set>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

/// Helper struct for named
/// variables
struct variable {

		/// Name of this variable
		string varName_;

		/// Value of this variable
		double val_;

		/// Implicit copy constructor used to cast a variable to its name
		variable(const string& varName = "", const double& val = 0.)
	  : varName_(varName),
	    val_(val) {}

		/// Overload operator < to compare in set
		bool operator < (const variable& rhs) const {
			return varName_<rhs.varName_;
		}

		/// Overload operator == to compare in set
	  bool operator==(const variable& rhs) const {
	    return varName_ == rhs.varName_;
	  }

	  /// Self cast operator, returns the underlying value
	  operator double() const {
	  	return val_;
	  }

		/// Overload operator << to printout variable name and value
		friend ostream& operator << ( ostream&os, const variable& var) {
			os <<"  "<< var.varName_ <<"  "<<var.val_;
		}

};

/// Utility class that derives from set and overloads the
/// operator [] with set::find
class VarSet : public set<variable> {

	public:

		variable operator [] (string varName){
			return *(find(varName));
		}


};

/// File parser able to store the variables read
/// into a file to a container set
class VariableFileParser {

	public:

		/// Constructor
		VariableFileParser(string);

		/// Destructor
		~VariableFileParser();

		/// Parse the file
		void parse();

		/// Check that all the required variables have been
		/// prompted into the file. Otherwise throws
		void check();

	private:

		/// Name of the file to be opened
		string fileName_;

		/// Set of variables read from input file
		VarSet variables_;

		/// Collection of all variables requested for the analysis
		std::vector<std::string> requiredVariables_;

};

#endif

