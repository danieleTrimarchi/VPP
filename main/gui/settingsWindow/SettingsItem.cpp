#include "SettingsItem.h"
//#include <QtWidgets/QLineEdit>
//#include <QtWidgets/QSpinBox>
//#include <QtGui/QColor>
#include <iostream>
#include "VppSettingsXmlWriter.h"
#include "VppSettingsXmlReader.h"
#include "VariableFileParser.h"
#include "Units.h"

using namespace std;

// Init static members
const string SettingsItemBase::className_("SettingsItemBase");
const string SettingsItemRoot::className_("SettingsItemRoot");
const string SettingsItemGroup::className_("SettingsItemGroup");

// Ctor
SettingsItemBase::SettingsItemBase() :
											Item(),
											tooltip_(QVariant()),
											variableName_(QString()){

	// Instantiate the data columns
	columns_.push_back(new NameColumn);
	columns_.push_back(new ValueColumn);
	columns_.push_back(new UnitColumn);

	// Set this item as editable. Override the default
	// value assigned in the parent class constructor
	editable_ = Qt::ItemIsEditable;
}

// Copy Ctor
SettingsItemBase::SettingsItemBase(const SettingsItemBase& rhs):
								Item(rhs){

	tooltip_= rhs.tooltip_;

	// Deep copy the variableName for the variableFileParser
	variableName_ = rhs.variableName_;

	// Cleanup the current cols
	for(size_t iCol=0; iCol<columns_.size(); iCol++){
		delete columns_[iCol];
	}
	columns_.clear();

	// Deep copy the columns
	columns_.resize(rhs.columns_.size());
	for(size_t i=0; i<columns_.size(); i++)
		columns_[i] = rhs.columns_[i]->clone();

	// Also deep copy the children!
	children_.clear();
	for(size_t iChild=0; iChild<rhs.children_.size(); iChild++)
		children_.append( rhs.children_[iChild]->clone() );

	// Make sure the children have knowledge of their parent
	for(size_t iChild=0; iChild<children_.size(); iChild++)
		children_[iChild]->setParent(this);
}

// Static Factory method - builds a SettingsItem from the attributes
// read from xml and stored into an appropriate set
SettingsItemBase* SettingsItemBase::settingsItemFactory(const XmlAttributeSet& attSet){

	// What class am I going to instantiate?
	string className = (attSet["ClassName"]).getString();
	string unit = (attSet["Unit"]).getString();

	// Instantiate the units to get their signature
	NoUnit noUnit;
	Meters meters;
	MetersPerSec metersPerSec;
	Degrees degrees;
	SquareMeters squareMeters;
	CubeMeters cubeMeters;
	Kilograms kilograms;

	SettingsItemBase* pItem=0;
	// Now instantiate the item based on its type
	if(className == SettingsItemRoot::className_){
		pItem = new SettingsItemRoot;

	} else if(className == SettingsItem<NoUnit>::className_){
		if(unit == meters.getUnitName())
			pItem = new SettingsItem<Meters>(attSet);
		else if (unit == metersPerSec.getUnitName())
			pItem = new SettingsItem<MetersPerSec>(attSet);
		else if (unit == degrees.getUnitName())
			pItem = new SettingsItem<Degrees>(attSet);
		else if (unit == squareMeters.getUnitName())
			pItem = new SettingsItem<SquareMeters>(attSet);
		else if (unit == cubeMeters.getUnitName())
			pItem = new SettingsItem<CubeMeters>(attSet);
		else if (unit == kilograms.getUnitName())
			pItem = new SettingsItem<Kilograms>(attSet);
		else if (unit == noUnit.getUnitName())
			pItem = new SettingsItem<NoUnit>(attSet);

	} else if(className == SettingsItemComboBox<NoUnit>::className_){
		if(unit == meters.getUnitName())
			pItem = new SettingsItemComboBox<Meters>(attSet);
		else if (unit == metersPerSec.getUnitName())
			pItem = new SettingsItemComboBox<MetersPerSec>(attSet);
		else if (unit == degrees.getUnitName())
			pItem = new SettingsItemComboBox<Degrees>(attSet);
		else if (unit == squareMeters.getUnitName())
			pItem = new SettingsItemComboBox<SquareMeters>(attSet);
		else if (unit == cubeMeters.getUnitName())
			pItem = new SettingsItemComboBox<CubeMeters>(attSet);
		else if (unit == kilograms.getUnitName())
			pItem = new SettingsItemComboBox<Kilograms>(attSet);
		else if (unit == noUnit.getUnitName())
			pItem = new SettingsItemComboBox<NoUnit>(attSet);

	} else if(className == SettingsItemInt<NoUnit>::className_){
		if(unit == meters.getUnitName())
			pItem = new SettingsItemInt<Meters>(attSet);
		else if (unit == metersPerSec.getUnitName())
			pItem = new SettingsItemInt<MetersPerSec>(attSet);
		else if (unit == degrees.getUnitName())
			pItem = new SettingsItemInt<Degrees>(attSet);
		else if (unit == squareMeters.getUnitName())
					pItem = new SettingsItemInt<SquareMeters>(attSet);
		else if (unit == cubeMeters.getUnitName())
					pItem = new SettingsItemInt<CubeMeters>(attSet);
		else if (unit == kilograms.getUnitName())
					pItem = new SettingsItemInt<Kilograms>(attSet);
		else if (unit == noUnit.getUnitName())
			pItem = new SettingsItemInt<NoUnit>(attSet);

	} else if(className == SettingsItemGroup::className_){
		pItem = new SettingsItemGroup(attSet);

	} else if(className == SettingsItemBounds<NoUnit>::className_){
		if(unit == meters.getUnitName())
			pItem = new SettingsItemBounds<Meters>(attSet);
		else if (unit == metersPerSec.getUnitName())
			pItem = new SettingsItemBounds<MetersPerSec>(attSet);
		else if (unit == degrees.getUnitName())
			pItem = new SettingsItemBounds<Degrees>(attSet);
		else if (unit == squareMeters.getUnitName())
					pItem = new SettingsItemBounds<SquareMeters>(attSet);
		else if (unit == cubeMeters.getUnitName())
					pItem = new SettingsItemBounds<CubeMeters>(attSet);
		else if (unit == kilograms.getUnitName())
					pItem = new SettingsItemBounds<Kilograms>(attSet);
		else if (unit == noUnit.getUnitName())
			pItem = new SettingsItemBounds<NoUnit>(attSet);

	} 	else {
		char msg[256];
		sprintf(msg,"The class name: \"%s\" is not supported",className.c_str());
		throw std::logic_error(msg);
	}

	return pItem;
}

