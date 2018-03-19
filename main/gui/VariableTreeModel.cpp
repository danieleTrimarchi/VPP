#include "VariableTreeModel.h"

#include <QStringList>
#include "VariableTreeItem.h"
#include <iostream>

VariableTreeModel::VariableTreeModel(QObject *parent)
: VppItemModel(parent) {

	QList<QVariant> rootData;
	rootData <<  "Name" << "Value";
	pRootItem_.reset( new VariableTreeItem(rootData) );
	setupModelData();

}

VariableTreeModel::~VariableTreeModel() {}

// Append a variable item to the tree
void VariableTreeModel::append( QList<QVariant>& columnData ) {

	// Check if we have a fakeRootItem; if not add it
	if(!pRootItem_->childCount())
		pRootItem_->appendChild(new VariableTreeFakeRoot(columnData, pRootItem_.get()));

  	// Get the index of the fake root item
  	QModelIndex fakeRootItemIndex= index(1,0);

	size_t nchildren = pRootItem_->child(0)->childCount();

	// Call this method to append ONE row under the fake root.
	// The method emits the rowsAboutToBeInserted() signal that allows
	// for a smooth connection with the view
	beginInsertRows(fakeRootItemIndex, nchildren, nchildren);

	pRootItem_->child(0)->appendChild(new VariableTreeItem(columnData, pRootItem_->child(0)));

	endInsertRows();

}

// Actually generate the item tree
void VariableTreeModel::setupModelData() {

	// Append variables to the tree
	QList<QVariant> columnData;
	columnData << "Variables"<<" ";

	// Place the fake root under the real root
	pRootItem_->appendChild(new VariableTreeFakeRoot(columnData, pRootItem_.get()));

}



