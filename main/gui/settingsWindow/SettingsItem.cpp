#include "SettingsItem.h"
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtGui/QColor>
#include <iostream>

#include "GetSettingsItemVisitor.h"
#include "VppSettingsXmlWriter.h"
#include "VppSettingsXmlReader.h"

//// Forward declarations
//class SettingsItem;
//class SettingsItemComboBox;
//class SettingsItemInt;
//class SettingsItemgroup;
//class SettingsItemBounds;
//class SettingsItemRoot;

using namespace std;

// Ctor
SettingsItemBase::SettingsItemBase() :
					pParent_(0),
					editable_(Qt::ItemIsEditable),
					tooltip_(QVariant()),
					expanded_(false){

	columns_.push_back(new NameColumn);
	columns_.push_back(new ValueColumn);
	columns_.push_back(new UnitColumn);
}

// Copy Ctor
SettingsItemBase::SettingsItemBase(const SettingsItemBase& rhs) {
	// Do not copy the parent as the clone is not
	// supposed to be under the original parent!
	//pParent_=rhs.pParent_;
	pParent_ = 0;
	editable_= rhs.editable_;
	tooltip_= rhs.tooltip_;
	expanded_ = rhs.expanded_;

	// Deep copy the columns
	columns_.clear();
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

// Set the parent of this item
void SettingsItemBase::setParent(SettingsItemBase* parentItem) {
	pParent_= parentItem;
}

// Set the parent of this item
void SettingsItemBase::setParentRecursive(SettingsItemBase* parentItem) {

	// Give me a parent
	pParent_= parentItem;

	// Tell my children that I am their father
	for(size_t iChild=0; iChild<children_.size(); iChild++)
		children_[iChild]->setParentRecursive(this);

}

// This slot is triggered when the item is expanded in the view
void SettingsItemBase::setExpanded(bool expanded) {
	expanded_ = expanded;
}

// Return the flag 'expanded'
bool SettingsItemBase::expanded() const {
	return expanded_;
}

// Static Factory method - builds a SettingsItem from the attributes
// read from xml and stored into an appropriate set
SettingsItemBase* SettingsItemBase::settingsItemFactory(const XmlAttributeSet& attSet){

	// What class am I going to instantiate?
	string className = attSet["ClassName"];

	SettingsItemBase* pItem;
	// Now instantiate the item based on its type
	if(className == string("SettingsItemRoot") ){
			pItem = new SettingsItemRoot;
	} else if(className == string("SettingsItem")){
			pItem = new SettingsItem(attSet);
	} else if(className == string("SettingsItemComboBox")){
			pItem = new SettingsItemComboBox(attSet);
	} else if(className == string("SettingsItemInt")){
		pItem = new SettingsItemInt(attSet);
	} else if(className == string("SettingsItemGroup")){
			pItem = new SettingsItemGroup(attSet);
	} else if(className == string("SettingsItemBounds")){
			pItem = new SettingsItemBounds(attSet);
	} 	else {
		char msg[256];
		sprintf(msg,"The class name: \"%s\" is not supported",className.c_str());
		throw std::logic_error(msg);
	}

	return pItem;
}

// Dtor
SettingsItemBase::~SettingsItemBase() {
	qDeleteAll(children_);
}

// Append a child under me
void SettingsItemBase::appendChild(SettingsItemBase* child) {

	// Make sure the item knows who his parent is
	child->setParent(this);
	// Append the item to the child list
	children_.append(child);

}

// Remove all children under me
void SettingsItemBase::clearChildren() {
	children_.clear();
}

// Remove a child by position
void SettingsItemBase::removeChild(size_t iChild) {
	children_.removeAt(iChild);
}

// Get the i-th child
SettingsItemBase* SettingsItemBase::child(int row) {
	return children_.value(row);
}

// Get the i-th child - const variety
SettingsItemBase* SettingsItemBase::child(int row) const{
	return children_.value(row);
}

// Get a child by name
SettingsItemBase* SettingsItemBase::child(QString& childName) {

	// Instantiate a visitor that will be searching for the item by name
	GetSettingsItemByNameVisitor vn(this);
	return vn.get(childName);
}

// Get a child by path - inclusive of the child name, of course
SettingsItemBase* SettingsItemBase::childPath(QString& childPath) {

	// Instantiate a visitor that will be searching for the item by name
	GetSettingsItemByPathVisitor vp(this);
	return vp.get(childPath);
}

// How many children do I have?
int SettingsItemBase::childCount() const {
	return children_.count();
}

// What child number am I?
int SettingsItemBase::childNumber() const {
	if (pParent_)
		return pParent_->children_.indexOf(const_cast<SettingsItemBase*>(this));

	return 0;
}

// Clone this item, which is basically equivalent to calling the copy ctor
SettingsItemBase* SettingsItemBase::clone() const {

	return new SettingsItemBase(*this);
}

// Number of rows required to the cols of this item.
// i.e: the number of children. For the moment we
// just return 1.
int SettingsItemBase::columnCount() const {
	return columns_.size();
}

// Return the parent item
SettingsItemBase* SettingsItemBase::parentItem() {
	return pParent_;
}

int SettingsItemBase::row() const {

	if (pParent_)
		return pParent_->children_.indexOf(const_cast<SettingsItemBase*>(this));

	return 0;
}

// Returns the associated icon - in this case an empty QVariant
QVariant SettingsItemBase::getIcon() {
	return QVariant();
}

// Get the internal name of this item, used to locate it in the tree
// This is in the format parentName.myName, where the "spaces" have
// been substituted by "_"
QString SettingsItemBase::getInternalName() const {

	QString path;

	// Get the parent internal name
	if(pParent_)
		path = pParent_->getInternalName() +  QString(".");

	// And now add my own name. Replace spaces with '_'
	path += getName().replace(" ","_",Qt::CaseSensitive);

	return path;
}

bool SettingsItemBase::setData(int column, const QVariant& value) {

	if (column < 0 || column >= columns_.size())
		return false;

	columns_[column]->setData( value );
	return true;
}

// Return the data stored in the i-th column of
// this item. In this case, either the label
// or the numerical value
QVariant SettingsItemBase::data(int iColumn) const {
	return columns_[iColumn]->getData(iColumn);
}

// Set if this item is editable
void SettingsItemBase::setEditable(bool editable) {
	if(editable)
		editable_ = Qt::ItemIsEditable;
	else
		editable_ = Qt::NoItemFlags;

}

// Is this item editable?
Qt::ItemFlag SettingsItemBase::editable() const {
	return editable_;
}

// Get a reference to the columns vector
std::vector<SettingsColumn*>& SettingsItemBase::getColumnVector() {
	return columns_;
}

// Get a reference to the i-th column in the vector
SettingsColumn* SettingsItemBase::getColumn(const int iColumn) {
	return getColumnVector()[iColumn];
}

// Returns the font this item should be visualized
// with in the item tree
QFont SettingsItemBase::getFont() const {
	return QFont();
}

// Return the backGround color for this item based on the column
QColor SettingsItemBase::getBackGroundColor(int iColumn) const {

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
	painter->drawText(textRect, index.data(Qt::TextAlignmentRole).toInt(), index.data().toString());

}

// Accept a visitor, the role of which is to iterate and
// return an item by path
SettingsItemBase* SettingsItemBase::accept(const GetSettingsItemByPathVisitor& v, QString varName) {

	if( getInternalName() == varName )
		return this;

	for(size_t iChild=0; iChild<childCount(); iChild++) {
		SettingsItemBase* pChild = child(iChild)->accept(v,varName);
		if(pChild)
			return pChild;
	}

	return 0;
}

// Accept a visitor, the role of which is to iterate and
// return an item by name
SettingsItemBase* SettingsItemBase::accept(const GetSettingsItemByNameVisitor& v, QString varName) {

	//std::cout << internalName_.toStdString() <<std::endl;
	if(columns_[columnNames::name]->getData().toString() == varName )
		return this;

	for(size_t iChild=0; iChild<childCount(); iChild++) {
		SettingsItemBase* pChild = child(iChild)->accept(v,varName);
		if(pChild)
			return pChild;
	}

	return 0;
}

// Accept a visitor that will write this item to XML
void SettingsItemBase::accept( VPPSettingsXmlWriterVisitor& v ) {

	// Visit me
	v.visit(this);

}

// Accept a visitor that will instantiate the items described in an
// xml file and append them under me
void SettingsItemBase::accept( VPPSettingsXmlReaderVisitor& v) {

	// Visit me
	v.visit(this);

}

// Get the display name of this item
QString SettingsItemBase::getName() const {
	return columns_[columnNames::name]->getData().toString();
}

// Only meaningful for the combo-box item, returns zero
// for all the other items
size_t SettingsItemBase::getActiveIndex() const {
	return 0;
}


// Only meaningful for the combo-box item,
// returns an empty QString for the base-class
QString SettingsItemBase::getActiveLabel() const {
	return QString();
}

// Assignment operator
const SettingsItemBase& SettingsItemBase::operator=(const SettingsItemBase& rhs) {

	pParent_= rhs.pParent_;
	editable_=rhs.editable_;
	tooltip_= rhs.tooltip_;
	columns_= rhs.columns_;

	return *this;
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

// Clone this item, which is basically equivalent to calling the copy ctor
SettingsItemRoot* SettingsItemRoot::clone() const {
	return new SettingsItemRoot(*this);
}

/// Get the internal name of this item, used to locate it in the tree
/// In this case the internal name is '/'
QString SettingsItemRoot::getInternalName() const {
	return QString("/");
}

/// Clone this item, which is basically equivalent to calling the copy ctor
SettingsItemRoot::SettingsItemRoot(const SettingsItemRoot& rhs) :
		SettingsItemBase(rhs){
	// Do nothing because this class does not own members
}

// ----------------------------------------------------------------

// Ctor
SettingsItemGroup::SettingsItemGroup(const QVariant& name):
						SettingsItemBase(){

	// The group is not editable
	columns_[columnNames::name]->setData( name );

	// The root is not editable
	setEditable(false);

}

// Ctor from xml
SettingsItemGroup::SettingsItemGroup(const XmlAttributeSet& xmlAttSet) :
	SettingsItemGroup(xmlAttSet["Name"].toQString()){

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

// ----------------------------------------------------------------

// Ctor
SettingsItemBounds::SettingsItemBounds(const QVariant& name,double min,double max,const QVariant& unit,const QVariant& tooltip) :
				SettingsItemGroup(name){

	appendChild( new SettingsItem("min",min,unit,"min value") );
	appendChild( new SettingsItem("max",max,unit,"max value") );

	// Set the tooltip
	tooltip_= tooltip;

}

// Ctor from xml
SettingsItemBounds::SettingsItemBounds(const XmlAttributeSet& xmlAttSet) :
		SettingsItemGroup(xmlAttSet["Name"].toQString()){

	// Do not instantiate children, if requested they are instantiated on
	// the fly while reading the rest of the xml
}

// Copy Ctor, called by clone()
SettingsItemBounds::SettingsItemBounds(const SettingsItemBounds& rhs):
		SettingsItemGroup(rhs){

	// Do nothing as SettingsItemBounds do not have own members
}

// Dtor
SettingsItemBounds::~SettingsItemBounds() {

}

// Accept a visitor that will write this item to XML
void SettingsItemBounds::accept( VPPSettingsXmlWriterVisitor& v ) {

	// Visit me
	v.visit(this);
}

// Clone this item, which is basically equivalent to calling the copy ctor
SettingsItemBounds* SettingsItemBounds::clone() const {
	return new SettingsItemBounds(*this);
}

QFont SettingsItemBounds::getFont() const {
	QFont myFont;
	myFont.setBold(true);
	myFont.setUnderline(false);
	int fontSize = myFont.pointSize();
	myFont.setPointSize(fontSize-2);
	return myFont;
}

// Get the min value of this bound
double SettingsItemBounds::getMin(){

	// Just return the first item, that I know is 'min'
	// return this->child(0)->data(columnNames::value).toDouble();
	// Better strategy : instantiate a visitor and search the item by name
	GetSettingsItemByNameVisitor v(this);
	return v.get("min")->data(columnNames::value).toDouble();
}

// Get the max value of this bound
double SettingsItemBounds::getMax() {
	// Just return the first item, that I know is 'max'
	// return this->child(1)->data(columnNames::value).toDouble();
	// Better strategy : instantiate a visitor and search the item by name
	GetSettingsItemByNameVisitor v(this);
	return v.get("max")->data(columnNames::value).toDouble();
}

// ----------------------------------------------------------------

// Ctor
SettingsItem::SettingsItem(	const QVariant& name,
		const QVariant& value,
		const QVariant& unit,
		const QVariant& tooltip):
						SettingsItemBase(){

	columns_[0]->setData( name );
	columns_[1]->setData( value );
	columns_[2]->setData( unit );

	// Set the tooltip
	tooltip_= tooltip;

	// The editable columns for the settings item are editable
	setEditable(true);

}

// Ctor from xml. Just call the other constructor
SettingsItem::SettingsItem(const XmlAttributeSet& xmlAttSet):
		SettingsItem( 	xmlAttSet["Name"].toQString(),
									xmlAttSet["Value"].toQString(),
									xmlAttSet["Unit"].toQString(),
									xmlAttSet["ToolTip"].toQString() ){
}

// Copy Ctor
SettingsItem::SettingsItem(const SettingsItem& rhs) :
	SettingsItemBase(rhs) {

}

// Dtor
SettingsItem::~SettingsItem(){

}

// Accept a visitor that will write this item to XML
void SettingsItem::accept( VPPSettingsXmlWriterVisitor& v ) {

	// Visit me
	v.visit(this);
}

// Clone this item, which is basically equivalent to calling the copy ctor
SettingsItem* SettingsItem::clone() const {
	return new SettingsItem(*this);
}

// Return the backGround color for this item based on the column
QColor SettingsItem::getBackGroundColor(int iColumn) const {

	return columns_[iColumn]->getBackGroundColor();

}

// The item will give the Delegate the editor
// to be properly edited - in this case a QLineEdit with
// double validator
QWidget* SettingsItem::createEditor(QWidget *parent) {

	// Set local settings for the validators
	QLocale localSettings = QLocale::c();
	localSettings.setNumberOptions(QLocale::RejectGroupSeparator | QLocale::OmitGroupSeparator);

	QLineEdit *editor = new QLineEdit(parent);
	QDoubleValidator* doubleValidator= new QDoubleValidator(0.000, 999999.000, 3, editor);
	doubleValidator->setLocale(localSettings);

	editor->setStyleSheet(	"background-color: white;"
			"selection-color: orange;" );

	editor->setValidator(doubleValidator);
	editor->setFrame(false);
	editor->setText( QString::number(0.000) );

	return editor;

}

// Edit the data in the editor
void SettingsItem::setEditorData(QWidget *editor,const QModelIndex& index) {

	double value = index.model()->data(index, Qt::EditRole).toDouble();

	// Set local settings for the validators
	QLocale localSettings = QLocale::c();
	localSettings.setNumberOptions(QLocale::RejectGroupSeparator | QLocale::OmitGroupSeparator);

	QDoubleValidator* doubleValidator= new QDoubleValidator(0.000, 999999.000, 3, editor);
	doubleValidator->setLocale(localSettings);

	QLineEdit* pLineEdit = static_cast<QLineEdit*>(editor);

	pLineEdit->setValidator(doubleValidator);
	pLineEdit->setFrame(false);
	pLineEdit->setText( QString::number(value) );

}

// Set the data in the model
void SettingsItem::setModelData(	QWidget* editor,
		QAbstractItemModel* model,
		const QModelIndex& index) const {

	QLineEdit* pQLineEdit = static_cast<QLineEdit*>(editor);

	model->setData(index, pQLineEdit->text(), Qt::EditRole);

}

// Visual options - requested by the Delegate - this directly derives from MEMS+
// Decorates the base class method paint
void SettingsItem::paint(QPainter* painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const {

	painter->save();

	// If the element is editable we draw a background and a box around the text
	if (index.model()->flags(index).testFlag(Qt::ItemIsEditable)) {

		// Gets a rectangle to draw the background and adjust it
		QRect boxRect(option.rect);
		boxRect.adjust(1, 1, -1, -1);

		// Sets the color of the border depending on the selected or hovered state
		if ( (option.state & QStyle::State_Selected) || (option.state & QStyle::State_MouseOver) )
			painter->setPen(QApplication::palette().highlight().color());
		else
			painter->setPen(QPen(Qt::lightGray));

		// Sets the brush depending on the selected state
		if (option.state & QStyle::State_Selected)
			painter->setBrush(QBrush(QApplication::palette().highlight().color(), Qt::Dense6Pattern));
		else
			painter->setBrush(QColor("white"));

		// Draws the rectangle around the text
		painter->drawRect(boxRect);
	}

	// Call base-class method
	SettingsItemBase::paint(painter,option,index);

	painter->restore();
}

// ---------------------------------------------------------------

/// Ctor
SettingsItemInt::SettingsItemInt(const QVariant& name,
		const QVariant& value,
		const QVariant& unit,
		const QVariant& tooltip):
				SettingsItem(name,value,unit,tooltip){

}

// Ctor from xml
SettingsItemInt::SettingsItemInt(const XmlAttributeSet& xmlAttSet) :
		SettingsItemInt(	xmlAttSet["Name"].toQString(),
										xmlAttSet["Value"].toQString(),
										xmlAttSet["Unit"].toQString(),
										xmlAttSet["ToolTip"].toQString() ){
}

/// Dtor
SettingsItemInt::~SettingsItemInt() {
	/* do nothing */
}

// Accept a visitor that will write this item to XML
void SettingsItemInt::accept( VPPSettingsXmlWriterVisitor& v ) {

	// Visit me
	v.visit(this);
}

// Clone this item, which is basically equivalent to calling the copy ctor
SettingsItemInt* SettingsItemInt::clone() const {
	return new SettingsItemInt(*this);
}

/// The item will give the Delegate the editor
/// to be properly edited - in this case a spinbox
QWidget* SettingsItemInt::createEditor(QWidget *parent) {

	QSpinBox* editor = new QSpinBox(parent);
	editor->setFrame(false);
	editor->setMinimum(0);
	editor->setMaximum(1000);

	editor->setStyleSheet(	"background-color: white;"
			"selection-color: orange;");

	return editor;

}


// Edit the data in the editor
void SettingsItemInt::setEditorData(QWidget *editor,const QModelIndex& index) {

	// Get the value stored in the model for this object
	int value = index.model()->data(index, Qt::EditRole).toInt();

	QSpinBox* pSpinBox = static_cast<QSpinBox*>(editor);
	pSpinBox->setValue(value);

}

void SettingsItemInt::setModelData(	QWidget *editor,
									QAbstractItemModel *model,
									const QModelIndex &index) const {

	QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
	spinBox->interpretText();
	int value = spinBox->value();

	std::cout<<"Setting \'"<<value<<"\' to item "<<this<<" ; model "<<model<<std::endl;
	model->setData(index, value, Qt::EditRole);
}

// Copy ctor, called by clone()
SettingsItemInt::SettingsItemInt(const SettingsItemInt& rhs) :
		SettingsItem(rhs) {

	// There is nothing else to do, as there are no owned class
	// members
}

// ---------------------------------------------------------------

// Ctor
SettingsItemComboBox::SettingsItemComboBox(const QVariant& name,
		const QVariant& unit,
		const QList<QString>& options,
		const QVariant& tooltip):
				SettingsItem(name,options[0],unit,tooltip),
				opts_(options),
				activeIndex_(0) {

}

// Ctor from xml
SettingsItemComboBox::SettingsItemComboBox(const XmlAttributeSet& xmlAttSet) :
		SettingsItem(	xmlAttSet["Name"].toQString(),
									xmlAttSet["Option0"].toQString(),
									xmlAttSet["Unit"].toQString(),
									xmlAttSet["ToolTip"].toQString() ){

	// Populate the options
	for(size_t i=0; i<xmlAttSet["numOpts"]; i++){
		char msg[256];
		sprintf(msg,"Option%zu",i);
		opts_.push_back(xmlAttSet[string(msg)].toQString());
	}

	activeIndex_= xmlAttSet["ActiveIndex"];

}

// Dtor
SettingsItemComboBox::~SettingsItemComboBox() {
	/* do nothing */
}

// Accept a visitor that will write this item to XML
void SettingsItemComboBox::accept( VPPSettingsXmlWriterVisitor& v ) {

	// Visit me
	v.visit(this);

}

/// Clone this item, which is basically equivalent to calling the copy ctor
SettingsItemComboBox* SettingsItemComboBox::clone() const {
	return new SettingsItemComboBox(*this);
}

// The item will give the Delegate the editor
// to be properly edited - in this case a spinbox
QWidget* SettingsItemComboBox::createEditor(QWidget *parent) {

	QComboBox* editor = new QComboBox(parent);

	for(size_t i=0; i<opts_.size(); i++)
		editor->addItem(opts_[i]);

	editor->setCurrentIndex(activeIndex_);

	editor->setStyleSheet(	"background-color: white;"
			"selection-color: orange;");

	return editor;
}

// Edit the data in the editor
void SettingsItemComboBox::setEditorData(QWidget *editor,const QModelIndex& index) {

	// I have stored in the model the index of the selected option
	activeIndex_ = index.model()->data(index, Qt::EditRole).toInt();

	QComboBox* pComboBox = static_cast<QComboBox*>(editor);
	pComboBox->setCurrentIndex(activeIndex_);

}

// Set the data in the model - called by the Delegate
void SettingsItemComboBox::setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const {

	QComboBox* pComboBox = static_cast<QComboBox*>(editor);

	QString value = pComboBox->currentText();

	model->setData(index, value, Qt::EditRole);

}

// Returns the label of the active (selected) item
QString SettingsItemComboBox::getActiveLabel() const {
	return opts_[activeIndex_];
}

// How many options are available to this combo-box?
size_t SettingsItemComboBox::getNumOpts() const {
	return opts_.size();
}

// Get the i-th option for this Combo-Box
QString SettingsItemComboBox::getOption(size_t i) const {
	return opts_[i];
}

// Only meaningful for the combo-box item, returns zero for all the
// other items
size_t SettingsItemComboBox::getActiveIndex() const {
	return activeIndex_;
}

// Assignment operator
const SettingsItemComboBox& SettingsItemComboBox::operator=(const SettingsItemComboBox& rhs) {

	// Call the parent operator=
	SettingsItem::operator=(rhs);

	// Now copy own members
	opts_= rhs.opts_;
	activeIndex_= rhs.activeIndex_;

	return *this;
}

// Copy Ctor
SettingsItemComboBox::SettingsItemComboBox(const SettingsItemComboBox& rhs) :
		SettingsItem(rhs),
		opts_(rhs.opts_),
		activeIndex_(rhs.activeIndex_){

}

