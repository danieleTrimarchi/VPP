#ifndef ENUM_DATA_H
#define ENUM_DATA_H

#include <stddef.h>
#include <string>

/// Generic container for index and label.
/// This class is used as underlying container for
/// all multi-purpose enum allowing for getting
/// an index as well as a label
struct enumData {

		/// Ctor
		enumData(size_t idx, const char* name):
		 idx_(idx),
		 label_(name){};

		/// Index
		size_t idx_;

		/// Label
		const char* label_;
};

//////////////////////////////////////////////

/// Enum defined for the Settings Window columns
struct colNames {
		static enumData name_; //> Name column
		static enumData value_;//> Value column
		static enumData unit_; //> Unit column
};

//////////////////////////////////////////////
/// Define a struct holding the data of the variables.
/// Its displayName, internalName, unit and tooltip
/// These data will be used to instantiate variables
/// in the settings window, as well as in the variableFileParser
class varData {

	public :
		/// Ctor
		varData(	const char* display,
						const char* name,
						const char* unit,
						const char* tooltip);

		/// Self cast operator, returns the underlying value
		/// See https://msdn.microsoft.com/en-us/library/wwywka61.aspx
		operator std::string() const;

		const char* display_;//> Display variable name
		const char* name_;		//> Internal variable name
		const char* unit_;		//> Variable unit
		const char* tootip_;		//> Variable tooltip
};

class varDataBounds : public varData {

	public :
		varDataBounds(	const char* display,
				const char* name,
				const char* unit,
				const char* tooltip);

		std::string min_; //> Display variable name for the min of the bound
		std::string max_; //> Display variable name for the max of the bound

};


/// Define all the possible variables required by the vpp
/// This set is required and sufficient
struct Var {
		/// Analysis Settings
		static varDataBounds vBounds_;		//> Velocity bounds
		static varDataBounds heelBounds_; //> Heel angle bounds
		static varDataBounds crewBounds_; //> Crew position bounds
		static varDataBounds flatBounds_; //> Sail flat bounds
		/// Wind Settings
		static varDataBounds vtwBounds_; //> Wind speed bounds
		static varData ntw_; //> Number of wind speeds
		static varDataBounds atwBounds_; //> Wind angle bounds
		static varData nta_; //> Number of wind angles
		/// Hull Settings
		static varData lwl_; //> Waterline lenght
		static varData bwl_; //> Waterline beam
		static varData b_;   //> Design max beam
		static varData t_;   //> Design max draft
		static varData divCan_; //> Volume of the canoe body
		static varData avgfreb_; //> Average freeboard height
		static varData xfb_; //> Longitudinal center of buoyancy LCB from fpp
		static varData xff_; //> Longitudinal center of flotation LCF from fpp
		static varData cpl_; //> Longitudinal prismatic coefficient
		static varData hullff_; //> Hull form factor
		static varData aw_; //> Design waterplane area
		static varData sc_; //> Wetted surface area of canoe body
		static varData cms_; //> Midship section coefficient
		static varData tcan_; //> Draft of canoe body
		static varData alt_; //> Total lateral area of yacht
		static varData kg_; //> Center of gravity above moulded base or keel
		static varData km_; //> Transverse metacentre above moulded base or keel
		/// Crew Settings
		static varData mmvblcrw_; //> Movable Crew Mass
		/// Keel Settings
		static varData dvk_; //> Displaced volume of keel
		static varData apk_; //> Keel planform area
		static varData ask_; //> Keel Aspect Ratio
		static varData sk_; //> Keel Wetted Surface
		static varData zcbk_; //> Keel Vertical Center of Buoyancy
		static varData chmek_; //> Mean chord length
		static varData chrtk_; //> Root chord length
		static varData chtpk_; //> Tip chord length
		static varData keelff_; //> Keel's form factor
		static varData delttk_; //> Mean thickness ratio of keel section
		static varData tak_; //> Taper ratio of keel (CHRTK/CHTPK)
		/// Rudder Settings
		static varData dvr_; //> Rudder displaced volume
		static varData apr_; //> Rudder planform area
		static varData sr_; //> Rudder wetted surface
		static varData chmer_; //> Rudder Mean chord length
		static varData chrtr_; //> Rudder Root chord length
		static varData chtpr_; //> Rudder Tip chord length
		static varData delttr_; //> Mean thickness ratio of rudder section
		static varData ruddff_; //> Rudder form factor
		/// Sail Settings
		static varData p_; //> Main height
		static varData e_; //> Main base
		static varData mroach_; //> Correction for mainsail roach
		static varData bad_; //> Boom height above deck
		static varData i_; //> Fore Triangle height
		static varData j_; //> Fore Triangle base
		static varData lpg_; //> Perpendicular of longest jib
		static varData sl_; //> Spinnaker length
		static varData ehm_; //> Mast height above deck
		static varData emdc_; //> Mast average diameter
		static varData mflb_; //> Full main battens in main
		static varData sailSet_; //> Sail configuration
		//
		static varData an_; //> Nominal sail area
		static varData am_; //> MainSail area
		static varData aj_; //> Jib area
		static varData as_; //> Spinnaker area
		static varData af_; //> Fore triangle area
		static varData zce_; //> Nominal sail center's height
		static varData zcem_; //> MainSail center's height
		static varData zcej_; //> Jib center's height
		static varData zces_; //> Spinnaker center's height

};

#endif
