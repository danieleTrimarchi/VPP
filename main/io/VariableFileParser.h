#ifndef VARIABLE_FILEPARSER_H
#define VARIABLE_FILEPARSER_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "Variables.h"

using namespace std;

/// Forward declarations
class VariableTreeModel;

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

		/// Get the value of a variable by name
		double get(std::string);

		/// Get the variables contained in the parser
		const VarSet* getVariables() const;

		/// Printout the list of all variables we have collected
		void printVariables();

		/// Get the number of variables that have been read in
		size_t getNumVars();

		/// Populate the tree model that will be used to
		/// visualize the variables in the UI
		void populate(VariableTreeModel* pTreeModel);

	private:

		/// Name of the file to be opened
		string fileName_;

		/// Set of variables read from input file
		VarSet variables_;

		/// Collection of all variables requested for the analysis
		std::vector<std::string> requiredVariables_;

};

#endif
