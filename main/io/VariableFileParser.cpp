#include "VariableFileParser.h"
#include <stdio.h>
#include <cmath>

#include "../gui/settingsWindow/GetItemVisitor.h"
#include "Warning.h"
#include "VPPException.h"
#include "mathUtils.h"
#include "TreeTab.h"
#include "VPPSettingsDialog.h"
#include "VariableTreeModel.h"

// Ctor
VariableParserGetVisitor::VariableParserGetVisitor(VariableFileParser* pParser):
pParser_(pParser) {

}

// Dtor
VariableParserGetVisitor::~VariableParserGetVisitor() {

}

// Disallow default ctor
VariableParserGetVisitor::VariableParserGetVisitor():
			pParser_(0) {

}

// Visit a 'Generic' SettingsItemBase
void VariableParserGetVisitor::visit(SettingsItemBase* pRoot) {

	// Do nothing special

	// Loop on the tree
	for(size_t iChild=0; iChild<pRoot->childCount(); iChild++){
		SettingsItemBase* pChild= dynamic_cast<SettingsItemBase*>(pRoot->child(iChild));
		pChild->accept(*this);
	}
}

// Visit a 'Generic' SettingsItemBase
void VariableParserGetVisitor::visit(SettingsItemRoot* pRoot) {

	// Do nothing special

	// Loop on the tree
	for(size_t iChild=0; iChild<pRoot->childCount(); iChild++){
		SettingsItemBase* child= dynamic_cast<SettingsItemBase*>(pRoot->child(iChild));
		child->accept(*this);
	}
}

//---------------------------------------------------------


