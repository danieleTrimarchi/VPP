#include "DataColumn.h"

#include <QtWidgets/QLineEdit>

// Ctor
DataColumn::DataColumn() {

}

// Copy Ctor
DataColumn::DataColumn(const DataColumn& rhs) {
	data_ = rhs.data_;
}

// Dtor
DataColumn::~DataColumn() {

}

// Set the data stored in this column
void DataColumn::setData(const QVariant& data){
	data_ =	data;
}

// Get the data stored in this column
QVariant DataColumn::getData(const int role /*=0*/) const {
	return data_;
}

// Get the backGround color for the items of this
// column
QColor DataColumn::getBackGroundColor() const {
	return QColor(228,228,228);
}

// Comparison operator
bool DataColumn::operator==(const DataColumn& rhs) {
	if(data_ == rhs.data_)
		return true;

	return false;
}

// Inverse Comparison operator
bool DataColumn::operator!=(const DataColumn& rhs){
	return !(*this==rhs);
}

std::string DataColumn::getDisplayName() const {
	return columnDisplayName_;
}


//-----------------------------------------

// Ctor
NameColumn::NameColumn() :
			DataColumn() {
			columnDisplayName_= colNames::name_.label_;
}

// Value Ctor
NameColumn::NameColumn(QVariant value) :
					DataColumn() {

}


// Copy Ctor
NameColumn::NameColumn(const NameColumn& rhs) :
			DataColumn(rhs){
	/* Do nothing as the name is copied over by
	 * the base class */
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
			DataColumn() {
			columnDisplayName_= colNames::value_.label_;
}

// Value Ctor
ValueColumn::ValueColumn(QVariant value):
					DataColumn() {
	data_=value;
}


// Copy Ctor
ValueColumn::ValueColumn(const ValueColumn& rhs):
				DataColumn(rhs){

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
			DataColumn() {
	columnDisplayName_=colNames::unit_.label_;
}

// Copy Ctor
UnitColumn::UnitColumn(const UnitColumn& rhs) :
				DataColumn(rhs) {

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