// Dtor
SettingsItemBase::~SettingsItemBase() {

	// Cleanup the columns instantiated in the ctor
	for(size_t iCol=0; iCol<columns_.size(); iCol++){
		delete columns_[iCol];
	}
	columns_.clear();

}

// Clone this item, which is basically equivalent to calling the copy ctor
SettingsItemBase* SettingsItemBase::clone() const {

	return new SettingsItemBase(*this);
}

// Get the name of the variable as this will be registered
// in the VariableFileParser
QString SettingsItemBase::getVariableName() const {
	return variableName_;
}

// Return the backGround color for this item based on the column
QColor SettingsItemBase::getBackGroundColor(int iColumn) const {

	// return grey
	return QColor(228,228,228);

}

// Returns the tooltip for this item, if any
QVariant SettingsItemBase::getToolTip() {
	return tooltip_;
}

// The item will give the Delegate the editor
// to be properly edited
QWidget* SettingsItemBase::createEditor(QWidget *parent) {

	QLineEdit *editor = new QLineEdit(parent);
	editor->setStyleSheet(	"background-color: white;"
			"selection-color: orange;");
	return editor;

}

// Edit the data in the editor
void SettingsItemBase::setEditorData(QWidget *editor, const QModelIndex&) {
	// do nothing
}

// Set the data in the model
void SettingsItemBase::setModelData(	QWidget* editor,
		QAbstractItemModel* model,
		const QModelIndex& index) const {
	// do nothing
}

