
//#include <QtCore/QStringList>
#include "VariableTreeItem.h"

VariableTreeItemBase::VariableTreeItemBase(Item* parentItem /*=0*/) :
	Item(parentItem) {

	// The variable item is visualized by name and value
	columns_.push_back(new NameColumn);
	columns_.push_back(new ValueColumn);

}

VariableTreeItemBase::~VariableTreeItemBase() {}


////////////////////////////////////////////////////////////

// Ctor
VariableTreeRoot::VariableTreeRoot(Item* parentItem /*=0*/) :
		VariableTreeItemBase(parentItem) {

	// Fill the columns with the name (intended as the 'dysplayName',
	// the value and the unit
	columns_[columnNames::name] ->setData( "Name" );
	columns_[columnNames::value]->setData( "Value");

}

// Dtor
VariableTreeRoot::~VariableTreeRoot(){

}

QVariant VariableTreeRoot::getIcon(size_t row/*=0*/) {
	return QVariant();
}

////////////////////////////////////////////////////////////

/// Ctor
VariableTreeItem::VariableTreeItem(const QVariant& variableName, const QVariant& value, Item *parentItem /*=0*/):
		VariableTreeItemBase(parentItem) {

	columns_[columnNames::name]->setData( variableName );
	columns_[columnNames::value]->setData( value );

}


QVariant VariableTreeItem::getIcon(size_t row/*=0*/) {

	// Only return the icon for col 0
	if(row)
		return QVariant();
	else
		return QIcon::fromTheme("Variable", QIcon(":/icons/variable.png"));
}

VariableTreeItem::~VariableTreeItem(){

}

///////////////////////////////////////////////////////////////////////

// Ctor
VariableTreeItemGroup::VariableTreeItemGroup(const QVariant& grooupName,Item *parentItem) :
		VariableTreeItemBase(parentItem) {

	columns_[columnNames::name]->setData( grooupName );
	// Leave the data column empty
}

// Dtor
VariableTreeItemGroup::~VariableTreeItemGroup() {

}

// Returns the icon associated with this variable tree item
QVariant VariableTreeItemGroup::getIcon(size_t row/*=0*/) {
	// Only return the icon for col 0
	if(row)
		return QVariant();
	else
		return QIcon::fromTheme("Variable", QIcon(":/icons/variable_root.png"));;
}


