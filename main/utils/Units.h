#ifndef UNIT_H
#define UNIT_H

#include <string>
using namespace std;

/// Unit base interface class. The Unit class
/// represents the Measure Units used by the
/// program and the relations between those.
class Unit {

	public:

		/// Ctor
		Unit();

		/// Dtor
		virtual ~Unit();

		/// Returns the name of this unit as a string
		virtual string getUnitName() =0;

};

//////////////////////////////////////////////////////

/// Dimensionless unit, used for a-dimensional
/// quantities
class NoUnit : public Unit {

	public:

		/// Ctor
		NoUnit();

		/// Dtor
		virtual ~NoUnit();

		/// Returns the name of this unit as a string
		virtual string getUnitName();

};

//////////////////////////////////////////////////////

/// Angular Unit base interface class. This class
/// represents the angular Unit, such as Radians
/// and degrees.
class AngularUnit : public Unit {

	public:

		/// Ctor
		AngularUnit();

		/// Dtor
		virtual ~AngularUnit();

		/// Returns the name of this unit as a string
		virtual string getUnitName() =0;

};

//////////////////////////////////////////////////////

/// This class represents the angular Unit degree
class Degrees : public AngularUnit {

	public:

		/// Ctor
		Degrees();

		/// Dtor
		~Degrees();

		/// Returns the name of this unit as a string
		virtual string getUnitName();

};

//////////////////////////////////////////////////////

/// This class represents the angular Unit Radians
class Radians : public AngularUnit{

	public:

		/// Ctor
		Radians();

		/// Dtor
		~Radians();

		/// Returns the name of this unit as a string
		virtual string getUnitName();

};

//////////////////////////////////////////////////////

/// This class represents the Linear distance unit,
/// such as for example the meter
class LinearDistanceUnit : public Unit {

	public:

		/// Ctor
		LinearDistanceUnit();

		/// Dtor
		~LinearDistanceUnit();

		/// Returns the name of this unit as a string
		virtual string getUnitName() =0;

};

//////////////////////////////////////////////////////

/// This class represents the Meter unit
class Meters : public LinearDistanceUnit {

	public:

		/// Ctor
		Meters();

		/// Dtor
		~Meters();

		/// Returns the name of this unit as a string
		virtual string getUnitName();

};

//////////////////////////////////////////////////////

/// This class represents the Velocity unit,
/// such as for example m/s
class VelocityUnit : public Unit {

	public:

		/// Ctor
		VelocityUnit();

		/// Dtor
		~VelocityUnit();

		/// Returns the name of this unit as a string
		virtual string getUnitName() =0;

};

//////////////////////////////////////////////////////

/// This class represents the m/s
class MetersPerSec : public VelocityUnit {

	public:

		/// Ctor
		MetersPerSec();

		/// Dtor
		~MetersPerSec();

		/// Returns the name of this unit as a string
		virtual string getUnitName();

};

//////////////////////////////////////////////////////

/// This class represents the Surface unit,
/// such as for example m^2
class SurfaceUnit : public Unit {

	public:

		/// Ctor
		SurfaceUnit();

		/// Dtor
		~SurfaceUnit();

		/// Returns the name of this unit as a string
		virtual string getUnitName() =0;

};

//////////////////////////////////////////////////////

/// This class represents the m^2
class SquareMeters : public SurfaceUnit {

	public:

		/// Ctor
		SquareMeters();

		/// Dtor
		~SquareMeters();

		/// Returns the name of this unit as a string
		virtual string getUnitName();

};

//////////////////////////////////////////////////////

/// This class represents the Volume unit,
/// such as for example m^3
class VolumeUnit : public Unit {

	public:

		/// Ctor
		VolumeUnit();

		/// Dtor
		~VolumeUnit();

		/// Returns the name of this unit as a string
		virtual string getUnitName() =0;

};

//////////////////////////////////////////////////////

/// This class represents the m^3
class CubeMeters : public VolumeUnit {

	public:

		/// Ctor
		CubeMeters();

		/// Dtor
		~CubeMeters();

		/// Returns the name of this unit as a string
		virtual string getUnitName();

};

//////////////////////////////////////////////////////

/// This class represents the Volume unit,
/// such as for example m^3
class WeightUnit : public Unit {

	public:

		/// Ctor
		WeightUnit();

		/// Dtor
		~WeightUnit();

		/// Returns the name of this unit as a string
		virtual string getUnitName() =0;

};

//////////////////////////////////////////////////////

/// This class represents the m^3
class Kilograms : public WeightUnit {

	public:

		/// Ctor
		Kilograms();

		/// Dtor
		~Kilograms();

		/// Returns the name of this unit as a string
		virtual string getUnitName();

};

#endif
