#ifndef FILEPARSERBASE_H
#define FILEPARSERBASE_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "Variables.h"

using namespace std;

/// File parser able to store the variables read
/// into a file to a container set
class FileParserBase {

	public:

		enum keepParsing {
			stop=0,
			keep_going=1
		};

		/// Constructor
		FileParserBase();

		/// Destructor
		virtual ~FileParserBase();

		/// Parse the file
		void parse(string fileName);

	protected:

		/// Do all is required before starting the parse (init)
		virtual size_t preParse() =0;

		/// Get the identifier for the beginning of a file section
		virtual const string getHeaderBegin() const =0;

		/// Get the identifier for the end of a file section
		virtual const string getHeaderEnd() const =0;

		/// Each subclass implement its own method to parse
		/// the line and make something out of it
		virtual void parseLine(string& line) =0;

		/// Check that all the required entries have been
		/// prompted into the file. Otherwise throw
		virtual void check() =0;

		/// Name of the file to be opened
		string fileName_;

	private:

		// Parse the section of the file relative to the variables
		void parseSection(std::ifstream& infile);

};

#endif

