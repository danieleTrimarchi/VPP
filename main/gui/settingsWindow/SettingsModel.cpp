#include "SettingsModel.h"
#include <iostream>

#include "GetSettingsItemVisitor.h"

SettingsModel::SettingsModel(SettingsWindowView* pView,QObject* parent):
			QAbstractItemModel(parent),
			pView_(pView) {

	// Instantiate the root, which is invisible
	pRootItem_ = new SettingsItemRoot;

	// Fill the model with the items
  setupModelData(pRootItem_);

}


/// Setup the data of this model
void SettingsModel::setupModelData(SettingsItemBase *parent) {

	// Instantiate the SettingsItemGroups. These items are added under
	// the root. They are label and they have editable children
	// composed by label and an editable value with validator
	SettingsItemGroup* pVPPSettings = new SettingsItemGroup("VPP Settings",pRootItem_);
	pRootItem_->appendChild(pVPPSettings);

	pVPPSettings->appendChild( new SettingsItemBounds("Velocity bounds",0,10,"m/s","Allowed boat speed bounds",pVPPSettings));
	pVPPSettings->appendChild( new SettingsItemInt("n_twv",5," ","Number of wind speed points",pVPPSettings) );

	SettingsItemGroup* pHullSettings = new SettingsItemGroup("Hull Settings",pRootItem_);
	pRootItem_->appendChild(pHullSettings);

	// Add the items under the group...
	pHullSettings->appendChild( new SettingsItem("LWL",15.4,"m","Design waterline length",pHullSettings) 	);
	pHullSettings->appendChild( new SettingsItem("B",3.2,"m","Design waterline beam",pHullSettings) );
	pHullSettings->appendChild( new SettingsItem("T",2.3,"m","Design waterline drought",pHullSettings) );

	// Second group...
	SettingsItemGroup* pSailSettings = new SettingsItemGroup("Sail Settings",pRootItem_);
	pRootItem_->appendChild(pSailSettings);

	pSailSettings->appendChild( new SettingsItem("SA",5.2,"m^2","Sail Area",pSailSettings) );
	pSailSettings->appendChild( new SettingsItem("J",2.2,"m","Foretriangle base",pSailSettings) );
	pSailSettings->appendChild( new SettingsItem("P",3.2,"m","Mainsail height",pSailSettings) );

	// Define combo-box opts for the SailSet
	QList<QString> options;
	options.append("Main only");
	options.append("Main and jib");
	options.append("Main and spinnaker");
	options.append("Main, jib and spinnaker");
	SettingsItemComboBox* pSettingsCombo = new SettingsItemComboBox("SailSet"," ",options,"Sail configuration",pSailSettings);
	pSailSettings->appendChild( pSettingsCombo );

	// Get a variable by name and modify its unit
	GetSettingsItemByPathVisitor v(pRootItem_);
	SettingsItemBase* pItem = v.get("/.Sail_Settings.SA");

	pItem->getColumn(columnNames::unit)->setData("newUnit");
	std::cout<<"SA value= "<<pItem->data(columnNames::value).toDouble()<<"\n";

	GetSettingsItemByNameVisitor vn(pRootItem_);
	SettingsItemBase* pNItem = vn.get("LWL");
	std::cout<<"LWL value= "<<pNItem->data(columnNames::value).toDouble()<<"\n";

	pNItem = vn.get("SailSet");
	std::cout<<"SailSet value= "<<pNItem->data(columnNames::value).toString().toStdString()<<"\n";

	std::cout<<"SailSet current index (from Visitor)= "<<pNItem->getActiveIndex()<<"\n";
	std::cout<<"SailSet current index (from Item)= "<<pSettingsCombo->getActiveIndex()<<"\n";
	std::cout<<"SailSet current Value= "<<pSettingsCombo->getActiveLabel().toStdString()<<"\n";


	// One more group...
	SettingsItemGroup* pOtherSettings = new SettingsItemGroup("OtherSettings",pRootItem_);
	pRootItem_->appendChild(pOtherSettings);

	pOtherSettings->appendChild( new SettingsItem("SA2",5.2,"m^2","Sail Area",pOtherSettings) );
	pOtherSettings->appendChild( new SettingsItem("J2",2.2,"m","Foretriangle base",pOtherSettings) );
	pOtherSettings->appendChild( new SettingsItem("P2",3.2,"m","Mainsail height",pOtherSettings) );

}

// Virtual Dtor
SettingsModel::~SettingsModel(){
	delete pRootItem_;
}


QVariant SettingsModel::data(const QModelIndex &index, int role) const {

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

Qt::ItemFlags SettingsModel::flags(const QModelIndex &index) const {

	if (!index.isValid())
		return 0;

	if(index.column()==0)
		return QAbstractItemModel::flags(index);

	// We return editable IF the item AND the column are both editable
	if( getItem(index)->editable() == Qt::ItemIsEditable &&
			 getItem(index)->getColumn(index.column())->editable() == Qt::ItemIsEditable )
		return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
	else
		return Qt::NoItemFlags | QAbstractItemModel::flags(index);

}

QVariant SettingsModel::headerData(	int section,
																	Qt::Orientation orientation,
																	int role) const {

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return pRootItem_->data(section);

	return QVariant();
}

QModelIndex SettingsModel::index(int row, int column, const QModelIndex &parent) const {

	if (!hasIndex(row, column, parent))
		return QModelIndex();

	SettingsItemBase* parentItem;

	if (!parent.isValid())
		parentItem = pRootItem_;
	else
		parentItem = static_cast<SettingsItemBase*>(parent.internalPointer());

	SettingsItemBase *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex SettingsModel::parent(const QModelIndex &index) const {

	if (!index.isValid())
		return QModelIndex();

	SettingsItemBase* childItem = static_cast<SettingsItemBase*>(index.internalPointer());
	SettingsItemBase* parentItem = childItem->parentItem();

	if (parentItem == pRootItem_)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

// How many rows?
int SettingsModel::rowCount(const QModelIndex &parent) const {

	SettingsItemBase* parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = pRootItem_;
	else
		parentItem = static_cast<SettingsItemBase*>(parent.internalPointer());

	return parentItem->childCount();
}

// How many cols?
int SettingsModel::columnCount(const QModelIndex &parent) const {

	if (parent.isValid())
		return static_cast<SettingsItemBase*>(parent.internalPointer())->columnCount();
	else
		return pRootItem_->columnCount();
}

SettingsItemBase* SettingsModel::getItem(const QModelIndex &index) const {

	if (index.isValid()) {
		SettingsItemBase* item = static_cast<SettingsItemBase*>(index.internalPointer());
		if (item)
			return item;
	}
	return pRootItem_;
}


bool SettingsModel::setData(const QModelIndex &index, const QVariant &value, int role) {

	if (role != Qt::EditRole)
		return false;

	SettingsItemBase* item = getItem(index);
	bool result = item->setData(index.column(), value);

	if (result){
		emit dataChanged(index, index);
		pView_->resizeColumnToContents(index.column());
	}

	return result;
}
