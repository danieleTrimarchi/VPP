#include "VariableFileParser.h"
#include <stdio.h>
#include <cmath>
#include "Warning.h"
#include "VPPException.h"
#include "mathUtils.h"
#include "VariableTreeModel.h"

// Constructor
VariableFileParser::VariableFileParser(std::string fileName) :
FileParserBase(fileName) {

	// Set the variable the user must define in the input file.
	// Method check() will assure all variables have been defined
	requiredVariables_.reserve(61);

	// %%%%%%% SETUP THE DESIGN SPACE FOR THE OPTIMIZER %%%%%%%%
	requiredVariables_.push_back("V_MIN");   // [m/s] Constraint the min boat speed
	requiredVariables_.push_back("V_MAX");   // [m/s] Constraint the max boat speed
	requiredVariables_.push_back("PHI_MIN"); // [deg] Constraint the min heel angle
	requiredVariables_.push_back("PHI_MAX"); // [deg] Constraint the max heel angle
	requiredVariables_.push_back("B_MIN");   // [m]   Constraint the min crew distance from centerline
	requiredVariables_.push_back("B_MAX");   // [m]   Constraint the max crew distance from centerline
	requiredVariables_.push_back("F_MIN");   // [-]   Constraint the min Flattening factor
	requiredVariables_.push_back("F_MAX");   // [-]   Constraint the max Flattening factor

	// %%%%%%% WIND %%%%%%%%
	requiredVariables_.push_back("V_TW_MIN");	// [m/s] Min true wind velocity
	requiredVariables_.push_back("V_TW_MAX");	// [m/s] Max true wind velocity
	requiredVariables_.push_back("N_TWV");		// [-]   Number of wind velocities in range

	requiredVariables_.push_back("ALPHA_TW_MIN"); // [deg] Min true wind angle
	requiredVariables_.push_back("ALPHA_TW_MAX"); // [deg] Max true wind angle
	requiredVariables_.push_back("N_ALPHA_TW"); 	// [-]   Number of wind angles in range

	// %%%%%%% HULL %%%%%%%%
	requiredVariables_.push_back("DIVCAN"); 	// [m^3] Displaced volume of canoe body
	requiredVariables_.push_back("LWL");   		// [m]  Design waterline's length
	requiredVariables_.push_back("BWL");     	// [m]  Design waterline's beam
	requiredVariables_.push_back("B");       	// [m]  Design maximum beam
	requiredVariables_.push_back("AVGFREB"); 	// [m]  Average freeboard
	requiredVariables_.push_back("XFB");     	// [m]  Longitudinal center of buoyancy LCB from fpp
	requiredVariables_.push_back("XFF");     	// [m]  Longitudinal center of flotation LCF from fpp
	requiredVariables_.push_back("CPL");     	// [-]  Longitudinal prismatic coefficient
	requiredVariables_.push_back("HULLFF");  	// [-]  Hull form factor
	requiredVariables_.push_back("AW");   	 	// [m^2] Design waterplane's area
	requiredVariables_.push_back("SC");     	// [m^2] Wetted surface area of canoe body
	requiredVariables_.push_back("CMS");    	// [-]  Midship section coefficient
	requiredVariables_.push_back("T");     		// [m]  Total draft
	requiredVariables_.push_back("TCAN"); 		// [m]  Draft of canoe body
	requiredVariables_.push_back("ALT"); 			// [m^2] Total lateral area of yacht
	requiredVariables_.push_back("KG");       // [m]  Center of gravity above moulded base or keel
	requiredVariables_.push_back("KM");   		// [m]  Transverse metacentre above moulded base or keel
	// %%%%%%% KEEL %%%%%%%%
	requiredVariables_.push_back("DVK");     	// [m^3] Displaced volume of keel
	requiredVariables_.push_back("APK");     	// [m^2] Keel's planform area
	requiredVariables_.push_back("ASK");    	// [-]   Keel?s aspect ratio
	requiredVariables_.push_back("SK");    		// [m^2] Keel?s wetted surface
	requiredVariables_.push_back("ZCBK");  		// [m]  Keel?s vertical center of buoyancy (below free surface)
	requiredVariables_.push_back("CHMEK");   	// [m]  Mean chord length
	requiredVariables_.push_back("CHRTK");   	// [m]  Root chord length
	requiredVariables_.push_back("CHTPK");   	// [m]  Tip chord length
	requiredVariables_.push_back("KEELFF");  	// [-]  Keel's form factor
	requiredVariables_.push_back("DELTTK");  	// [-]  Mean thickness ratio of keel section
	requiredVariables_.push_back("TAK");     	// [-]  Taper ratio of keel (CHRTK/CHTPK)
	// %%%%%%% RUDDER %%%%%%%
	requiredVariables_.push_back("DVR");     	// [m^3] Rudder?s displaced volume
	requiredVariables_.push_back("APR");    	// [m^2] Rudder?s planform area
	requiredVariables_.push_back("SR");  			// [m^2] Rudder?s wetted surface
	requiredVariables_.push_back("CHMER"); 		// [m]  Mean chord length
	requiredVariables_.push_back("CHRTR");   	// [m]  Root chord length
	requiredVariables_.push_back("CHTPR");   	// [m]  Tip chord length
	requiredVariables_.push_back("DELTTR");  	// [m]  Mean thickness ratio of rudder section
	requiredVariables_.push_back("RUDDFF");  	// [m]  Rudder?s form factor
	// %%%%%%% SAILS %%%%%%%%
	requiredVariables_.push_back("SAILSET");	//% 3 - main & jib; 5 - main & spi; 7 - main, jib, & spinnaker;
	requiredVariables_.push_back("P");  			// [m]  Mainsail height
	requiredVariables_.push_back("E"); 				// [m]  Mainsail base
	requiredVariables_.push_back("MROACH");		// [-]  Correction for mainsail roach [-]
	requiredVariables_.push_back("MFLB");			// [0/1] Full main battens in main
	requiredVariables_.push_back("BAD"); 			// [m]  Boom height above deck
	requiredVariables_.push_back("I"); 				// [m]  Foretriangle height
	requiredVariables_.push_back("J"); 				// [m]  Foretriangle base
	requiredVariables_.push_back("LPG"); 			// [m]  Perpendicular of longest jib
	requiredVariables_.push_back("SL"); 			// [m]  Spinnaker length
	requiredVariables_.push_back("EHM"); 			// [m]  Mast's height above deck
	requiredVariables_.push_back("EMDC");			// [m]  Mast's average diameter
	//%%%%%%% CREW %%%%%%%%%%
	requiredVariables_.push_back("MMVBLCRW"); // [kg] Movable Crew Mass

}

