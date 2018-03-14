
//#include <QtCore/QStringList>
#include "VariableTreeItem.h"

VariableTreeItemBase::VariableTreeItemBase(const QList<QVariant> &data, Item *parent) :
	Item(parent) {

	// The variable item is visualized by name and value
	// WARNING!!
	// todo dtrimarchi : the data are filled based on
	// an intrinsic order name-value in the QList<> data.
	// This is not acceptable and must be seriously improved!
	columns_.push_back(new NameColumn(data.value(0)));
	columns_.push_back(new ValueColumn(data.value(1)));

}

VariableTreeItemBase::~VariableTreeItemBase() {}


////////////////////////////////////////////////////////////

VariableTreeFakeRoot::VariableTreeFakeRoot(const QList<QVariant>&data, Item *parentItem /*=0*/) :
		VariableTreeItemBase(data, parentItem) {

}

QVariant VariableTreeFakeRoot::getIcon() {
	return QIcon::fromTheme("Variable", QIcon(":/icons/variable_root.png"));
}


VariableTreeFakeRoot::~VariableTreeFakeRoot(){

}

////////////////////////////////////////////////////////////

VariableTreeItem::VariableTreeItem(const QList<QVariant>&data, Item *parentItem /*=0*/) :
		VariableTreeItemBase(data, parentItem) {

}

QVariant VariableTreeItem::getIcon() {
	return QIcon::fromTheme("Variable", QIcon(":/icons/variable.png"));
}

VariableTreeItem::~VariableTreeItem(){

}

