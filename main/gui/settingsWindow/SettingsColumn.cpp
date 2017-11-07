#include "SettingsColumn.h"
#include <QLineEdit>

// Ctor
SettingsColumn::SettingsColumn() {

}

// Dtor
SettingsColumn::~SettingsColumn() {

}

// Set the data stored in this column
void SettingsColumn::setData(const QVariant& data){
	data_ =	data;
}

// Get the backGround color for the items of this
// column
QColor SettingsColumn::getBackGroundColor() const {
	return QColor(228,228,228);
}

//-----------------------------------------

// Ctor
NameColumn::NameColumn() :
	SettingsColumn() {

}

// Dtor
NameColumn::~NameColumn() {

}

// Get the data stored in this column
QVariant NameColumn::getData(const int role /*=0*/) {

	return data_;
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

// Dtor
ValueColumn::~ValueColumn() {

}

// Get the data stored in this column
QVariant ValueColumn::getData(const int role /*=0*/) {

	return data_;

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

// Dtor
UnitColumn::~UnitColumn() {

}

// Get the data stored in this column
QVariant UnitColumn::getData(const int role /*=0*/) {

	return data_;

}

// Is this column editable in the view?
Qt::ItemFlag UnitColumn::editable() const {
	return  Qt::NoItemFlags;
}


