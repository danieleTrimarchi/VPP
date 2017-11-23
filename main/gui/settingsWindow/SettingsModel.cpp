#include "SettingsModel.h"
#include <iostream>
#include "VPPException.h"
#include "GetSettingsItemVisitor.h"

SettingsModel::SettingsModel():
QAbstractItemModel() {

	// Instantiate the root, which is invisible
	pRootItem_ = new SettingsItemRoot;

}

// Copy Ctor
SettingsModel::SettingsModel(const SettingsModel& rhs) :
							pRootItem_( rhs.pRootItem_->clone() ) {

}


/// Setup the data of this model
void SettingsModel::setupModelData() {

	// Instantiate the SettingsItemGroups. These items are added under
	// the root. They are label and they have editable children
	// composed by label and an editable value with validator
	SettingsItemGroup* pVPPSettings = new SettingsItemGroup("VPP Settings");
	pVPPSettings->setParent(pRootItem_);
	pRootItem_->appendChild(pVPPSettings);

	pVPPSettings->appendChild( new SettingsItemBounds("Velocity bounds",0,15,"m/s","Allowed boat speed bounds"));
	//	pVPPSettings->appendChild( new SettingsItemBounds("Heel angle bounds",-1e-5,85,"deg","Allowed boat heel angle bounds"));
	//	pVPPSettings->appendChild( new SettingsItemBounds("Crew position bounds",0,3,"[m]","Allowed boat heel angle bounds"));
	//	pVPPSettings->appendChild( new SettingsItemBounds("Flat bounds",0.4,1," ","Allowed boat heel angle bounds"));
	//
	//	//-- Wind
	//	SettingsItemGroup* pWindSettings = new SettingsItemGroup("Wind Settings");
	//	pWindSettings->setParent(pRootItem_);
	//	pRootItem_->appendChild(pWindSettings);
	//
	//	pWindSettings->appendChild( new SettingsItemBounds("Wind speed bounds",2,6,"[m/s]","Real Wind speed bounds"));
	//	pWindSettings->appendChild( new SettingsItemInt("n_twv",5," ","Number of wind speed points") );
	//	pWindSettings->appendChild( new SettingsItemBounds("Wind angle bounds",35,179,"[deg]","Real Wind angle bounds"));
	//	pWindSettings->appendChild( new SettingsItemInt("n_alpha_tw",10," ","Number of wind speed points") );
	//
	//	//-- Hull Settings...
	//	SettingsItemGroup* pHullSettings = new SettingsItemGroup("Hull Settings");
	//	pHullSettings->setParent(pRootItem_);
	//	pRootItem_->appendChild(pHullSettings);
	//
	//	pHullSettings->appendChild( new SettingsItem("LWL",0,"m","Design waterline length") 	);
	//	pHullSettings->appendChild( new SettingsItem("B",0,"m","Design waterline beam") );
	//	pHullSettings->appendChild( new SettingsItem("B",0,"m","Design maximum beam") );
	//	pHullSettings->appendChild( new SettingsItem("T",0,"m","Total draft") );
	//	pHullSettings->appendChild( new SettingsItem("DIVCAN",0,"m^3","Displaced volume of canoe body") );
	//	pHullSettings->appendChild( new SettingsItem("XFB",0,"m","Longitudinal center of buoyancy LCB from fpp") );
	//	pHullSettings->appendChild( new SettingsItem("XFF",0,"m","Longitudinal center of flotation LCF from fpp") );
	//	pHullSettings->appendChild( new SettingsItem("CPL",0," ","Longitudinal prismatic coefficient") );
	//	pHullSettings->appendChild( new SettingsItem("HULLFF",0," ","Hull form factor") );
	//	pHullSettings->appendChild( new SettingsItem("AW",0,"m^2","Design waterplane area") );
	//	pHullSettings->appendChild( new SettingsItem("SC",0,"m^2","Wetted surface area of canoe body") );
	//	pHullSettings->appendChild( new SettingsItem("CMS",0," ","Midship section coefficient") );
	//	pHullSettings->appendChild( new SettingsItem("TCAN",0,"m","Draft of canoe body") );
	//	pHullSettings->appendChild( new SettingsItem("ALT",0,"m^2","Total lateral area of yacht") );
	//	pHullSettings->appendChild( new SettingsItem("KG",0,"m","Center of gravity above moulded base or keel") );
	//	pHullSettings->appendChild( new SettingsItem("KM",0,"m","Transverse metacentre above moulded base or keel") );
	//
	//	//-- Crew Settings...
	//	SettingsItemGroup* pCrewSettings = new SettingsItemGroup("Crew Settings");
	//	pCrewSettings->setParent(pRootItem_);
	//	pRootItem_->appendChild(pCrewSettings);
	//
	//	pCrewSettings->appendChild( new SettingsItem("MMVBLCRW",0,"Kg","Movable Crew Mass") 	);
	//
	//	// Keel Settings...
	//	SettingsItemGroup* pKeelSettings = new SettingsItemGroup("Keel Settings");
	//	pKeelSettings->setParent(pRootItem_);
	//	pRootItem_->appendChild(pKeelSettings);
	//
	//	pKeelSettings->appendChild( new SettingsItem("DVK",0,"m^3","Displaced volume of keel") );
	//	pKeelSettings->appendChild( new SettingsItem("APK",0,"m^2","Keel planform area") );
	//	pKeelSettings->appendChild( new SettingsItem("ASK",0,"m^2","Keel Aspect Ratio") );
	//	pKeelSettings->appendChild( new SettingsItem("SK",0,"m^2","Keel Wetted Surface") );
	//	pKeelSettings->appendChild( new SettingsItem("ZCBK",0,"m","Keel Vertical Center of Buoyancy") );
	//	pKeelSettings->appendChild( new SettingsItem("CMEK",0,"m","Mean chord length") );
	//	pKeelSettings->appendChild( new SettingsItem("CHRTK",0,"m","Root chord length") );
	//	pKeelSettings->appendChild( new SettingsItem("CHRTK",0,"m","Tip chord length") );
	//	pKeelSettings->appendChild( new SettingsItem("KEELFF",1,"m","Keel's form factor") );
	//	pKeelSettings->appendChild( new SettingsItem("DELTTK",0," ","Mean thickness ratio of keel section") );
	//	pKeelSettings->appendChild( new SettingsItem("TAK",0," ","Taper ratio of keel (CHRTK/CHTPK)") );
	//
	//	// Rudder Settings...
	//	SettingsItemGroup* pRudderSettings = new SettingsItemGroup("Rudder Settings");
	//	pRudderSettings->setParent(pRootItem_);
	//	pRootItem_->appendChild(pRudderSettings);
	//
	//	pRudderSettings->appendChild( new SettingsItem("DVR",0,"m^3","Rudder displaced volume") );
	//	pRudderSettings->appendChild( new SettingsItem("APR",0,"m^2","Rudder planform area") );
	//	pRudderSettings->appendChild( new SettingsItem("SR",0,"m^2","Rudder wetted surface") );
	//	pRudderSettings->appendChild( new SettingsItem("CHMER",0,"m","Mean chord length") );
	//	pRudderSettings->appendChild( new SettingsItem("CHRTR",0,"m","Root chord length") );
	//	pRudderSettings->appendChild( new SettingsItem("CHTPR",0,"m","Tip chord length") );
	//	pRudderSettings->appendChild( new SettingsItem("DELTTR",0,"m","Mean thickness ratio of rudder section") );
	//	pRudderSettings->appendChild( new SettingsItem("RUDDFF",1," ","Rudder form factor") );
	//
	//	// Sail Settings...
	//	SettingsItemGroup* pSailSettings = new SettingsItemGroup("Sail Settings");
	//	pSailSettings->setParent(pRootItem_);
	//	pRootItem_->appendChild(pSailSettings);
	//
	//	pSailSettings->appendChild( new SettingsItem("P",0,"m","Main height") );
	//	pSailSettings->appendChild( new SettingsItem("E",0,"m","Main base") );
	//	pSailSettings->appendChild( new SettingsItem("MROACH",0," ","Correction for mainsail roach") );
	//	pSailSettings->appendChild( new SettingsItem("MFLB",0," ","Correction for mainsail roach") );
	//	pSailSettings->appendChild( new SettingsItem("BAD",0,"m","Boom height above deck") );
	//	pSailSettings->appendChild( new SettingsItem("I",0,"m","Fore Triangle height") );
	//	pSailSettings->appendChild( new SettingsItem("J",0,"m","Fore Triangle base") );
	//	pSailSettings->appendChild( new SettingsItem("LPG",0,"m","Perpendicular of longest jib") );
	//	pSailSettings->appendChild( new SettingsItem("SL",0,"m","Spinnaker length") );
	//	pSailSettings->appendChild( new SettingsItem("EHM",0,"m","Mast height above deck") );
	//	pSailSettings->appendChild( new SettingsItem("EMDC",0,"m","Mast average diameter") );
	//
	//	// Define combo-box opts for the SailSet
	//	QList<QString> options;
	//	options.append("Standard battens");
	//	options.append("Full battens");
	//	SettingsItemComboBox* pBattensCombo = new SettingsItemComboBox("MainBattens"," ",options,"Battens configuration");
	//	pSailSettings->appendChild( pBattensCombo );
	//
	//	options.clear();
	//	options.append("Main only");
	//	options.append("Main and jib");
	//	options.append("Main and spinnaker");
	//	options.append("Main, jib and spinnaker");
	//	SettingsItemComboBox* pSettingsCombo = new SettingsItemComboBox("SailSet"," ",options,"Sail configuration");
	//	pSailSettings->appendChild( pSettingsCombo );
	//

	////////////////////////////////////////////////
	//// THIS PART REMAINS COMMENTED!
	//	 Get a variable by name and modify its unit
	//		GetSettingsItemByPathVisitor v(pRootItem_);
	//		SettingsItemBase* pItem = v.get("/.Sail_Settings.MROACH");
	//
	//		pItem->getColumn(columnNames::unit)->setData("newUnit");
	//		std::cout<<"MROACH value= "<<pItem->data(columnNames::value).toDouble()<<"\n";
	//
	//		GetSettingsItemByNameVisitor vn(pRootItem_);
	//		SettingsItemBase* pNItem = vn.get("KEELFF");
	//		std::cout<<"KEELFF value= "<<pNItem->data(columnNames::value).toDouble()<<"\n";
	//
	//		pNItem = vn.get("SailSet");
	//		std::cout<<"SailSet value= "<<pNItem->data(columnNames::value).toString().toStdString()<<"\n";
	//
	//		std::cout<<"SailSet current index (from Visitor)= "<<pNItem->getActiveIndex()<<"\n";
	//		std::cout<<"SailSet current index (from Item)= "<<pSettingsCombo->getActiveIndex()<<"\n";
	//		std::cout<<"SailSet current Value= "<<pSettingsCombo->getActiveLabel().toStdString()<<"\n";

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

// Returns a ptr to the root of this model
SettingsItemBase* SettingsModel::getRoot() const {
	return pRootItem_;
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

/// Get my parent
QModelIndex SettingsModel::parent(const QModelIndex &index) const {

	if (!index.isValid())
		return QModelIndex();

	SettingsItemBase* childItem = static_cast<SettingsItemBase*>(index.internalPointer());
	SettingsItemBase* parentItem = childItem->parentItem();

	if(!childItem)
		throw VPPException(HERE,"index has no item associated!");

	if(!parentItem)
		return QModelIndex(); //throw VPPException(HERE,"item has no parent!");

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
	}

	return result;
}

// Assignment operator
const SettingsModel& SettingsModel::operator=(const SettingsModel& rhs) {

	std::cout<<"SettingsModel operator =\n";
	// Cleanup the root of the current model
	pRootItem_->clearChildren();


	// Loop on the children of the rhs and clone them here
	for(size_t iChild=0; iChild<rhs.getRoot()->childCount(); iChild++){
		// Add a clone of the child to the new root
		pRootItem_->appendChild(rhs.getRoot()->child(iChild)->clone());
		// Tell the child who's his parent
		pRootItem_->child(iChild)->setParentRecursive(pRootItem_);
	}

	// Return me with the new data
	return *this;
}