// Destructor
VariableFileParser::~VariableFileParser() {
	// make nothing
}

// Implement the pure virtual : do all is required before
// starting the parse (init)
void VariableFileParser::preParse() {

	// Make sure the variables_ set is empty
	variables_.clear();

}

// Implement pure virtual: get the identifier for the beginning
// of a file section
const string VariableFileParser::getHeaderBegin() const {
	return VarSet::headerBegin_;
}

// Implement pure virtual: get the identifier for the end of a
// file section
const string VariableFileParser::getHeaderEnd() const {
	return VarSet::headerEnd_;
}

// Each subclass implement its own method to do something
// out of this stream
void VariableFileParser::parseLine(string& line) {

	if(line.empty())
		return;

	// Use stringstream to read the name of the variable and its value
	std::stringstream ss(line);

	Variable newVariable;
	ss >> newVariable.varName_;
	ss >> newVariable.val_;
	//std::cout<< "  -->> Read: "<<newVariable<<std::endl;

	variables_.insert(newVariable);

}

// Check that all the required variables have been
// prompted into the file. Otherwise throws
void VariableFileParser::check() {

	// Loop on all the required variables and make sure all have
	// been defined in the input file
	for(size_t iVar=0; iVar<requiredVariables_.size(); iVar++) {

		std::set<Variable>::iterator it;
		it=variables_.find(requiredVariables_[iVar]);
		if(it == variables_.end()) {
			char msg[256];
			sprintf(msg,"Variable \"%s\" was not defined in file \"%s\"",requiredVariables_[iVar].c_str(),fileName_.c_str());
			throw VPPException(HERE,msg);
		}
	}

	// Add some check on the variables: are they within the model validity?
	// TODO dtrimarchi : add the source of these limitations: possibly Delft
	if( variables_["LWL"]/variables_["BWL"] <=  2.73 ||
			variables_["LWL"]/variables_["BWL"] >=  5.00 )
		Warning("LWL/BWL is out of valuable interval");

	if( variables_["BWL"]/variables_["TCAN"] <=  2.46 ||
			variables_["BWL"]/variables_["TCAN"] >=  19.38 )
		Warning("BWL/TCAN is out of valuable interval");

	if( variables_["LWL"]/std::pow(variables_["DIVCAN"],1./3) <=   4.34 ||
			variables_["LWL"]/std::pow(variables_["DIVCAN"],1./3) >=  8.50 )
		Warning("LWL/DIVCAN^(1/3) is out of valuable interval");

	if( variables_["CPL"] <= 0.52 || variables_["CPL"] >= 0.6 ){
		std::cout<<"CPL= "<<variables_["CPL"]<<std::endl;
		Warning("CPL is out of valuable interval 0.52-0.6");
	}

	if( variables_["CMS"] <= 0.65 || variables_["CMS"] >= 0.78 ){
		std::cout<<"CMS= "<<variables_["CMS"]<<std::endl;
		Warning("CMS is out of valuable interval 0.65-0.78");
	}

	if( variables_["AW"]/std::pow(variables_["DIVCAN"],2./3) <= 3.78 ||
			variables_["AW"]/std::pow(variables_["DIVCAN"],2./3) >= 12.67 )
		Warning("Loading Factor is out of valuable data");

	if(variables_["V_TW_MAX"] <= variables_["V_TW_MIN"] )
		throw VPPException(HERE,"V_TW_MIN is larger than V_TW_MAX!");

	if(variables_["ALPHA_TW_MAX"] <= variables_["ALPHA_TW_MIN"] )
		throw VPPException(HERE,"ALPHA_TW_MIN is larger than ALPHA_TW_MAX!");

	// Convert ALL the angles from DEG to RAD
	variables_["PHI_MIN"] = mathUtils::toRad( variables_["PHI_MIN"] );
	variables_["PHI_MAX"] = mathUtils::toRad( variables_["PHI_MAX"] );

	variables_["ALPHA_TW_MIN"] = mathUtils::toRad( variables_["ALPHA_TW_MIN"] );
	variables_["ALPHA_TW_MAX"] = mathUtils::toRad( variables_["ALPHA_TW_MAX"] );

}

/// Get the value of a variable
double VariableFileParser::get(std::string varName) {
	return variables_[varName];
}

// Get the variables contained in the parser
const VarSet* VariableFileParser::getVariables() const {
	return &variables_;
}

// Printout the list of all variables we have collected
void VariableFileParser::print(FILE* outStream/*=stdout*/) {

	variables_.print(outStream);

}

// Get the number of variables that have been read in
size_t VariableFileParser::getNumVars() {
	return variables_.size();
}

// Populate the tree model that will be used to
// visualize the variables in the UI
void VariableFileParser::populate(VariableTreeModel* pTreeModel) {
	variables_.populate(pTreeModel);
}








