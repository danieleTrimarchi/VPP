#ifndef VARIABLE_FILEPARSER_H
#define VARIABLE_FILEPARSER_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "FileParserBase.h"
#include "Variables.h"

using namespace std;

/// Forward declarations
class VariableTreeModel;

/// File parser able to store the variables read
/// into a file to a container set
class VariableFileParser : public FileParserBase {

	public:

		/// Constructor
		VariableFileParser(string);

		/// Destructor
		virtual ~VariableFileParser();

		/// Check that all the required variables have been
		/// prompted into the file. Otherwise throws
		virtual void check();

		/// Get the value of a variable by name
		double get(std::string);

		/// Get the variables contained in the parser
		const VarSet* getVariables() const;

		/// Printout the list of all variables we have collected
		void print(FILE* outStream=stdout);

		/// Get the number of variables that have been read in
		size_t getNumVars();

		/// Populate the tree model that will be used to
		/// visualize the variables in the UI
		void populate(VariableTreeModel* pTreeModel);

	protected:

		/// Implement the pure virtual : do all is required before
		/// starting the parse (init)
		virtual void preParse();

		/// Implement pure virtual: get the identifier for the beginning
		/// of a file section
		virtual const string getHeaderBegin() const;

		/// Implement pure virtual: get the identifier for the end of a
		/// file section
		virtual const string getHeaderEnd() const;

		/// Each subclass implement its own method to do something
		/// out of this stream
		virtual void parseLine(string&);

	private:

		/// Set of variables read from input file
		VarSet variables_;

		/// Collection of all variables requested for the analysis
		std::vector<std::string> requiredVariables_;

};

#endif

