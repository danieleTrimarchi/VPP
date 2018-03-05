#include "VariableTreeModel.h"

#include <QStringList>
#include "VariableTreeItem.h"

VariableTreeModel::VariableTreeModel(QObject *parent)
: QAbstractItemModel(parent) {

	QList<QVariant> rootData;
	rootData <<  "Name" << "Value";
	rootItem_ = new VariableTreeItem(rootData);
	setup();

}

VariableTreeModel::~VariableTreeModel()
{
	// Delete the root of the children. The destructor
    // of the item cleans up the children
	delete rootItem_;
}

int VariableTreeModel::columnCount(const QModelIndex &parent) const {

	if (parent.isValid())
		return static_cast<VariableTreeItemBase*>(parent.internalPointer())->columnCount();
	else
		return rootItem_->columnCount();
}

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

Qt::ItemFlags VariableTreeModel::flags(const QModelIndex &index) const {

	if (!index.isValid())
		return 0;

	if(index.column()==0)
		return QAbstractItemModel::flags(index);

	return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QVariant VariableTreeModel::headerData(int section, Qt::Orientation orientation,
		int role) const {

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem_->data(section);

	return QVariant();
}

QModelIndex VariableTreeModel::index(int row, int column, const QModelIndex &parent) const {

	if (!hasIndex(row, column, parent))
		return QModelIndex();

	VariableTreeItemBase *parentItem;

	if (!parent.isValid())
		parentItem = rootItem_;
	else
		parentItem = static_cast<VariableTreeItemBase*>(parent.internalPointer());

	VariableTreeItemBase *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex VariableTreeModel::parent(const QModelIndex &index) const {

	if (!index.isValid())
		return QModelIndex();

	VariableTreeItemBase *childItem = static_cast<VariableTreeItemBase*>(index.internalPointer());
	VariableTreeItemBase *parentItem = childItem->parentItem();

	if (parentItem == rootItem_)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int VariableTreeModel::rowCount(const QModelIndex &parent) const {

	VariableTreeItemBase *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = rootItem_;
	else
		parentItem = static_cast<VariableTreeItemBase*>(parent.internalPointer());

	return parentItem->childCount();
}

void VariableTreeModel::setup() {

	// Append variables to the tree
	QList<QVariant> columnData;
	columnData << "Variables"<<" ";

	// Place the fake root under the real root
	rootItem_->appendChild(new VariableTreeFakeRoot(columnData, rootItem_));

}

// Append a variable item to the tree
void VariableTreeModel::append( QList<QVariant>& columnData ) {

	// Check if we have a fakeRootItem; if not add it
	if(!rootItem_->childCount())
		rootItem_->appendChild(new VariableTreeFakeRoot(columnData, rootItem_));

  	// Get the index of the fake root item
  	QModelIndex fakeRootItemIndex= index(1,0);

	size_t nchildren = rootItem_->child(0)->childCount();

	// Call this method to append ONE row under the fake root.
	// The method emits the rowsAboutToBeInserted() signal that allows
	// for a smooth connection with the view
	beginInsertRows(fakeRootItemIndex, nchildren, nchildren);

	rootItem_->child(0)->appendChild(new VariableTreeItem(columnData, rootItem_->child(0)));

	endInsertRows();

}

// Remove the children of this model
void VariableTreeModel::clearChildren() {

	beginResetModel();

	// Actually delete the children
	rootItem_->deleteChildren();

	endResetModel();
}
