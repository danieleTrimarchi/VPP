#include "VppItemModel.h"

#include <QStringList>
#include "VariableTreeItem.h"
#include "VPPException.h"

VppItemModel::VppItemModel(QObject *parent)
: QAbstractItemModel(parent) {

}

// Copy Ctor
VppItemModel::VppItemModel(const VppItemModel& rhs) :
		pRootItem_( rhs.pRootItem_->clone() ) {
}

// Virtual Dtor
VppItemModel::~VppItemModel() {}

// Remove the children of this model
void VppItemModel::clearChildren() {

	beginResetModel();

	// Actually delete the children
	pRootItem_->clearChildren();

	endResetModel();
}

// How many cols?
int VppItemModel::columnCount(const QModelIndex &parent) const {

	if (parent.isValid())
		return static_cast<Item*>(parent.internalPointer())->columnCount();
	else
		return pRootItem_->columnCount();
}

QVariant VppItemModel::data(const QModelIndex &index, int role) const {

	// Index not valid, just return
	if (!index.isValid())
		return QVariant();

	// Display or edit...
	if (role == Qt::DisplayRole || role == Qt::EditRole)
		return getItem(index)->data(index.column());

	if(role==Qt::FontRole)
		return getItem(index)->getFont();

	if(role==Qt::BackgroundColorRole)
		return getItem(index)->getBackGroundColor(index.column());

	if(role==Qt::ToolTipRole)
		return getItem(index)->getToolTip();

	// Decorate with an icon...
	else if (role == Qt::DecorationRole )
		return getItem(index)->getIcon();

	// Default, that should never happen
	return QVariant();

}

// Called by Qt, returns specific flags
Qt::ItemFlags VppItemModel::flags(const QModelIndex &index) const {

	if (!index.isValid())
		return 0;

	if(index.column()==0)
		return QAbstractItemModel::flags(index);

	return Qt::NoItemFlags | VppItemModel::flags(index);

}

Item* VppItemModel::getItem(const QModelIndex &index) const {

	if (index.isValid()) {
		Item* item = static_cast<Item*>(index.internalPointer());
		if (item)
			return item;
	}
	return pRootItem_.get();
}

// Returns a ptr to the root of this model
Item* VppItemModel::getRoot() const {
	return pRootItem_.get();
}

QVariant VppItemModel::headerData(	int section,
		Qt::Orientation orientation,
		int role) const {

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return pRootItem_->data(section);

	return QVariant();
}

QModelIndex VppItemModel::index(int row, int column, const QModelIndex &parent) const {

	if (!hasIndex(row, column, parent))
		return QModelIndex();

	Item* parentItem;

	if (!parent.isValid())
		parentItem = pRootItem_.get();
	else
		parentItem = static_cast<Item*>(parent.internalPointer());

	Item* childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

// Get my parent
QModelIndex VppItemModel::parent(const QModelIndex &index) const {

	if (!index.isValid())
		return QModelIndex();

	Item* childItem = static_cast<Item*>(index.internalPointer());
	Item* parentItem = childItem->parentItem();

	if(!childItem)
		throw VPPException(HERE,"index has no item associated!");

	if(!parentItem)
		return QModelIndex(); //throw VPPException(HERE,"item has no parent!");

	if (parentItem == pRootItem_.get())
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

// How many rows?
int VppItemModel::rowCount(const QModelIndex &parent) const {

	Item* parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = pRootItem_.get();
	else
		parentItem = static_cast<Item*>(parent.internalPointer());

	return parentItem->childCount();
}
