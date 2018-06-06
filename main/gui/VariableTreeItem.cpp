
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
	columns_[colNames::name_.idx_] ->setData(colNames::name_.label_);
	columns_[colNames::value_.idx_]->setData(colNames::value_.label_);

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

	columns_[colNames::name_.idx_]->setData( variableName );
	columns_[colNames::value_.idx_]->setData( value );

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

	columns_[colNames::name_.idx_]->setData( grooupName );
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


