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
class VariableFileParser;
class VPPSettingsDialog;
class VariableTreeModel;

class SettingsItemBase;
class SettingsItemRoot;
template <class TUnit>
class SettingsItem;
template <class TUnit>
class SettingsItemComboBox;
template <class TUnit>
class SettingsItemBounds;

/// Visitor used to retrieve the variables from the
/// SettingsModel and store them in the VariableFileParser
class VariableParserGetVisitor {

	public:

		/// Ctor
		VariableParserGetVisitor(VariableFileParser*);

		/// Dtor
		~VariableParserGetVisitor();

		/// Visit a SettingsItemBase
		void visit(SettingsItemBase*);

		/// Visit a SettingsItemRoot
		void visit(SettingsItemRoot*);

		/// Visit a SettingsItem
		template <class TUnit>
		void visit(SettingsItem<TUnit>* pItem);

		/// Visit a SettingsItem
		template <class TUnit>
		void visit(SettingsItemComboBox<TUnit>* pItem);

		/// Visit a SettingsItemBounds
		template <class TUnit>
		void visit(SettingsItemBounds<TUnit>* pItem);

	private:

		/// Disallow default ctor
		VariableParserGetVisitor();

		/// Ptr to the parser
		VariableFileParser* pParser_;
};

///---------------------------------------------------------

/// File parser able to store the variables read
/// into a file to a container set
class VariableFileParser : public FileParserBase {

	public:

		/// Constructor
		VariableFileParser();

		/// Constructor - the settingsDialog is in charge for
		/// populating the parser
		VariableFileParser(VPPSettingsDialog*);

		/// Constructor using directly the root of the variableTreeModel
		VariableFileParser(SettingsItemBase*);

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

		/// Insert a new variable given its name and value
		void insert(QString variableName, double variableValue);

		/// Get the number of variables that have been read in
		size_t getNumVars();

		/// Populate the tree model that will be used to
		/// visualize the variables in the UI
		void populate(VariableTreeModel* pTreeModel);

		/// Comparison operator. Are the variables contained into
		/// this parser equal to the variables of another parser?
		bool operator == (const VariableFileParser&);

	protected:

		/// Implement the pure virtual : do all is required before
		/// starting the parse (init)
		virtual size_t preParse();

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

#include "VariableFileParser_tpl.h"

#endif

