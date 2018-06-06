#include "EnumData.h"

// Init static data
enumData colNames::name_= enumData(0,"Name");
enumData colNames::value_=enumData(1,"Value");
enumData colNames::unit_= enumData(2,"Unit");

//////////////////////////////////////////////////////

/// Ctor
varData::varData(	const char* display,
									const char* name,
									const char* unit,
									const char* tooltip):
					display_(display), //> Display variable name
					name_(name),		//> Internal variable name
					unit_(unit),		//> Variable unit
					tootip_(tooltip)	{};	//> Variable tooltip

// Self cast operator, returns the underlying value
varData::operator std::string() const {
	return std::string(name_);
}

varDataBounds::varDataBounds(	const char* display,
		const char* name,
		const char* unit,
		const char* tooltip):
		varData(display,name,unit,tooltip),
							min_(name), //> init min_
							max_(name){ //> init max_

	// Decorate min and max - initialized to the internal
	// name - with their suffixes
	min_ += std::string("_MIN");
	max_ += std::string("_MAX");
};



//////////////////////////////////////////////////////

// Analysis Settings
varDataBounds Var::vBounds_= varDataBounds("Velocity bounds","V","m/s","Allowed boat speed bounds");
varDataBounds Var::heelBounds_= varDataBounds("Heel angle bounds","PHI","deg","Allowed boat heel angle bounds");
varDataBounds Var::crewBounds_= varDataBounds("Crew position bounds","B","m","Allowed boat heel angle bounds");
varDataBounds Var::flatBounds_= varDataBounds("Flat bounds","F"," ","Allowed sail flat bounds");
// Wind Settings
varDataBounds Var::vtwBounds_= varDataBounds("Wind speed bounds","VTW","m/s","Real Wind speed bounds"); //> Wind speed bounds
varData Var::ntw_= varData("n_twv","NTW"," ","Number of wind speed points"); //> Number of wind speeds
varDataBounds Var::atwBounds_= varDataBounds("Wind angle bounds","TWA","deg","Real Wind angle bounds"); //> Wind angle bounds
varData Var::nta_= varData("n_alpha_tw","N_TWA"," ","Number of wind speed points"); //> Number of wind angles
// Hull Settings
varData Var::lwl_= varData("LWL","LWL","m","Design waterline length"); //> Waterline lenght
varData Var::bwl_= varData("BWL","BWL","m","Design waterline beam"); //> Waterline beam
varData Var::b_= varData("B","B","m","Design maximum beam");   //> Design max beam
varData Var::t_= varData("T","T","m","Total draft");   //> Design max draft
varData Var::divCan_= varData("DIVCAN","DIVCAN","m^2","Displaced volume of canoe body"); //> Volume of the canoe body
varData Var::avgfreb_= varData("AVGFREB","AVGFREB","m","Average freeboard height"); //> Average freeboard height
varData Var::xfb_= varData("XFB","XFB","m","Longitudinal center of buoyancy LCB from fpp"); //> Longitudinal center of buoyancy LCB from fpp
varData Var::xff_= varData("XFF","XFF","m","Longitudinal center of flotation LCF from fpp"); //> Longitudinal center of flotation LCF from fpp
varData Var::cpl_= varData("CPL","CPL"," ","Longitudinal prismatic coefficient"); //> Longitudinal prismatic coefficient
varData Var::hullff_= varData("HULLFF","HULLFF"," ","Hull form factor"); //> Hull form factor
varData Var::aw_= varData("AW","AW","m^2","Design waterplane area"); //> Design waterplane area
varData Var::sc_= varData("SC","SC","m^2","Wetted surface area of canoe body"); //> Wetted surface area of canoe body
varData Var::cms_= varData("CMS","CMS"," ","Midship section coefficient"); //> Midship section coefficient
varData Var::tcan_= varData("TCAN","TCAN","m","Draft of canoe body"); //> Draft of canoe body
varData Var::alt_= varData("ALT","ALT","m^2","Total lateral area of yacht"); //> Total lateral area of yacht
varData Var::kg_= varData("KG","KG","m","Center of gravity above moulded base or keel"); //> Center of gravity above moulded base or keel
varData Var::km_= varData("KM","KM","m","Transverse metacentre above moulded base or keel"); //> Transverse metacentre above moulded base or keel
// Crew Settings
varData Var::mmvblcrw_= varData("MMVBLCRW","MMVBLCRW","Kg","Movable Crew Mass"); //> Movable Crew Mass
// Keel Settings
varData Var::dvk_("DVK","DVK","m^3","Displaced volume of keel"); //> Displaced volume of keel
varData Var::apk_("APK","APK","m^2","Keel planform area"); //> Keel planform area
varData Var::ask_("ASK","ASK","m^2","Keel Aspect Ratio"); //> Keel Aspect Ratio
varData Var::sk_("SK","SK","m^2","Keel Wetted Surface"); //> Keel Wetted Surface
varData Var::zcbk_("ZCBK","ZCBK","m","Keel Vertical Center of Buoyancy"); //> Keel Vertical Center of Buoyancy
varData Var::chmek_("CHMEK","CHMEK","m","Mean chord length"); //> Mean chord length
varData Var::chrtk_("CHRTK","CHRTK","m","Root chord length"); //> Tip chord length
varData Var::chtpk_("CHTPK","CHTPK","m","Tip chord length"); //> Tip chord length
varData Var::keelff_("KEELFF","KEELFF"," ","Keel's form factor"); //> Keel's form factor
varData Var::delttk_("DELTTK","DELTTK"," ","Mean thickness ratio of keel section"); //> Mean thickness ratio of keel section
varData Var::tak_("TAK","TAK"," ","Taper ratio of keel (CHRTK/CHTPK)"); //> Taper ratio of keel (CHRTK/CHTPK)
// Rudder Settings
varData Var::dvr_("DVR","DVR","m^3","Rudder displaced volume"); //> Rudder displaced volume
varData Var::apr_("APR","APR","m^2","Rudder planform area"); //> Rudder planform area
varData Var::sr_("SR","SR","m^2","Rudder wetted surface"); //> Rudder wetted surface
varData Var::chmer_("CHMER","CHMER","m","Mean chord length"); //> Rudder Mean chord length
varData Var::chrtr_("CHRTR","CHRTR","m","Root chord length"); //> Rudder Root chord length
varData Var::chtpr_("CHTPR","CHTPR","m","Tip chord length"); //> Rudder Tip chord length
varData Var::delttr_("DELTTR","DELTTR","m","Mean thickness ratio of rudder section"); //> Mean thickness ratio of rudder section
varData Var::ruddff_("RUDDFF","RUDDFF"," ","Rudder form factor"); //> Rudder form factor
/// Sail Settings
varData Var::p_("P","P","m","Main height"); //> Main height
varData Var::e_("E","E","m","Main base"); //> Main base
varData Var::mroach_("MROACH","MROACH"," ","Correction for mainsail roach"); //> Correction for mainsail roach
varData Var::bad_("BAD","BAD","m","Boom height above deck"); //> Boom height above deck
varData Var::i_("I","I","m","Fore Triangle height"); //> Fore Triangle height
varData Var::j_("J","J","m","Fore Triangle base"); //> Fore Triangle base
varData Var::lpg_("LPG","LPG","m","Perpendicular of longest jib"); //> Perpendicular of longest jib
varData Var::sl_("SL","SL","m","Spinnaker length"); //> Spinnaker length
varData Var::ehm_("EHM","EHM","m","Mast height above deck"); //> Mast height above deck
varData Var::emdc_("EMDC","EMDC","m","Mast average diameter"); //> Mast average diameter
//
varData Var::mflb_("MainBattens","MFLB"," ","Battens configuration"); //> Full main battens in main
varData Var::sailSet_("SailSet","SAILSET"," ","Sail configuration"); //> Sail configuration
//
varData Var::an_("AN","AN","m^2","Nominal sail area");
varData Var::am_("AM","AM","m^2","MainSail area"); //> mainSail area
varData Var::aj_("AJ","AJ","m^2","Jib area"); //> jib area
varData Var::as_("AS","AS","m^2","Spinnaker area"); //> Spinnaker area
varData Var::af_("AF","AF","m^2","Fore triangle area"); //> Fore triangle area
varData Var::zce_("ZCE","ZCE","m","Nominal sail center height "); //> Nominal Sail center's height
varData Var::zcem_("ZCEM","ZCEM","m","MainSail center height "); //> MainSail center's height
varData Var::zcej_("ZCEJ","ZCEJ","m","Jib center height"); //> Jib center's height
varData Var::zces_("ZCES","ZCES","ZCES","Spinnaker center height"); //> Spinnaker center's height



