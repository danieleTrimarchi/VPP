#include "SettingsModel.h"
#include "VPPException.h"

// Ctor
SettingItem::SettingItem(SettingItem* parentItem):
pParent_(parentItem),
editable_(Qt::ItemIsEditable){

	data_ << "VariableName" << "";

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

// What child number am I?
int SettingItem::childNumber() const {
  if (pParent_)
      return pParent_->children_.indexOf(const_cast<SettingItem*>(this));

  return 0;
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

bool SettingItem::setData(int column, const QVariant &value) {

    if (column < 0 || column >= data_.size())
        return false;

    data_[column] = value;
    return true;
}

// Set if this item is editable
void SettingItem::setEditable(bool editable) {
	if(editable)
		editable_ = Qt::ItemIsEditable;
	else
		editable_ = Qt::NoItemFlags;

}

// Is this item editable?
Qt::ItemFlag SettingItem::editable() const {
	return editable_;
}


//------------------------------------------------------------------


SettingsModel::SettingsModel(QObject* parent):
			QAbstractItemModel(parent) {

	// Instantiate the root, which is invisible
	rootItem_ = new SettingItem(rootItem_);
	rootItem_->setData(0,"Variable Name");
	rootItem_->setData(1,"Value");
	rootItem_->setEditable(false);

	// Propperly fill the model
  setupModelData(rootItem_);

}


/// Setup the data of this model
void SettingsModel::setupModelData(SettingItem *parent) {

	// Instantiate the sections. These items are addedd under
	// the root. They are label and they have editable children
	// composed by label and an editable value with validator
	SettingItem* pHullSettings = new SettingItem(rootItem_);
	pHullSettings->setData(0,"Hull Settings");
	pHullSettings->setEditable(false);
	rootItem_->appendChild(pHullSettings);

	pHullSettings->appendChild(new SettingItem(pHullSettings));
	pHullSettings->child(0)->setData(0,"Lenght");
	pHullSettings->child(0)->setData(1,"12.3");

	// Second Root...
	SettingItem* pSailSettings = new SettingItem(rootItem_);
	pSailSettings->setData(0,"Sail Settings");
	pSailSettings->setEditable(false);
	rootItem_->appendChild(pSailSettings);

	pSailSettings->appendChild(new SettingItem(pSailSettings));
	pSailSettings->child(0)->setData(0,"SailArea");
	pSailSettings->child(0)->setData(1,"3.2");

}


/// Virtual Dtor
SettingsModel::~SettingsModel(){
	delete rootItem_;
}

QVariant SettingsModel::data(const QModelIndex &index, int role) const {

	// Index not valid, just return
	if (!index.isValid())
		return QVariant();

	// Display or edit...
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	  return getItem(index)->data(index.column());

	// Decorate with an icon...
	else if (role == Qt::DecorationRole )
		return getItem(index)->getIcon();


	// Default, that should never happen
	return QVariant();

}

Qt::ItemFlags SettingsModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	if(index.column()==0)
		return QAbstractItemModel::flags(index);

	return getItem(index)->editable() | QAbstractItemModel::flags(index);

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

	throw VPPException(HERE,"In append");

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

SettingItem* SettingsModel::getItem(const QModelIndex &index) const {

	if (index.isValid()) {
		SettingItem* item = static_cast<SettingItem*>(index.internalPointer());
		if (item)
			return item;
	}
	return rootItem_;
}


bool SettingsModel::setData(const QModelIndex &index, const QVariant &value, int role) {

	if (role != Qt::EditRole)
		return false;

	SettingItem* item = getItem(index);
	bool result = item->setData(index.column(), value);

	if (result)
		emit dataChanged(index, index);

	return result;
}
