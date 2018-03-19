#include "VppItemModel.h"

#include <QStringList>
#include "VariableTreeItem.h"

VppItemModel::VppItemModel(QObject *parent)
: QAbstractItemModel(parent) {

}

// Copy Ctor
VppItemModel::VppItemModel(const VppItemModel& rhs) :
		pRootItem_( rhs.pRootItem_->clone() ) {
}

// Virtual Dtor
VppItemModel::~VppItemModel() {}

// Returns a ptr to the root of this model
Item* VppItemModel::getRoot() const {
	return pRootItem_.get();
}

Item* VppItemModel::getItem(const QModelIndex &index) const {

	if (index.isValid()) {
		Item* item = static_cast<Item*>(index.internalPointer());
		if (item)
			return item;
	}
	return pRootItem_.get();
}

