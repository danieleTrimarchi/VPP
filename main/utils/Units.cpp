#include "Units.h"

using namespace std;

//Unit base interface class. The Unit class
//represents the Measure Units used by the
//program and the relations between those.
Unit::Unit(){
}

// Dtor
Unit::~Unit(){

}

//////////////////////////////////////////////////////

//Dimensionless unit, used for a-dimensional
//quantities
NoUnit::NoUnit() {
}

//Dtor
NoUnit::~NoUnit() {

}

//Returns the name of this unit as a string
string NoUnit::getUnitName() {
	return string("");
};

//////////////////////////////////////////////////////

//Angular Unit base interface class. This class
//represents the angular Unit, such as Radians
//and degrees.
AngularUnit::AngularUnit() {
}

//Dtor
AngularUnit::~AngularUnit() {
}

//////////////////////////////////////////////////////

//This class represents the angular Unit degree
Degrees::Degrees(){
}

//Dtor
Degrees::~Degrees() {
}

//Returns the name of this unit as a string
string Degrees::getUnitName() {
	return string("deg");

};

//////////////////////////////////////////////////////

//This class represents the angular Unit Radians
Radians::Radians(){
}

//Dtor
Radians::~Radians(){
}

//Returns the name of this unit as a string
string Radians::getUnitName(){
	return string("rad");
}

//////////////////////////////////////////////////////

//This class represents the Linear distance unit,
//such as for example the meter
LinearDistanceUnit::LinearDistanceUnit() {
}

//Dtor
LinearDistanceUnit::~LinearDistanceUnit() {
}

//////////////////////////////////////////////////////

//Ctor
Meters::Meters(){
}

//Dtor
Meters::~Meters(){
}

//Returns the name of this unit as a string
string Meters::getUnitName(){
	return string("m");
}

//////////////////////////////////////////////////////

//This class represents the Velocity unit,
//such as for example m/s
VelocityUnit::VelocityUnit() {
}

//Dtor
VelocityUnit::~VelocityUnit(){

}

//////////////////////////////////////////////////////

//This class represents the m/s
MetersPerSec::MetersPerSec() {
}

//Dtor
MetersPerSec::~MetersPerSec() {
}

//Returns the name of this unit as a string
string MetersPerSec::getUnitName(){
	return string("m/s");
}

//////////////////////////////////////////////////////

//This class represents the Surface unit,
//such as for example m^2
SurfaceUnit::SurfaceUnit(){
}

//Dtor
SurfaceUnit::~SurfaceUnit(){
}

//////////////////////////////////////////////////////

//This class represents the m^2
SquareMeters::SquareMeters() {
}

//Dtor
SquareMeters::~SquareMeters(){
}

//Returns the name of this unit as a string
string SquareMeters::getUnitName(){
	return string("m^2");
};

//////////////////////////////////////////////////////

//This class represents the Volume unit,
//such as for example m^3
VolumeUnit::VolumeUnit() {
}

//Dtor
VolumeUnit::~VolumeUnit(){
}

//////////////////////////////////////////////////////

//This class represents the m^3
CubeMeters::CubeMeters(){
}

//Dtor
CubeMeters::~CubeMeters(){
}

//Returns the name of this unit as a string
string CubeMeters::getUnitName(){
	return string("m^3");
};

//////////////////////////////////////////////////////

//Ctor
WeightUnit::WeightUnit(){
}

//Dtor
WeightUnit::~WeightUnit() {
}

//////////////////////////////////////////////////////

//This class represents the m^3
Kilograms::Kilograms(){
}

//Dtor
Kilograms::~Kilograms(){
}

//Returns the name of this unit as a string
string Kilograms::getUnitName(){
	return string("Kg");
}
