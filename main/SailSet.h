#ifndef SAILSET_H
#define SAILSET_H

#include "VariableFileParser.h"
#include "Variables.h"

using namespace std;

enum sailConfig {
	mainOnly=1,
	mainAndJib=3,
	mainAndSpi=5,
	mainJibAndSpi=7,
};

/// Compute the sail geometry/configuration given the
/// informations stored in the parser
class SailSet {

	public:

		/// Constructor
		SailSet(const VariableFileParser& parser);

		/// Factory method
		static SailSet* SailSetFactory(VariableFileParser& parser);

		/// Destructor
		~SailSet();

	protected:

		/// Parser storing all the variables read
		/// from the input file
		const VariableFileParser* pParser_;

		/// Sail-specific variables
		VarSet sailVariables_;

};

class MainOnlySailSet : public SailSet {

	public:

		/// Constructor
		MainOnlySailSet(const VariableFileParser& parser);

};

class MainAndJibSailSet : public SailSet {

	public:

		/// Constructor
		MainAndJibSailSet(const VariableFileParser& parser);

};

class MainAndSpiSailSet : public SailSet {

	public:

		/// Constructor
		MainAndSpiSailSet(const VariableFileParser& parser);

};

class MainJibAndSpiSailSet : public SailSet {

	public:

		/// Constructor
		MainJibAndSpiSailSet(const VariableFileParser& parser);

};

#endif
