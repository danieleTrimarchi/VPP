#include "SettingsModel.h"
#include <iostream>

#include "GetItemVisitor.h"

SettingsModel::SettingsModel():
	VppItemModel() {

	// Instantiate the root, which is invisible
	pRootItem_.reset( new SettingsItemRoot );

}

// Copy Ctor
SettingsModel::SettingsModel(const SettingsModel& rhs) :
	VppItemModel(rhs) {

}

// Virtual Dtor
SettingsModel::~SettingsModel(){}

Qt::ItemFlags SettingsModel::flags(const QModelIndex &index) const {

	// We return editable IF the item AND the column are both editable
	if( getItem(index)->editable() == Qt::ItemIsEditable &&
			getItem(index)->getColumn(index.column())->editable() == Qt::ItemIsEditable )
		return Qt::ItemIsEditable | VppItemModel::flags(index);

	// Or, we call the base-class method
	else return VppItemModel::flags(index);

}

// Returns a ptr to the root of this model
SettingsItemBase* SettingsModel::getRoot() const {
	return dynamic_cast<SettingsItemBase*>(VppItemModel::getRoot());
}

// Setup the data of this model
void SettingsModel::setupModelData() {

	// Instantiate the SettingsItemGroups. These items are added under
	// the root. They are label and they have editable children
	// composed by label and an editable value with validator
	SettingsItemGroup* pVPPSettings = new SettingsItemGroup("VPP Settings");
	pVPPSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pVPPSettings);
	pVPPSettings->appendChild( new SettingsItemBounds("Velocity bounds","V",0,15,"m/s","Allowed boat speed bounds"));
	pVPPSettings->appendChild( new SettingsItemBounds("Heel angle bounds","PHI",-1e-5,85,"deg","Allowed boat heel angle bounds"));
	pVPPSettings->appendChild( new SettingsItemBounds("Crew position bounds","B",0,3,"[m]","Allowed boat heel angle bounds"));
	pVPPSettings->appendChild( new SettingsItemBounds("Flat bounds","F",0.4,1," ","Allowed boat heel angle bounds"));

	//	//-- Wind
	SettingsItemGroup* pWindSettings = new SettingsItemGroup("Wind Settings");
	pWindSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pWindSettings);

	pWindSettings->appendChild( new SettingsItemBounds("Wind speed bounds","VTW",2,6,"[m/s]","Real Wind speed bounds"));
	pWindSettings->appendChild( new SettingsItemInt("n_twv","NTW",5," ","Number of wind speed points") );
	pWindSettings->appendChild( new SettingsItemBounds("Wind angle bounds","TWA",35,179,"[deg]","Real Wind angle bounds"));
	pWindSettings->appendChild( new SettingsItemInt("n_alpha_tw","N_TWA",10," ","Number of wind speed points") );

	//-- Hull Settings...
	SettingsItemGroup* pHullSettings = new SettingsItemGroup("Hull Settings");
	pHullSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pHullSettings);

	pHullSettings->appendChild( new SettingsItem("LWL","LWL",0,"m","Design waterline length") 	);
	pHullSettings->appendChild( new SettingsItem("BWL","BWL",0,"m","Design waterline beam") );
	pHullSettings->appendChild( new SettingsItem("B","B",0,"m","Design maximum beam") );
	pHullSettings->appendChild( new SettingsItem("T","T",0,"m","Total draft") );
	pHullSettings->appendChild( new SettingsItem("DIVCAN","DIVCAN",0,"m^3","Displaced volume of canoe body") );
	pHullSettings->appendChild( new SettingsItem("XFB","XFB",0,"m","Longitudinal center of buoyancy LCB from fpp") );
	pHullSettings->appendChild( new SettingsItem("XFF","XFF",0,"m","Longitudinal center of flotation LCF from fpp") );
	pHullSettings->appendChild( new SettingsItem("CPL","CPL",0," ","Longitudinal prismatic coefficient") );
	pHullSettings->appendChild( new SettingsItem("HULLFF","HULLFF",0," ","Hull form factor") );
	pHullSettings->appendChild( new SettingsItem("AW","AW",0,"m^2","Design waterplane area") );
	pHullSettings->appendChild( new SettingsItem("SC","SC",0,"m^2","Wetted surface area of canoe body") );
	pHullSettings->appendChild( new SettingsItem("CMS","CMS",0," ","Midship section coefficient") );
	pHullSettings->appendChild( new SettingsItem("TCAN","TCAN",0,"m","Draft of canoe body") );
	pHullSettings->appendChild( new SettingsItem("ALT","ALT",0,"m^2","Total lateral area of yacht") );
	pHullSettings->appendChild( new SettingsItem("KG","KG",0,"m","Center of gravity above moulded base or keel") );
	pHullSettings->appendChild( new SettingsItem("KM","KM",0,"m","Transverse metacentre above moulded base or keel") );

	//-- Crew Settings...
	SettingsItemGroup* pCrewSettings = new SettingsItemGroup("Crew Settings");
	pCrewSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pCrewSettings);

	pCrewSettings->appendChild( new SettingsItem("MMVBLCRW","MMVBLCRW",0,"Kg","Movable Crew Mass") 	);

	// Keel Settings...
	SettingsItemGroup* pKeelSettings = new SettingsItemGroup("Keel Settings");
	pKeelSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pKeelSettings);

	pKeelSettings->appendChild( new SettingsItem("DVK","DVK",0,"m^3","Displaced volume of keel") );
	pKeelSettings->appendChild( new SettingsItem("APK","APK",0,"m^2","Keel planform area") );
	pKeelSettings->appendChild( new SettingsItem("ASK","ASK",0,"m^2","Keel Aspect Ratio") );
	pKeelSettings->appendChild( new SettingsItem("SK","SK",0,"m^2","Keel Wetted Surface") );
	pKeelSettings->appendChild( new SettingsItem("ZCBK","ZCBK",0,"m","Keel Vertical Center of Buoyancy") );
	pKeelSettings->appendChild( new SettingsItem("CMEK","CMEK",0,"m","Mean chord length") );
	pKeelSettings->appendChild( new SettingsItem("CHRTK","CHRTK",0,"m","Root chord length") );
	pKeelSettings->appendChild( new SettingsItem("CHRTK","CHRTK",0,"m","Tip chord length") );
	pKeelSettings->appendChild( new SettingsItem("KEELFF","KEELFF",1,"m","Keel's form factor") );
	pKeelSettings->appendChild( new SettingsItem("DELTTK","DELTTK",0," ","Mean thickness ratio of keel section") );
	pKeelSettings->appendChild( new SettingsItem("TAK","TAK",0," ","Taper ratio of keel (CHRTK/CHTPK)") );

	// Rudder Settings...
	SettingsItemGroup* pRudderSettings = new SettingsItemGroup("Rudder Settings");
	pRudderSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pRudderSettings);

	pRudderSettings->appendChild( new SettingsItem("DVR","DVR",0,"m^3","Rudder displaced volume") );
	pRudderSettings->appendChild( new SettingsItem("APR","APR",0,"m^2","Rudder planform area") );
	pRudderSettings->appendChild( new SettingsItem("SR","SR",0,"m^2","Rudder wetted surface") );
	pRudderSettings->appendChild( new SettingsItem("CHMER","CHMER",0,"m","Mean chord length") );
	pRudderSettings->appendChild( new SettingsItem("CHRTR","CHRTR",0,"m","Root chord length") );
	pRudderSettings->appendChild( new SettingsItem("CHTPR","CHTPR",0,"m","Tip chord length") );
	pRudderSettings->appendChild( new SettingsItem("DELTTR","DELTTR",0,"m","Mean thickness ratio of rudder section") );
	pRudderSettings->appendChild( new SettingsItem("RUDDFF","RUDDFF",1," ","Rudder form factor") );

	// Sail Settings...
	SettingsItemGroup* pSailSettings = new SettingsItemGroup("Sail Settings");
	pSailSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pSailSettings);

	pSailSettings->appendChild( new SettingsItem("P","P",0,"m","Main height") );
	pSailSettings->appendChild( new SettingsItem("E","E",0,"m","Main base") );
	pSailSettings->appendChild( new SettingsItem("MROACH","MROACH",0," ","Correction for mainsail roach") );
	pSailSettings->appendChild( new SettingsItem("MFLB","MFLB",0," ","Correction for mainsail roach") );
	pSailSettings->appendChild( new SettingsItem("BAD","BAD",0,"m","Boom height above deck") );
	pSailSettings->appendChild( new SettingsItem("I","I",0,"m","Fore Triangle height") );
	pSailSettings->appendChild( new SettingsItem("J","J",0,"m","Fore Triangle base") );
	pSailSettings->appendChild( new SettingsItem("LPG","LPG",0,"m","Perpendicular of longest jib") );
	pSailSettings->appendChild( new SettingsItem("SL","SL",0,"m","Spinnaker length") );
	pSailSettings->appendChild( new SettingsItem("EHM","EHM",0,"m","Mast height above deck") );
	pSailSettings->appendChild( new SettingsItem("EMDC","EMDC",0,"m","Mast average diameter") );

	// Define combo-box opts for the SailSet
	QList<QString> options;
	options.append("Standard battens");
	options.append("Full battens");
	SettingsItemComboBox* pBattensCombo = new SettingsItemComboBox("MainBattens","MFLB"," ",options,"Battens configuration");
	pSailSettings->appendChild( pBattensCombo );

	options.clear();
	options.append("Main only");
	options.append("Main and jib");
	options.append("Main and spinnaker");
	options.append("Main, jib and spinnaker");
	SettingsItemComboBox* pSettingsCombo = new SettingsItemComboBox("SailSet","SAILSET"," ",options,"Sail configuration");
	pSailSettings->appendChild( pSettingsCombo );

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


