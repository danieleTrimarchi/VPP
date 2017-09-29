#include "SettingsModel.h"

// Ctor
SettingItem::SettingItem(SettingItem* parentItem):
	pParent_(parentItem){

	// Set some data by default - just string for the moment
	data_ << "VariableName" << 0.1243;

}

// Dtor
SettingItem::~SettingItem() {
  qDeleteAll(children_);
}

// Append a child under me
void SettingItem::appendChild(SettingItem* child) {
	children_.append(child);
}

// Get the i-th child
SettingItem* SettingItem::child(int row) {
  return children_.value(row);
}

// How many children do I have?
int SettingItem::childCount() const {
  return children_.count();
}

// Number of rows required to the cols of this item.
// i.e: the number of children. For the moment we
// just return 1.
int SettingItem::columnCount() const {
	return data_.count();
}

// Return the data stored in the ith column of
// this item. In this case, either the label
// or the numerical value
QVariant SettingItem::data(int column) const {
	return data_.value(column);
}

// Return the parent item
SettingItem* SettingItem::parentItem() {
	return pParent_;
}

int SettingItem::row() const {

    if (pParent_)
        return pParent_->children_.indexOf(const_cast<SettingItem*>(this));

    return 0;
}

// Returns the associated icon - in this case an empty QVariant
QVariant SettingItem::getIcon() {
	return QVariant();
}


//-----------------------------------------------


SettingsModel::SettingsModel(QObject* parent):
	QAbstractItemModel(parent) {

	// Instantiate the root, which is invisible
	rootItem_ = new SettingItem;

	// Instantiate the sections. These items are addedd under
	// the root. They are label and they have editable children
	// composed by label and an editable value with validator
//	QList<QVariant> sections;
//	sections << "Hull Data";
	SettingItem* pHullSettings = new SettingItem(rootItem_);
	SettingItem* pSailSettings = new SettingItem(rootItem_);

	rootItem_->appendChild(pHullSettings);
	rootItem_->appendChild(pSailSettings);

}


/// Virtual Dtor
SettingsModel::~SettingsModel(){
	delete rootItem_;
}

QVariant SettingsModel::data(const QModelIndex &index, int role) const {

	if (!index.isValid()){
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		SettingItem *item = static_cast<SettingItem*>(index.internalPointer());
		return item->data(index.column());
	}
	else if (role == Qt::DecorationRole ) {
		SettingItem *item = static_cast<SettingItem*>(index.internalPointer());
		if(index.column()==0)
			return item->getIcon();
		else
			return QVariant();
	}

	return QVariant();

}

Qt::ItemFlags SettingsModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	if(index.column()==0)
		return QAbstractItemModel::flags(index);

	return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QVariant SettingsModel::headerData(int section, Qt::Orientation orientation,
		int role) const {

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem_->data(section);

	return QVariant();
}

QModelIndex SettingsModel::index(int row, int column, const QModelIndex &parent) const {

	if (!hasIndex(row, column, parent))
		return QModelIndex();

	SettingItem* parentItem;

	if (!parent.isValid())
		parentItem = rootItem_;
	else
		parentItem = static_cast<SettingItem*>(parent.internalPointer());

	SettingItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex SettingsModel::parent(const QModelIndex &index) const {

	if (!index.isValid())
		return QModelIndex();

	SettingItem *childItem = static_cast<SettingItem*>(index.internalPointer());
	SettingItem *parentItem = childItem->parentItem();

	if (parentItem == rootItem_)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

// How many rows?
int SettingsModel::rowCount(const QModelIndex &parent) const {

	SettingItem *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = rootItem_;
	else
		parentItem = static_cast<SettingItem*>(parent.internalPointer());

	return parentItem->childCount();
}

// How many cols?
int SettingsModel::columnCount(const QModelIndex &parent) const {

	if (parent.isValid())
		return static_cast<SettingItem*>(parent.internalPointer())->columnCount();
	else
		return rootItem_->columnCount();
}

// Append a variable item to the tree
void SettingsModel::append( QList<QVariant>& columnData ) {

	// Get the index of the fake root item
	QModelIndex fakeRootItemIndex= index(1,0);

	size_t nchildren = rootItem_->child(0)->childCount();

	// Call this method to append ONE row under the fake root.
	// The method emits the rowsAboutToBeInserted() signal that allows
	// for a smooth connection with the view
	beginInsertRows(fakeRootItemIndex, nchildren, nchildren);

	// todo dtrimarchi :
	// WARNING : Here I get data from the outer world but I append a default
	// item because I am not still sure of what data the item contains!
	rootItem_->child(0)->appendChild(new SettingItem( rootItem_->child(0)));

	endInsertRows();

}
