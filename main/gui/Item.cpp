#include "Item.h"

#include "GetItemVisitor.h"
#include "VPPException.h"

// Default Ctor
Item::Item(Item* pParent) :
pParent_(pParent),
editable_(Qt::NoItemFlags),
expanded_(false){

}

// Copy Ctor
Item::Item(const Item& rhs) :
		// Do not copy the parent as the clone is not
		// supposed to be under the original parent!
		pParent_(0),
		editable_(rhs.editable_),
		expanded_(rhs.expanded_){

}

// Dtor
Item::~Item(){

	// Delete my children
	qDeleteAll(children_);

	// Delete the data columns
	for(size_t i=0;i<columns_.size(); i++){
		delete columns_[i];
	}
}

// Accept a visitor, the role of which is to iterate and
// return an item by path
Item* Item::accept(const GetItemByPathVisitor& v, QString varName) {

	if( getInternalName() == varName )
		return this;

	for(size_t iChild=0; iChild<childCount(); iChild++) {
		Item* pChild = child(iChild)->accept(v,varName);
		if(pChild)
			return pChild;
	}

	return 0;
}

// Accept a visitor, the role of which is to iterate and
// return an item by name
Item* Item::accept(const GetItemByNameVisitor& v, QString varName) {

	//std::cout << internalName_.toStdString() <<std::endl;
	if(columns_[columnNames::name]->getData().toString() == varName )
		return this;

	for(size_t iChild=0; iChild<childCount(); iChild++) {
		Item* pChild = child(iChild)->accept(v,varName);
		if(pChild)
			return pChild;
	}

	char msg[512];
	sprintf(msg,"The item named %s was not found!", varName.toStdString().c_str());
	throw VPPException(HERE,msg);
	return 0;
}

// Append a child under me
void Item::appendChild(Item *child) {

	// Make sure the item knows who his parent is
	if(child) {
		child->setParent(this);
		// Append the item to the child list
		children_.append(child);
	}
	else
		throw VPPException(HERE,"The child cannot be appended because it does not exist!");

}

// Get a ptr to my i-th child
Item *Item::child(int row) {
	return children_.value(row);
}

// Get a child by name
Item* Item::child(QString& childName) {

	// Instantiate a visitor that will be searching for the item by name
	GetItemByNameVisitor vn(this);
	return vn.get(childName);
}

// How many children do I have
int Item::childCount() const {
	return children_.count();
}

// What child number am I?
int Item::childNumber() const {
	if (pParent_)
		return pParent_->getChildren().indexOf(const_cast<Item*>(this));

	return 0;
}

// Remove all children under me
void Item::clearChildren() {

	// Todo : this segfaults, but why..??
	// Delete the children
	//for(size_t iChild=0; iChild<childCount(); iChild++)
	//	if(child(iChild))
	//		delete child(iChild);
	// This also segfaults...
	qDeleteAll(children_);

	// And reset the child list
	children_.clear();
}

// Clone this item, which is basically equivalent to calling the copy ctor
Item* Item::clone() const {

	return new Item(*this);
}

// Number of rows required to the cols of this item.
// i.e: the number of children. For the moment we
// just return 1.
int Item::columnCount() const {
	return columns_.size();
}

// Return the data stored in the i-th column of
// this item. In this case, either the label
// or the numerical value
QVariant Item::data(int iColumn) const {
	if(columns_.size() < iColumn )
		throw VPPException(HERE,"Out of bound request");
	return columns_[iColumn]->getData(iColumn);
}

// Is this item editable?
Qt::ItemFlag Item::editable() const {
	return editable_;
}

// Return the flag 'expanded'
bool Item::expanded() const {
	return expanded_;
}

// Return the backGround color for this item based on the column
QColor Item::getBackGroundColor(int iColumn) const {

	return QColor(Qt::GlobalColor::white);

}

// Get a child by path - inclusive of the child name, of course
Item* Item::getChildbyPath(QString& childPath) {

	// Instantiate a visitor that will be searching for the item by name
	GetItemByPathVisitor vp(this);
	return vp.get(childPath);
}

// Returns a reference to the list of children
// for this item
const QList<Item*>& Item::getChildren() const {
	return children_;
}

// Get a reference to the columns vector
std::vector<DataColumn*>& Item::getColumnVector() {
	return columns_;
}

// Get a reference to the i-th column in the vector
DataColumn* Item::getColumn(const int iColumn) {
	return getColumnVector()[iColumn];
}

// Get the display name of this item
QString Item::getDisplayName() const {
	return columns_[columnNames::name]->getData().toString();
}

// Returns the font this item should be visualized
// with in the item tree
QFont Item::getFont() const {
	return QFont();
}

// Returns the associated icon - in this case an empty QVariant
QVariant Item::getIcon(size_t row/*=0*/) {
	return QVariant();
}

// Get the internal name of this item, used to locate it in the tree
// This is in the format parentName.myName, where the "spaces" have
// been substituted by "_"
QString Item::getInternalName() const {

	QString path;

	// Get the parent internal name
	if(pParent_)
		path = pParent_->getInternalName() +  QString(".");

	// And now add my own name. Replace spaces with '_'
	path += getDisplayName().replace(" ","_",Qt::CaseSensitive);

	return path;
}

// Returns the tooltip for this item, if any
QVariant Item::getToolTip() {
	return QVariant();
}

// Assignment operator
const Item& Item::operator=(const Item& rhs) {

	pParent_= rhs.pParent_;
	columns_= rhs.columns_;
	editable_=rhs.editable_;
	expanded_=rhs.expanded_;

	return *this;
}

// Comparison operator
bool Item::operator==(const Item& rhs) {

	// Compare the name
	if( editable_==rhs.editable_&&
			expanded_==rhs.expanded_ &&
			columns_.size() == rhs.columns_.size()){

		// Loop on the columns, are the data equal?
		for(size_t iCol=0; iCol<columns_.size(); iCol++)
			if(*columns_[iCol] != *rhs.columns_[iCol])
				return false;
        return true;
	}

	return false;

}

// Inverse comparison operator
bool Item::operator!=(const Item& rhs) {
	return !(*this==rhs);
}

// Get a handle to my parent
Item* Item::parentItem() {
    return pParent_;
}

// Remove a child by position
void Item::removeChild(size_t iChild) {
	if(children_.size()>iChild)
		children_.removeAt(iChild);
	else
		throw VPPException(HERE, "Out of bounds request!");
}

// What child number am I for my parent?
int Item::row() const {
	if (pParent_)
		return pParent_->children_.indexOf(const_cast<Item*>(this));

	return 0;
}

// Set the data for the i-th column of this item
bool Item::setData(int column, const QVariant& value) {

	if (column < 0 || columns_.size()<column)
		throw VPPException(HERE,"Out of bounds request!");

	columns_[column]->setData( value );
	return true;
}

// Set if this item is editable
void Item::setEditable(bool editable) {
	if(editable)
		editable_ = Qt::ItemIsEditable;
	else
		editable_ = Qt::NoItemFlags;
}

// This slot is triggered when the item is expanded in the view
void Item::setExpanded(bool expanded) {
	expanded_ = expanded;
}

// Set the parent of this item
void Item::setParent(Item* parentItem) {
	pParent_= parentItem;
}

// Set the parent of this item
void Item::setParentRecursive(Item* parentItem) {

	// Give me a parent
	pParent_= parentItem;

	// Tell my children that I am their father
	for(size_t iChild=0; iChild<children_.size(); iChild++)
		children_[iChild]->setParentRecursive(this);

}

