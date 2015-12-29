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

		/// Overload operator < to compare in set
		bool operator< (const variable& rhs) const {
			return varName_<rhs.varName_;
		}

		/// Overload operator << to printout variable name and value
		friend std::ostream& operator << (std::ostream&os, const variable& var) {
			os <<"  "<< var.varName_ <<"  "<<var.val_;
		}

};

class VariableFileParser {

	public:

		/// Constructor
		VariableFileParser(std::string);

		/// Destructor
		~VariableFileParser();

		/// Parse the file
		void parse();

	private:

		/// Name of the file to be opened
		std::string fileName_;

		std::set<variable> variables_;

};

#endif

