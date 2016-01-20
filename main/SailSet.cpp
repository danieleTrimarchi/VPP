#include "SailSet.h"
#include "VPPAeroItem.h"
#include "cmath"

// Constructor
SailSet::SailSet(const VariableFileParser& parser) :
	pParser_(&parser) {

	const VarSet& v = *(pParser_->getVariables());

	// Compute the mainSail area AM =  0.5 * P * E * MROACH
	sailVariables_.insert( Variable("AM", 0.5 * v["P"] * v["E"] * v["MROACH"] ) );

	// Compute the Jib area AJ =  0.5 * sqrt( I^2 + J^2) * LPG
	sailVariables_.insert( Variable("AJ", 0.5 * std::sqrt( v["I"] * v["I"] + v["J"] * v["J"] ) * v["LPG"] ) );

	// Compute the Spinnaker area AS = 1.15 * SL * J;
	sailVariables_.insert( Variable("AS", 1.15 * v["SL"] * v["J"] ) );

	// Compute the Fore Triangle area AF =  0.5 * I * J
	sailVariables_.insert( Variable("AF", 0.5 * v["I"] * v["J"] ) );

	// Compute heights :

	// Compute the MainSail center's height ZCEM = 0.39 * P + BAD
	sailVariables_.insert( Variable("ZCEM", 0.39 * v["P"] + v["BAD"] ) );

	// Compute the Jib center's height ZCEJ = 0.39 .* geom.I;
	sailVariables_.insert( Variable("ZCEJ", 0.39 * v["I"] ) );

	// Compute the Spinnaker center's height ZCES = 0.59 * I;
	sailVariables_.insert( Variable("ZCES", 0.59 * v["I"] ) );

}

/// Factory method
SailSet* SailSet::SailSetFactory(VariableFileParser& parser){

	// SAILSET VALUES: 1: main only; 3 - main & jib; 5 - main & spi; 7 - main, jib, & spinnaker;
	switch( int(parser.get("SAILSET")) )
	{
	case sailConfig::mainOnly :
		return new MainOnlySailSet(parser);

	case sailConfig::mainAndJib :
		return new MainAndJibSailSet(parser);

	case sailConfig::mainAndSpi:
		return new MainAndSpiSailSet(parser);

	case sailConfig::mainJibAndSpi:
		return new MainJibAndSpiSailSet(parser);

	default:
		char msg[256];
		sprintf(msg,"The value of SAILSET: \"%d\" is not supported",int(parser.get("SAILSET")));
		throw std::logic_error(msg);
	}
}


// Destructor
SailSet::~SailSet() {

}


/// Printout the list of all variables we have computed
void SailSet::printVariables() {

	std::cout<<"====== PRINTOUT SAILSET VARIABLES ================ "<<std::endl;
	sailVariables_.print();
	std::cout<<"==================================================\n"<<std::endl;

}

// Get the value of a variable
double SailSet::get(std::string varName) {
	return sailVariables_[varName];
}

//////////////////////////////////////////////////////////

// Constructor
MainOnlySailSet::MainOnlySailSet(const VariableFileParser& parser) :
	SailSet(parser) {

	// Compute the Sail Nominal Area AN = AM
	sailVariables_.insert( Variable("AN", sailVariables_["AM"] ) );

	// Compute the Sail Nominal Height ZCE = ZCEM
	sailVariables_.insert( Variable("ZCE", sailVariables_["ZCEM"] ) );

}

// Return the Id of this SailSet (this is the enum sailConfig)
size_t MainOnlySailSet::getType() {
	return sailConfig::mainOnly;
}

// Make a new SailCoefficientItem of the type required for
// this sailSet
SailCoefficientItem* MainOnlySailSet::sailCoefficientItemFactory(WindItem* pWind) {
	return new MainOnlySailCoefficientItem(pWind);
}

//////////////////////////////////////////////////////////

// Constructor
MainAndJibSailSet::MainAndJibSailSet(const VariableFileParser& parser) :
	SailSet(parser) {

	// use a tmp var to refer to sailVariables_
	VarSet& sv = sailVariables_;

	// Compute the Sail Nominal Area AN = AF + AM
	sailVariables_.insert( Variable("AN", sv["AF"] + sv["AM"] ) );

	// Compute the Sail Nominal Height ZCE = (ZCEM .* AM + ZCEJ .* AJ) ./ (AM + AJ);
	sailVariables_.insert( Variable("ZCE",
			( sv["ZCEM"] * sv["AM"] + sv["ZCEJ"] * sv["AJ"] ) / ( sv["AM"] + sv["AJ"] ) ) );

}

// Return the Id of this SailSet (this is the enum sailConfig)
size_t MainAndJibSailSet::getType() {
	return sailConfig::mainAndJib;
}

// Make a new SailCoefficientItem of the type required for
// this sailSet
SailCoefficientItem* MainAndJibSailSet::sailCoefficientItemFactory(WindItem* pWind) {
	return new MainAndJibCoefficientItem(pWind);
}

//////////////////////////////////////////////////////////

// Constructor
MainAndSpiSailSet::MainAndSpiSailSet(const VariableFileParser& parser):
	SailSet(parser) {

	// use a tmp var to refer to sailVariables_
	VarSet& sv = sailVariables_;

	// Compute the Sail Nominal Area AN = AS + AM
	sailVariables_.insert( Variable("AN", sv["AS"] + sv["AM"] ) );

	// Compute the Sail Nominal Height ZCE = (ZCEM .* AM + ZCES .* AS) ./ (AM + AS)
	sailVariables_.insert( Variable("ZCE",
			(sv["ZCEM"] * sv["AM"] + sv["ZCES"] * sv["AS"] ) / ( sv["AM"] + sv["AS"] ) ) );

}

// Return the Id of this SailSet (this is the enum sailConfig)
size_t MainAndSpiSailSet::getType() {
	return sailConfig::mainAndSpi;
}

// Make a new SailCoefficientItem of the type required for
// this sailSet
SailCoefficientItem* MainAndSpiSailSet::sailCoefficientItemFactory(WindItem* pWind) {
	return new MainAndSpiCoefficientItem(pWind);
}

//////////////////////////////////////////////////////////

// Constructor
MainJibAndSpiSailSet::MainJibAndSpiSailSet(const VariableFileParser& parser):
	SailSet(parser) {

	// use a tmp var to refer to sailVariables_
	VarSet& sv = sailVariables_;

	// Compute the Sail Nominal Area AN = AF + AS + AM
	sailVariables_.insert( Variable("AN", sv["AF"] + sv["AS"] + sv["AM"] ) );

	// Compute the Sail Nominal Height ZCE = (ZCEM .* AM + ZCEJ .* AJ + ZCES .* AS) ./ (AM + AJ + AS)
	sailVariables_.insert( Variable("ZCE",
			(sv["ZCEM"] * sv["AM"] + sv["ZCEJ"] * sv["AJ"] + sv["ZCES"] * sv["AS"] ) /
			( sv["AM"] + sv["AJ"] + sv["AS"] ) ) );

}

// Return the Id of this SailSet (this is the enum sailConfig)
size_t MainJibAndSpiSailSet::getType() {
	return sailConfig::mainJibAndSpi;
}

// Make a new SailCoefficientItem of the type required for
// this sailSet
SailCoefficientItem* MainJibAndSpiSailSet::sailCoefficientItemFactory(WindItem* pWind) {
	return new MainJibAndSpiCoefficientItem(pWind);
}