// Constructor
VariableFileParser::VariableFileParser() {

	// Set the variable the user must define in the input file.
	// Method check() will assure all variables have been defined
	requiredVariables_.reserve(61);

	// %%%%%%% SETUP THE DESIGN SPACE FOR THE OPTIMIZER %%%%%%%%
	requiredVariables_.push_back(Var::vBounds_.min_);   // [m/s] Constraint the min boat speed
	requiredVariables_.push_back(Var::vBounds_.max_);   // [m/s] Constraint the max boat speed
	requiredVariables_.push_back(Var::heelBounds_.min_); // [deg] Constraint the min heel angle
	requiredVariables_.push_back(Var::heelBounds_.max_); // [deg] Constraint the max heel angle
	requiredVariables_.push_back(Var::crewBounds_.min_);   // [m]   Constraint the min crew distance from centerline
	requiredVariables_.push_back(Var::crewBounds_.max_);   // [m]   Constraint the max crew distance from centerline
	requiredVariables_.push_back(Var::flatBounds_.min_);   // [-]   Constraint the min Flattening factor
	requiredVariables_.push_back(Var::flatBounds_.max_);   // [-]   Constraint the max Flattening factor

	// %%%%%%% WIND %%%%%%%%
	requiredVariables_.push_back(Var::vtwBounds_.min_);	// [m/s] Min true wind velocity
	requiredVariables_.push_back(Var::vtwBounds_.max_);	// [m/s] Max true wind velocity
	requiredVariables_.push_back(Var::ntw_);		// [-]   Number of wind velocities in range

	requiredVariables_.push_back(Var::atwBounds_.min_); // [deg] Min true wind angle
	requiredVariables_.push_back(Var::atwBounds_.max_); // [deg] Max true wind angle
	requiredVariables_.push_back(Var::nta_); 	// [-]   Number of wind angles in range

	// %%%%%%% HULL %%%%%%%%
	requiredVariables_.push_back(Var::divCan_); 	// [m^3] Displaced volume of canoe body
	requiredVariables_.push_back(Var::lwl_);   		// [m]  Design waterline's length
	requiredVariables_.push_back(Var::bwl_);     	// [m]  Design waterline's beam
	requiredVariables_.push_back(Var::b_);       	// [m]  Design maximum beam
	requiredVariables_.push_back(Var::avgfreb_); 	// [m]  Average freeboard
	requiredVariables_.push_back(Var::xfb_);     	// [m]  Longitudinal center of buoyancy LCB from fpp
	requiredVariables_.push_back(Var::xff_);     	// [m]  Longitudinal center of flotation LCF from fpp
	requiredVariables_.push_back(Var::cpl_);     	// [-]  Longitudinal prismatic coefficient
	requiredVariables_.push_back(Var::hullff_);  	// [-]  Hull form factor
	requiredVariables_.push_back(Var::aw_);   	 	// [m^2] Design waterplane's area
	requiredVariables_.push_back(Var::sc_);     	// [m^2] Wetted surface area of canoe body
	requiredVariables_.push_back(Var::cms_);    	// [-]  Midship section coefficient
	requiredVariables_.push_back(Var::t_);     		// [m]  Total draft
	requiredVariables_.push_back(Var::tcan_); 		// [m]  Draft of canoe body
	requiredVariables_.push_back(Var::alt_); 			// [m^2] Total lateral area of yacht
	requiredVariables_.push_back(Var::kg_);       // [m]  Center of gravity above moulded base or keel
	requiredVariables_.push_back(Var::km_);   		// [m]  Transverse metacentre above moulded base or keel
	// %%%%%%% KEEL %%%%%%%%
	requiredVariables_.push_back(Var::dvk_);     	// [m^3] Displaced volume of keel
	requiredVariables_.push_back(Var::apk_);     	// [m^2] Keel's planform area
	requiredVariables_.push_back(Var::ask_);    	// [-]   Keel?s aspect ratio
	requiredVariables_.push_back(Var::sk_);    		// [m^2] Keel?s wetted surface
	requiredVariables_.push_back(Var::zcbk_);  		// [m]  Keel?s vertical center of buoyancy (below free surface)
	requiredVariables_.push_back(Var::chmek_);   	// [m]  Mean chord length
	requiredVariables_.push_back(Var::chrtk_);   	// [m]  Root chord length
	requiredVariables_.push_back(Var::chtpk_);   	// [m]  Tip chord length
	requiredVariables_.push_back(Var::keelff_);  	// [-]  Keel's form factor
	requiredVariables_.push_back(Var::delttk_);  	// [-]  Mean thickness ratio of keel section
	requiredVariables_.push_back(Var::tak_);     	// [-]  Taper ratio of keel (CHRTK/CHTPK)
	// %%%%%%% RUDDER %%%%%%%
	requiredVariables_.push_back(Var::dvr_);     	// [m^3] Rudder?s displaced volume
	requiredVariables_.push_back(Var::apr_);    	// [m^2] Rudder?s planform area
	requiredVariables_.push_back(Var::sr_);  			// [m^2] Rudder?s wetted surface
	requiredVariables_.push_back(Var::chmer_); 		// [m]  Mean chord length
	requiredVariables_.push_back(Var::chrtr_);   	// [m]  Root chord length
	requiredVariables_.push_back(Var::chtpr_);   	// [m]  Tip chord length
	requiredVariables_.push_back(Var::delttr_);  	// [m]  Mean thickness ratio of rudder section
	requiredVariables_.push_back(Var::ruddff_);  	// [m]  Rudder?s form factor
	// %%%%%%% SAILS %%%%%%%%
	requiredVariables_.push_back(Var::sailSet_);	//% 3 - main & jib; 5 - main & spi; 7 - main, jib, & spinnaker;
	requiredVariables_.push_back(Var::p_);  			// [m]  Mainsail height
	requiredVariables_.push_back(Var::e_); 				// [m]  Mainsail base
	requiredVariables_.push_back(Var::mroach_);		// [-]  Correction for mainsail roach [-]
	requiredVariables_.push_back(Var::mflb_);			// [0/1] Full main battens in main
	requiredVariables_.push_back(Var::bad_); 			// [m]  Boom height above deck
	requiredVariables_.push_back(Var::i_); 				// [m]  Foretriangle height
	requiredVariables_.push_back(Var::j_); 				// [m]  Foretriangle base
	requiredVariables_.push_back(Var::lpg_); 			// [m]  Perpendicular of longest jib
	requiredVariables_.push_back(Var::sl_); 			// [m]  Spinnaker length
	requiredVariables_.push_back(Var::ehm_); 			// [m]  Mast's height above deck
	requiredVariables_.push_back(Var::emdc_);			// [m]  Mast's average diameter
	//%%%%%%% CREW %%%%%%%%%%
	requiredVariables_.push_back(Var::mmvblcrw_); // [kg] Movable Crew Mass

}