// Visual options - requested by the Delegate
void SettingsItemBase::paint(QPainter* painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const {

	// Get the box where we'll draw the text.
	QRect textRect(option.rect);

	// Adjust it so the text doen't touch the border of the rectangle
	textRect.adjust(1, 1, -1, -1);

	// Draw the text using the font, color and pen specified in the data
	painter->setPen(index.data(Qt::ForegroundRole).value<QColor>());
	painter->setFont(index.data(Qt::FontRole).value<QFont>());
	QColor color = index.data(Qt::ForegroundRole).value<QColor>();

	// Write the label of this item
	painter->drawText(textRect, index.data(Qt::TextAlignmentRole).toInt(),getActiveText(index));

}

// Accept a visitor that will write this item to XML
void SettingsItemBase::accept( VPPSettingsXmlWriterVisitor& v ) {

	// Visit me
	v.visit(this);

}

// Accept a visitor that will write this item to XML
void SettingsItemBase::accept( VPPSettingsXmlReaderVisitor& v ){

	// Visit me
	v.visit(this);

}

// Accept a visitor that will write this item to the variableFileParser
void SettingsItemBase::accept( VariableParserGetVisitor& v) {

	// Visit me
	v.visit(this);
}


// Assign my children to dstParent, who will 'adopt'
// my children. After this, I will have no more children
void SettingsItemBase::assign(SettingsItemBase* dstParent) {

	// Make sure the destination parent is empty
	dstParent->clearChildren();

	// Send a copy of my children to the new parent
	for(size_t iChild=0; iChild<childCount(); iChild++)
		dstParent->appendChild(child(iChild));

	// And reset the current child list
	// do NOT delete the children with clearChildren,
	// because they are still alive, they simply changed
	// their parent
	children_.clear();

}

// Only meaningful for the combo-box item, returns zero
// for all the other items
size_t SettingsItemBase::getActiveIndex() const {
	return 0;
}

// Returns the text that must be visualized
QString SettingsItemBase::getActiveText(const QModelIndex &index) const {
	return index.data().toString();
}

// Assignment operator
const SettingsItemBase& SettingsItemBase::operator=(const SettingsItemBase& rhs) {

	// Call parent class operator
	Item::operator=(rhs);

	// Copy class-wise data
	variableName_ = rhs.variableName_;
	tooltip_= rhs.tooltip_;

	return *this;
}

// Comparison operator
bool SettingsItemBase::operator==(const SettingsItemBase& rhs) {

	// Call the parent class comparison
	if(!Item::operator==(rhs))
		return false;

	// Parent class is fine, keep comparing own members
	if( tooltip_== rhs.tooltip_ &&
			variableName_ == rhs.variableName_ ){
		return true;
	}
	return false;

}

// ----------------------------------------------------------------

// Template specialization : in the case of deg, convert
// to radians
template <>
SettingsItemBase* SettingsItem<Degrees>::convertToSI() {

	// Instantiate the new unit
	Radians radians;

	// Construct an item in the SI unit system
	SettingsItem<Radians>* pSIItem= new SettingsItem<Radians>(
			this->columns_[columnNames::name]->getData().toString(),
			this->variableName_,
			this->data(columnNames::value).toDouble() * M_PI / 180.0,
			radians.getUnitName().c_str()
			);

	return pSIItem;
}

// ----------------------------------------------------------------

SettingsItemRoot::SettingsItemRoot():
												SettingsItemBase() {

	// Fill the columns with the name (intended as the 'dysplayName',
	// the value and the unit
	columns_[columnNames::name] ->setData( "Name" );
	columns_[columnNames::value]->setData( "Value");
	columns_[columnNames::unit] ->setData( "Unit" );

	// The root is not editable
	setEditable(false);

	// The root is always expanded
	setExpanded(true);
}

// Ctor from xml
SettingsItemRoot::SettingsItemRoot(const XmlAttributeSet& xmlAttSet) :
								SettingsItemRoot(){

}


SettingsItemRoot::~SettingsItemRoot() {

}

// Accept a visitor that will write this item to XML
void SettingsItemRoot::accept( VPPSettingsXmlWriterVisitor& v ) {

	// Visit me
	v.visit(this);
}

// Accept a visitor that will write this item to XML
void SettingsItemRoot::accept( VPPSettingsXmlReaderVisitor& v ) {

	// Visit me
	v.visit(this);
}

// Accept a visitor that will write this item to XML
void SettingsItemRoot::accept( VariableParserGetVisitor& v) {

	// Visit me
	v.visit(this);
}

// Clone this item, which is basically equivalent to calling the copy ctor
SettingsItemRoot* SettingsItemRoot::clone() const {
	return new SettingsItemRoot(*this);
}

/// Get the internal name of this item, used to locate it in the tree
/// In this case the internal name is '/'
QString SettingsItemRoot::getInternalName() const {
	return QString("/");
}

/// Get the variable name of this item, used to locate it in the tree
/// In this case the internal name is '/'
QString SettingsItemRoot::getVariableName() const {
	return getInternalName();
}

/// Clone this item, which is basically equivalent to calling the copy ctor
SettingsItemRoot::SettingsItemRoot(const SettingsItemRoot& rhs) :
								SettingsItemBase(rhs){
	// Do nothing because this class does not own members
}

// Assignment operator
const SettingsItemRoot& SettingsItemRoot::operator=(const SettingsItemRoot& rhs) {

	// Call mother-class operator
	SettingsItemBase::operator=(rhs);

	return *this;
}

// Comparison operator
bool SettingsItemRoot::operator==(const SettingsItemRoot& rhs) {

	// Call the parent class comparison
	if(!SettingsItemBase::operator==(rhs))
		return false;

	// Parent class is fine, keep comparing own members
	if( className_== rhs.className_ ){
		return true;
	}
	return false;

}

// ----------------------------------------------------------------

// Ctor
SettingsItemGroup::SettingsItemGroup(const QVariant& displayName):
												SettingsItemBase(){

	// The group is not editable
	columns_[columnNames::name]->setData( displayName );

	// The root is not editable
	setEditable(false);

}

// Ctor from xml
SettingsItemGroup::SettingsItemGroup(const XmlAttributeSet& xmlAttSet) :
							SettingsItemGroup(xmlAttSet["DisplayName"].toQString()){

}

// Copy Ctor, called by clone()
SettingsItemGroup::SettingsItemGroup(const SettingsItemGroup& rhs):
								SettingsItemBase(rhs){
	// Do nothing as the class has no own members
}


// Dtor
SettingsItemGroup::~SettingsItemGroup() {

}

// Accept a visitor that will write this item to XML
void SettingsItemGroup::accept( VPPSettingsXmlWriterVisitor& v ) {

	// Visit me
	v.visit(this);

}

// Clone this item, which is basically equivalent to calling the copy ctor
SettingsItemGroup* SettingsItemGroup::clone() const {
	return new SettingsItemGroup(*this);
}

// Returns the font this item should be visualized
// with in the item tree
QFont SettingsItemGroup::getFont() const {
	QFont myFont;
	myFont.setBold(true);
	myFont.setUnderline(true);
	return myFont;
}



