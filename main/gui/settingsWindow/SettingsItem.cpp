#include "SettingsItem.h"
//#include <QtWidgets/QLineEdit>
//#include <QtWidgets/QSpinBox>
//#include <QtGui/QColor>
#include <iostream>
#include "VppSettingsXmlWriter.h"
#include "VppSettingsXmlReader.h"
#include "VariableFileParser.h"

using namespace std;

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

// ----------------------------------------------------------------

// Ctor
SettingsItemBounds::SettingsItemBounds(const QVariant& displayName,const QVariant& variableName,double min,double max,const QVariant& unit,const QVariant& tooltip) :
								SettingsItemGroup(displayName){

	appendChild( new SettingsItem(displayName.toString()+QString("_min"),variableName.toString()+QString("_MIN"),min,unit,"min value") );
	appendChild( new SettingsItem(displayName.toString()+QString("_max"),variableName.toString()+QString("_MAX"),max,unit,"max value") );

	// Set the tooltip
	tooltip_= tooltip;

	// Set the variable Name for this item
	variableName_= variableName.toString();
}

// Ctor from xml
SettingsItemBounds::SettingsItemBounds(const XmlAttributeSet& xmlAttSet) :
						SettingsItemGroup(xmlAttSet["DisplayName"].toQString()){

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

// Accept a visitor that will write this item to the variableFileParser
void SettingsItemBounds::accept( VariableParserGetVisitor& v) {

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

// Returns a handle on the item that represents the min in this bound
SettingsItemBase* SettingsItemBounds::getItemMin() {
	return dynamic_cast<SettingsItemBase*>(child(0));
}

// Returns a handle on the item that represents the max in this bound
SettingsItemBase* SettingsItemBounds::getItemMax() {
	return dynamic_cast<SettingsItemBase*>(child(1));
}

// Get the min value of this bound
double SettingsItemBounds::getMin() {
	return getItemMin()->data(columnNames::value).toDouble();
}

// Get the max value of this bound
double SettingsItemBounds::getMax() {
	return getItemMax()->data(columnNames::value).toDouble();
}

// ----------------------------------------------------------------

// Ctor
SettingsItem::SettingsItem(	const QVariant& displayName,
		const QVariant& variableName,
		const QVariant& value,
		const QVariant& unit,
		const QVariant& tooltip):
								SettingsItemBase(){

	columns_[columnNames::name]->setData( displayName );
	columns_[columnNames::value]->setData( value );
	columns_[columnNames::unit]->setData( unit );

	// Set the tooltip
	tooltip_= tooltip;

	// The editable columns for the settings item are editable
	setEditable(true);

	// Set the variable name that will be used to build
	// the variable in the variableFileParser
	variableName_= variableName.toString();
}

// Ctor from xml. Just call the other constructor
SettingsItem::SettingsItem(const XmlAttributeSet& xmlAttSet):
						SettingsItem( 	xmlAttSet["DisplayName"].toQString(),
								xmlAttSet["VariableName"].toQString(),
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

// Accept a visitor that will write this item to XML
void SettingsItem::accept( VariableParserGetVisitor& v) {

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
SettingsItemInt::SettingsItemInt(const QVariant& displayName,const QVariant& variableName,
		const QVariant& value,
		const QVariant& unit,
		const QVariant& tooltip):
								SettingsItem(displayName,variableName,value,unit,tooltip){

}

// Ctor from xml
SettingsItemInt::SettingsItemInt(const XmlAttributeSet& xmlAttSet) :
						SettingsItemInt(	xmlAttSet["DisplayName"].toQString(),
								xmlAttSet["VariableName"].toQString(),
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

	//std::cout<<"Setting \'"<<value<<"\' to item "<<this<<" ; model "<<model<<std::endl;
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
SettingsItemComboBox::SettingsItemComboBox(
		const QVariant& displayName,
		const QVariant& variableName,
		const QVariant& unit,
		const QList<QString>& options,
		const QVariant& tooltip):
									SettingsItem(displayName,variableName,options[0],unit,tooltip),
									opts_(options),
									activeIndex_(0) {

}

// Ctor from xml
// todo : the options are known by the visitor, not the item. All the
// logics here should be displaced to the xmlreadvisitor!
SettingsItemComboBox::SettingsItemComboBox(const XmlAttributeSet& xmlAttSet) :
						SettingsItem(	xmlAttSet["DisplayName"].toQString(),
								xmlAttSet["VariableName"].toQString(),
								xmlAttSet["Option0"].toQString(),
								xmlAttSet["Unit"].toQString(),
								xmlAttSet["ToolTip"].toQString() ){

	// Populate the options
	for(size_t i=0; i<int((xmlAttSet["numOpts"]).getInt()); i++){
		char msg[256];
		sprintf(msg,"Option%zu",i);
		opts_.push_back(xmlAttSet[string(msg)].toQString());
	}

	activeIndex_= xmlAttSet["ActiveIndex"].getInt();

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

// Accept a visitor that will write this item to XML
void SettingsItemComboBox::accept( VariableParserGetVisitor& v) {

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

	// Set the data with the index of the selected option
	QComboBox* pComboBox = static_cast<QComboBox*>(editor);
	pComboBox->setCurrentIndex(activeIndex_);
}

// Set the data in the model - called by the Delegate
void SettingsItemComboBox::setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const {

	// Get a handle on the combo box
	QComboBox* pComboBox = static_cast<QComboBox*>(editor);

	// Get the selected text
	QString value = pComboBox->currentText();

	// Set the active index. Note that activeIndex_ is declared
	// mutable, as we need to set this inside a const method
	activeIndex_ = pComboBox->currentIndex();

	// Set the underlying model
	model->setData(index, value, Qt::EditRole);

}

//  Returns the label of the active (selected) item
// Called by the parent 'paint' method
QString SettingsItemComboBox::getActiveText(const QModelIndex &index) const {
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