// Constructor - the settingsDialog is in charge for
// populating the parser
VariableFileParser::VariableFileParser(VPPSettingsDialog* pSd) :
				VariableFileParser(){

	// Get a handle to the settings tree
	TreeTab* pSettingsTreeTab( pSd->getSettingsTreeTab() );

	// Get a handle to the reference model.
	const SettingsModel* pSettingsModel = pSettingsTreeTab->getReferenceSettingsModel();

	// Get the root of the reference settings model
	SettingsItemBase* pSettingsModelRoot = pSettingsModel->getRoot();

	// Instantiate a visitor that will visit root and fill the
	// variables for the variableFileParser
	VariableParserGetVisitor v(this);
	pSettingsModelRoot->accept(v);

}

// Constructor using directly the root of the variableTreeModel
VariableFileParser::VariableFileParser(SettingsItemBase* pSettingsModelRoot) {

	// Instantiate a visitor that will visit root and fill the
	// variables for the variableFileParser
	VariableParserGetVisitor v(this);
	pSettingsModelRoot->accept(v);

}


// Destructor
VariableFileParser::~VariableFileParser() {
	// make nothing
}

// Implement the pure virtual : do all is required before
// starting the parse (init)
size_t VariableFileParser::preParse() {

	// Make sure we have a file to parse
	if(!fileName_.size())
		throw VPPException(HERE,"VariableFileName not defined!");

	// Make sure the variables_ set is empty
	variables_.clear();

	return keepParsing::keep_going;
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

		VarSet::iterator it;
		it=variables_.find(requiredVariables_[iVar]);
		if(it == variables_.end()) {
			char msg[256];
			sprintf(msg,"Variable \"%s\" was not defined in file \"%s\"",requiredVariables_[iVar].c_str(),fileName_.c_str());
			throw VPPException(HERE,msg);
		}
	}

	// Add some check on the variables: are they within the model validity?
	// TODO dtrimarchi : add the source of these limitations: possibly Delft
	if( variables_[Var::lwl_]/variables_[Var::bwl_] <=  2.73 ||
			variables_[Var::lwl_]/variables_[Var::bwl_] >=  5.00 )
		Warning("LWL/BWL is out of valuable interval");

	if( variables_[Var::bwl_]/variables_[Var::tcan_] <=  2.46 ||
			variables_[Var::bwl_]/variables_[Var::tcan_] >=  19.38 )
		Warning("BWL/TCAN is out of valuable interval");

	if( variables_[Var::lwl_]/std::pow(variables_[Var::divCan_],1./3) <=   4.34 ||
			variables_[Var::lwl_]/std::pow(variables_[Var::divCan_],1./3) >=  8.50 )
		Warning("LWL/DIVCAN^(1/3) is out of valuable interval");

	if( variables_[Var::cpl_] <= 0.52 || variables_[Var::cpl_] >= 0.6 ){
		std::cout<<"CPL= "<<variables_[Var::cpl_]<<std::endl;
		Warning("CPL is out of valuable interval 0.52-0.6");
	}

	if( variables_[Var::cms_] <= 0.65 || variables_[Var::cms_] >= 0.78 ){
		std::cout<<"CMS= "<<variables_[Var::cms_]<<std::endl;
		Warning("CMS is out of valuable interval 0.65-0.78");
	}

	if( variables_[Var::aw_]/std::pow(variables_[Var::divCan_],2./3) <= 3.78 ||
			variables_[Var::aw_]/std::pow(variables_[Var::divCan_],2./3) >= 12.67 )
		Warning("Loading Factor is out of valuable data");

	if(variables_[Var::vtwBounds_.max_] <= variables_[Var::vtwBounds_.min_] )
		throw VPPException(HERE,"VTW_MIN is larger than VTW_MAX!");

	if(variables_[Var::atwBounds_.max_] <= variables_[Var::atwBounds_.min_] )
		throw VPPException(HERE,"TWA_MIN is larger than TWA_MAX!");
    
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

	// Cleanup : destroy the children of the variableTreeModel
	pTreeModel->clearChildren();

	// Now populate the variables_ container with the variables contained
	// in the tree model
	variables_.populate(pTreeModel);
}

// Comparison operator. Are the variables contained into
// this parser equal to the variables of another parser?
bool VariableFileParser::operator == (const VariableFileParser& rhs) {
	return (variables_ == rhs.variables_);
}

// Insert a new variable given its name and value
void VariableFileParser::insert(QString variableName, double variableValue) {

	Variable newVariable;
	newVariable.varName_= variableName.toStdString();
	newVariable.val_= variableValue;

	variables_.insert(newVariable);

}








