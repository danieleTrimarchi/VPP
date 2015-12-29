#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <string>
#include <set>
#include <fstream>
#include <iostream>
#include <sstream>

/// Helper struct for named
/// variables
struct variable {

		/// Name of this variable
		std::string varName_;

		/// Value of this variable
		double val_;

		/// Implicit copy constructor used to cast a variable to its name
		variable(const std::string& varName = "", const double& val = 0.)
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

		/// Overload operator << to printout variable name and value
		friend std::ostream& operator << (std::ostream&os, const variable& var) {
			os <<"  "<< var.varName_ <<"  "<<var.val_;
		}

};

/// File parser able to store the variables read
/// into a file to a container set
class VariableFileParser {

	public:

		/// Constructor
		VariableFileParser(std::string);

		/// Destructor
		~VariableFileParser();

		/// Parse the file
		void parse();

		/// Check that all the required variables have been
		/// prompted into the file. Otherwise throws
		void check();

	private:

		/// Name of the file to be opened
		std::string fileName_;

		std::set<variable> variables_;

};

#endif

