#include "SailSet.h"
#include "VPPAeroItem.h"
#include "cmath"
#include "VPPException.h"

// Constructor
SailSet::SailSet(const VariableFileParser& parser) :
pParser_(&parser) {

	const VarSet& v = *(pParser_->getVariables());

	// Compute the mainSail area AM =  0.5 * P * E * MROACH
	sailVariables_.insert( Variable(Var::am_, 0.5 * v[Var::p_] * v[Var::e_] * v[Var::mroach_] ) );

	// Compute the Jib area AJ =  0.5 * sqrt( I^2 + J^2) * LPG
	sailVariables_.insert( Variable(Var::aj_, 0.5 * sqrt( v[Var::i_] * v[Var::i_] + v[Var::j_] * v[Var::j_] ) * v["LPG"] ) );

	// Compute the Spinnaker area AS = 1.15 * SL * J;
	sailVariables_.insert( Variable(Var::as_, 1.15 * v[Var::sl_] * v[Var::sl_] ) );

	// Compute the Fore Triangle area AF =  0.5 * I * J
	sailVariables_.insert( Variable(Var::af_, 0.5 * v[Var::i_] * v[Var::j_] ) );

	// Compute heights :

	// Compute the MainSail center's height ZCEM = 0.39 * P + BAD
	sailVariables_.insert( Variable(Var::zcem_, 0.39 * v[Var::p_] + v[Var::bad_] ) );

	// Compute the Jib center's height ZCEJ = 0.39 .* geom.I;
	sailVariables_.insert( Variable(Var::zcej_, 0.39 * v[Var::i_] ) );

	// Compute the Spinnaker center's height ZCES = 0.59 * I;
	sailVariables_.insert( Variable(Var::zces_, 0.59 * v[Var::i_] ) );

}

/// Factory method
SailSet* SailSet::SailSetFactory(VariableFileParser& parser){

	// SAILSET VALUES: 1: main only; 3 - main & jib; 5 - main & spi; 7 - main, jib, & spinnaker;
	switch( int(parser.get(Var::sailSet_)) )
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
		sprintf(msg,"The value of SAILSET: \"%d\" is not supported",int(parser.get(Var::sailSet_)));
		throw std::logic_error(msg);
	}
}


// Destructor
SailSet::~SailSet() {

}

// Get the value of a variable
double SailSet::get(string varName) {
	return sailVariables_[varName];
}

// Populate the tree model that will be used to
// visualize the variables in the UI
void SailSet::populate(VariableTreeModel* pTreeModel) {
	sailVariables_.populate(pTreeModel);
}

//////////////////////////////////////////////////////////

// Constructor
MainOnlySailSet::MainOnlySailSet(const VariableFileParser& parser) :
			SailSet(parser) {

	// Compute the Sail Nominal Area AN = AM
	sailVariables_.insert( Variable(Var::an_, sailVariables_[Var::am_] ) );

	// Compute the Sail Nominal Height ZCE = ZCEM
	sailVariables_.insert( Variable(Var::zce_, sailVariables_[Var::zcem_] ) );

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
	sailVariables_.insert( Variable(Var::an_, sv[Var::af_] + sv[Var::am_] ) );

	// Compute the Sail Nominal Height ZCE = (ZCEM .* AM + ZCEJ .* AJ) ./ (AM + AJ);
	sailVariables_.insert( Variable(Var::zce_,
			( sv[Var::zcem_] * sv[Var::am_] + sv[Var::zcej_] * sv[Var::aj_] ) / ( sv[Var::am_] + sv[Var::aj_] ) ) );

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
	sailVariables_.insert( Variable(Var::an_, sv[Var::as_] + sv[Var::am_] ) );

	// Compute the Sail Nominal Height ZCE = (ZCEM .* AM + ZCES .* AS) ./ (AM + AS)
	sailVariables_.insert( Variable(Var::zce_,
			(sv[Var::zcem_] * sv[Var::am_] + sv[Var::zces_] * sv[Var::as_] ) / ( sv[Var::am_] + sv[Var::as_] ) ) );

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
	sailVariables_.insert( Variable(Var::an_, sv[Var::af_] + sv[Var::as_] + sv[Var::am_] ) );

	// Compute the Sail Nominal Height ZCE = (ZCEM .* AM + ZCEJ .* AJ + ZCES .* AS) ./ (AM + AJ + AS)
	sailVariables_.insert( Variable(Var::zce_,
			(sv[Var::zcem_] * sv[Var::am_] + sv[Var::zcej_] * sv[Var::aj_] + sv[Var::zces_] * sv[Var::as_] ) /
			( sv[Var::am_] + sv[Var::aj_] + sv[Var::as_] ) ) );

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


