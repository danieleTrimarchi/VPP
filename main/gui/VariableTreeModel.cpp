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

// Remove the children of this model
void VariableTreeModel::clearChildren() {

	beginResetModel();

	// Actually delete the children
	pRootItem_->clearChildren();

	endResetModel();
}

// How many cols?
int VariableTreeModel::columnCount(const QModelIndex &parent) const {

	if (parent.isValid()){
		return static_cast<VariableTreeItemBase*>(parent.internalPointer())->columnCount();
	} else{
		return pRootItem_->columnCount();
	}
}

// Called by Qt, this method returns the data to visualize or some
// display options
QVariant VariableTreeModel::data(const QModelIndex &index, int role) const {

	if (!index.isValid()){
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		VariableTreeItemBase *item = static_cast<VariableTreeItemBase*>(index.internalPointer());
		return item->data(index.column());
	}
	else if (role == Qt::DecorationRole ) {
		VariableTreeItemBase *item = static_cast<VariableTreeItemBase*>(index.internalPointer());
		if(index.column()==0)
			return item->getIcon();
		else
			return QVariant();
	}

	return QVariant();

}

// Called by Qt, returns specific flags
Qt::ItemFlags VariableTreeModel::flags(const QModelIndex &index) const {

	if (!index.isValid())
		return 0;

	if(index.column()==0)
		return VppItemModel::flags(index);

	return Qt::ItemIsEditable | VppItemModel::flags(index);
}

// Called by Qt, returns the header
QVariant VariableTreeModel::headerData(int section, Qt::Orientation orientation,
		int role) const {

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return pRootItem_->data(section);

	return QVariant();
}

// Returns an index given row and col
QModelIndex VariableTreeModel::index(int row, int column, const QModelIndex &parent) const {

	if (!hasIndex(row, column, parent))
		return QModelIndex();

	Item *parentItem;

	if (!parent.isValid())
		parentItem = pRootItem_.get();
	else
		parentItem = static_cast<VariableTreeItemBase*>(parent.internalPointer());

	Item* childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

// Get my parent
QModelIndex VariableTreeModel::parent(const QModelIndex &index) const {

	if (!index.isValid())
		return QModelIndex();

	VariableTreeItemBase *childItem = static_cast<VariableTreeItemBase*>(index.internalPointer());
	Item* parentItem = childItem->parentItem();

	if (parentItem == pRootItem_.get())
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

// How many rows?
int VariableTreeModel::rowCount(const QModelIndex &parent) const {

	Item* parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = pRootItem_.get();
	else
		parentItem = static_cast<VariableTreeItemBase*>(parent.internalPointer());

	return parentItem->childCount();
}

// Actually generate the item tree
void VariableTreeModel::setupModelData() {

	// Append variables to the tree
	QList<QVariant> columnData;
	columnData << "Variables"<<" ";

	// Place the fake root under the real root
	pRootItem_->appendChild(new VariableTreeFakeRoot(columnData, pRootItem_.get()));

}



