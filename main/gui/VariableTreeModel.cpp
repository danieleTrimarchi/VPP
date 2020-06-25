#include "VariableTreeModel.h"

#include <QStringList>
#include "VariableTreeItem.h"
#include <iostream>

VariableTreeModel::VariableTreeModel(QObject *parent)
: VppItemModel(parent) {

	pRootItem_.reset( new VariableTreeRoot );
	setupModelData();

}

VariableTreeModel::~VariableTreeModel() {}

// Append a variable item to the tree
void VariableTreeModel::append(QVariant variableName, QVariant value) {

	// Assume there is not fake root, instantiate one because we always
	// want to see the "Variable" item by construction
	if(!pRootItem_->childCount())
		setupModelData();

  	// Get the index of the fake root item
  	QModelIndex fakeRootItemIndex= index(1,0);

	size_t nchildren = pRootItem_->child(0)->childCount();

	// Call this method to append ONE row under the fake root.
	// The method emits the rowsAboutToBeInserted() signal that allows
	// for a smooth connection with the view
	beginInsertRows(fakeRootItemIndex, nchildren, nchildren);

	pRootItem_->child(0)->appendChild(new VariableTreeItem(variableName,value, pRootItem_.get()));

	endInsertRows();

}

// Actually generate the item tree
void VariableTreeModel::setupModelData() {

	// Place the fake root under the real root
	pRootItem_->appendChild(new VariableTreeItemGroup("Variables",pRootItem_.get()));

}