// The view notifies the model that an item has been expanded
// The model will pass the information to the item itself
// Required to remember the current state of the items when
// cloning item trees
void SettingsModel::setItemExpanded(const QModelIndex& index) {

	if (!index.isValid())
		return;

	getItem(index)->setExpanded(true);

}

// The view notifies the model that an item has been folded
// The model will pass the information to the item itself
// Required to remember the current state of the items when
// cloning item trees
void SettingsModel::setItemCollapsed(const QModelIndex& index) {

	if (!index.isValid())
		return;

	getItem(index)->setExpanded(false);
}

bool SettingsModel::setData(const QModelIndex &index, const QVariant &value, int role) {

	if (role != Qt::EditRole)
		return false;

	Item* item = getItem(index);
	bool result = item->setData(index.column(), value);

	if (result){
		emit dataChanged(index, index);
	}

	return result;
}

// Assignment operator
const SettingsModel& SettingsModel::operator=(const SettingsModel& rhs) {

	// Notify that we are about to reset the model
	beginResetModel();

	// Cleanup the root of the current model
	pRootItem_->clearChildren();

	// Loop on the children of the rhs and clone them here
	for(size_t iChild=0; iChild<rhs.getRoot()->childCount(); iChild++){
		// Add a clone of the child to the new root
		pRootItem_->appendChild(rhs.getRoot()->child(iChild)->clone());
		// Tell the child who's his parent
		pRootItem_->child(iChild)->setParentRecursive(pRootItem_.get());

	}

	// Notify that the model has been reset!
	endResetModel();

	// Appeareance...
	for(size_t iChild=0; iChild<rhs.getRoot()->childCount(); iChild++){
		// Fold or collapse the item based on the 'expanded_' flag
		// -> Create the index of this child. We assume there is
		// only one level of items, which should be true for the
		// current model but might not be always true in the future
		for(size_t iCol=0; iCol<pRootItem_->columnCount();iCol++){
			QModelIndex idx = createIndex(iChild,iCol,pRootItem_.get());
			if(pRootItem_->child(iChild)->expanded()){
				emit mustExpand(idx);
			} else {
				emit mustCollapse(idx);
			}
		}
	}
	// Return me with the new data
	return *this;
}


// Comparison operator
bool SettingsModel::operator==(const SettingsModel& rhs) {

	// If the number of children is different the models are not
	// comparable
	if(getRoot()->childCount() != rhs.getRoot()->childCount())
		return false;

	for(size_t iChild=0; iChild<getRoot()->childCount(); iChild++)
		if( *getRoot()->child(iChild) != *rhs.getRoot()->child(iChild) )
			return false;

	return true;

}

// Inverse Comparison operator
bool SettingsModel::operator!=(const SettingsModel& rhs) {
	return !(*this == rhs);
}




