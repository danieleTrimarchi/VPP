#include "SettingsColumn.h"
#include <QtWidgets/QLineEdit>

// Ctor
SettingsColumn::SettingsColumn() {

}

// Copy Ctor
SettingsColumn::SettingsColumn(const SettingsColumn& rhs) {
	data_ = rhs.data_;
}

// Dtor
SettingsColumn::~SettingsColumn() {

}

// Set the data stored in this column
void SettingsColumn::setData(const QVariant& data){
	data_ =	data;
}

// Get the data stored in this column
QVariant SettingsColumn::getData(const int role /*=0*/) const {
	return data_;
}

// Get the backGround color for the items of this
// column
QColor SettingsColumn::getBackGroundColor() const {
	return QColor(228,228,228);
}

// Comparison operator
bool SettingsColumn::operator==(const SettingsColumn& rhs) {
	if(data_ == rhs.data_)
		return true;

	return false;
}

// Inverse Comparison operator
bool SettingsColumn::operator!=(const SettingsColumn& rhs){
	return !(*this==rhs);
}


//-----------------------------------------

// Ctor
NameColumn::NameColumn() :
			SettingsColumn() {

}

// Copy Ctor
NameColumn::NameColumn(const NameColumn& rhs) :
			SettingsColumn(rhs){

}

// Dtor
NameColumn::~NameColumn() {

}

// Clone this item, which is basically equivalent to calling the copy ctor
NameColumn* NameColumn::clone() const {
	return new NameColumn(*this);
}

// Is this column editable in the view?
Qt::ItemFlag NameColumn::editable() const {
	return  Qt::NoItemFlags;
}

//-----------------------------------------

// Ctor
ValueColumn::ValueColumn() :
			SettingsColumn() {

}

// Copy Ctor
ValueColumn::ValueColumn(const ValueColumn& rhs):
				SettingsColumn(rhs){

}

// Dtor
ValueColumn::~ValueColumn() {

}

// Clone this item, which is basically equivalent to calling the copy ctor
ValueColumn* ValueColumn::clone() const {
	return new ValueColumn(*this);
}

// Is this column editable in the view?
Qt::ItemFlag ValueColumn::editable() const {
	return Qt::ItemIsEditable;
}

// Get the backGround color for the items of this
// column
QColor ValueColumn::getBackGroundColor() const {
	return QColor(Qt::white);
}

//-----------------------------------------

// Ctor
UnitColumn::UnitColumn() :
			SettingsColumn() {

}

// Copy Ctor
UnitColumn::UnitColumn(const UnitColumn& rhs) :
				SettingsColumn(rhs) {

}

// Dtor
UnitColumn::~UnitColumn() {

}

// Clone this item, which is basically equivalent to calling the copy ctor
UnitColumn* UnitColumn::clone() const {
	return new UnitColumn(*this);
}

// Is this column editable in the view?
Qt::ItemFlag UnitColumn::editable() const {
	return  Qt::NoItemFlags;
}


