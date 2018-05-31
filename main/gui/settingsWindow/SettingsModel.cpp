#include "SettingsModel.h"
#include <iostream>

#include "GetItemVisitor.h"
#include "Units.h"

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
	pVPPSettings->appendChild( new SettingsItemBounds<MetersPerSec>("Velocity bounds","V",0,15,"Allowed boat speed bounds"));
	pVPPSettings->appendChild( new SettingsItemBounds<Degrees>("Heel angle bounds","PHI",-1e-5,85,"Allowed boat heel angle bounds"));
	pVPPSettings->appendChild( new SettingsItemBounds<Meters>("Crew position bounds","B",0,3,"Allowed boat heel angle bounds"));
	pVPPSettings->appendChild( new SettingsItemBounds<NoUnit>("Flat bounds","F",0.4,1,"Allowed boat heel angle bounds"));

	//	//-- Wind
	SettingsItemGroup* pWindSettings = new SettingsItemGroup("Wind Settings");
	pWindSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pWindSettings);

	pWindSettings->appendChild( new SettingsItemBounds<MetersPerSec>("Wind speed bounds","VTW",2,6,"Real Wind speed bounds"));
	pWindSettings->appendChild( new SettingsItemInt<NoUnit>("n_twv","NTW",5,"Number of wind speed points") );
	pWindSettings->appendChild( new SettingsItemBounds<Degrees>("Wind angle bounds","TWA",35,179,"Real Wind angle bounds"));
	pWindSettings->appendChild( new SettingsItemInt<NoUnit>("n_alpha_tw","N_TWA",10,"Number of wind speed points") );

	//-- Hull Settings...
	SettingsItemGroup* pHullSettings = new SettingsItemGroup("Hull Settings");
	pHullSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pHullSettings);

	pHullSettings->appendChild( new SettingsItem<Meters>("LWL","LWL",0,"Design waterline length") 	);
	pHullSettings->appendChild( new SettingsItem<Meters>("BWL","BWL",0,"Design waterline beam") );
	pHullSettings->appendChild( new SettingsItem<Meters>("B","B",0,"Design maximum beam") );
	pHullSettings->appendChild( new SettingsItem<Meters>("T","T",0,"Total draft") );
	pHullSettings->appendChild( new SettingsItem<CubeMeters>("DIVCAN","DIVCAN",0,"Displaced volume of canoe body") );
	pHullSettings->appendChild( new SettingsItem<Meters>("AVGFREB","AVGFREB",0,"Average freeboard") );
	pHullSettings->appendChild( new SettingsItem<Meters>("XFB","XFB",0,"Longitudinal center of buoyancy LCB from fpp") );
	pHullSettings->appendChild( new SettingsItem<Meters>("XFF","XFF",0,"Longitudinal center of flotation LCF from fpp") );
	pHullSettings->appendChild( new SettingsItem<NoUnit>("CPL","CPL",0,"Longitudinal prismatic coefficient") );
	pHullSettings->appendChild( new SettingsItem<NoUnit>("HULLFF","HULLFF",1,"Hull form factor") );
	pHullSettings->appendChild( new SettingsItem<SquareMeters>("AW","AW",0,"Design waterplane area") );
	pHullSettings->appendChild( new SettingsItem<SquareMeters>("SC","SC",0,"Wetted surface area of canoe body") );
	pHullSettings->appendChild( new SettingsItem<NoUnit>("CMS","CMS",0,"Midship section coefficient") );
	pHullSettings->appendChild( new SettingsItem<Meters>("TCAN","TCAN",0,"Draft of canoe body") );
	pHullSettings->appendChild( new SettingsItem<SquareMeters>("ALT","ALT",0,"Total lateral area of yacht") );
	pHullSettings->appendChild( new SettingsItem<Meters>("KG","KG",0,"Center of gravity above moulded base or keel") );
	pHullSettings->appendChild( new SettingsItem<Meters>("KM","KM",0,"Transverse metacentre above moulded base or keel") );

	//-- Crew Settings...
	SettingsItemGroup* pCrewSettings = new SettingsItemGroup("Crew Settings");
	pCrewSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pCrewSettings);

	pCrewSettings->appendChild( new SettingsItem<Kilograms>("MMVBLCRW","MMVBLCRW",0,"Movable Crew Mass") 	);

	// Keel Settings...
	SettingsItemGroup* pKeelSettings = new SettingsItemGroup("Keel Settings");
	pKeelSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pKeelSettings);

	pKeelSettings->appendChild( new SettingsItem<CubeMeters>("DVK","DVK",0,"Displaced volume of keel") );
	pKeelSettings->appendChild( new SettingsItem<SquareMeters>("APK","APK",0,"Keel planform area") );
	pKeelSettings->appendChild( new SettingsItem<SquareMeters>("ASK","ASK",0,"Keel Aspect Ratio") );
	pKeelSettings->appendChild( new SettingsItem<SquareMeters>("SK","SK",0,"Keel Wetted Surface") );
	pKeelSettings->appendChild( new SettingsItem<Meters>("ZCBK","ZCBK",0,"Keel Vertical Center of Buoyancy") );
	pKeelSettings->appendChild( new SettingsItem<Meters>("CHMEK","CHMEK",0,"Mean chord length") );
	pKeelSettings->appendChild( new SettingsItem<Meters>("CHRTK","CHRTK",0,"Root chord length") );
	pKeelSettings->appendChild( new SettingsItem<Meters>("CHTPK","CHTPK",0,"Tip chord length") );
	pKeelSettings->appendChild( new SettingsItem<Meters>("KEELFF","KEELFF",1,"Keel's form factor") );
	pKeelSettings->appendChild( new SettingsItem<NoUnit>("DELTTK","DELTTK",0,"Mean thickness ratio of keel section") );
	pKeelSettings->appendChild( new SettingsItem<NoUnit>("TAK","TAK",0,"Taper ratio of keel (CHRTK/CHTPK)") );

	// Rudder Settings...
	SettingsItemGroup* pRudderSettings = new SettingsItemGroup("Rudder Settings");
	pRudderSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pRudderSettings);

	pRudderSettings->appendChild( new SettingsItem<CubeMeters>("DVR","DVR",0,"Rudder displaced volume") );
	pRudderSettings->appendChild( new SettingsItem<SquareMeters>("APR","APR",0,"Rudder planform area") );
	pRudderSettings->appendChild( new SettingsItem<SquareMeters>("SR","SR",0,"Rudder wetted surface") );
	pRudderSettings->appendChild( new SettingsItem<Meters>("CHMER","CHMER",0,"Mean chord length") );
	pRudderSettings->appendChild( new SettingsItem<Meters>("CHRTR","CHRTR",0,"Root chord length") );
	pRudderSettings->appendChild( new SettingsItem<Meters>("CHTPR","CHTPR",0,"Tip chord length") );
	pRudderSettings->appendChild( new SettingsItem<Meters>("DELTTR","DELTTR",0,"Mean thickness ratio of rudder section") );
	pRudderSettings->appendChild( new SettingsItem<NoUnit>("RUDDFF","RUDDFF",1,"Rudder form factor") );

	// Sail Settings...
	SettingsItemGroup* pSailSettings = new SettingsItemGroup("Sail Settings");
	pSailSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pSailSettings);

	pSailSettings->appendChild( new SettingsItem<Meters>("P","P",0,"Main height") );
	pSailSettings->appendChild( new SettingsItem<Meters>("E","E",0,"Main base") );
	pSailSettings->appendChild( new SettingsItem<NoUnit>("MROACH","MROACH",0,"Correction for mainsail roach") );
	pSailSettings->appendChild( new SettingsItem<Meters>("BAD","BAD",0,"Boom height above deck") );
	pSailSettings->appendChild( new SettingsItem<Meters>("I","I",0,"Fore Triangle height") );
	pSailSettings->appendChild( new SettingsItem<Meters>("J","J",0,"Fore Triangle base") );
	pSailSettings->appendChild( new SettingsItem<Meters>("LPG","LPG",0,"Perpendicular of longest jib") );
	pSailSettings->appendChild( new SettingsItem<Meters>("SL","SL",0,"Spinnaker length") );
	pSailSettings->appendChild( new SettingsItem<Meters>("EHM","EHM",0,"Mast height above deck") );
	pSailSettings->appendChild( new SettingsItem<Meters>("EMDC","EMDC",0,"Mast average diameter") );

	// Define combo-box opts for the SailSet
	QList<QString> options;
	options.append("Standard battens");
	options.append("Full battens");
	SettingsItemComboBox<NoUnit>* pBattensCombo = new SettingsItemComboBox<NoUnit>("MainBattens","MFLB",options,"Battens configuration");
	pSailSettings->appendChild( pBattensCombo );

	options.clear();
	options.append("Main only");
	options.append("Main and jib");
	options.append("Main and spinnaker");
	options.append("Main, jib and spinnaker");
	SettingsItemComboBox<NoUnit>* pSettingsCombo = new SettingsItemComboBox<NoUnit>("SailSet","SAILSET",options,"Sail configuration");
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
	size_t nChild=rhs.getRoot()->childCount();
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

	// Compare child by child. Return if any difference is found
	for(size_t iChild=0; iChild<getRoot()->childCount(); iChild++)
		if( *getRoot()->child(iChild) != *rhs.getRoot()->child(iChild) )
			return false;

	// If we are here, no difference was found
	return true;

}

// Inverse Comparison operator
bool SettingsModel::operator!=(const SettingsModel& rhs) {
	return !(*this == rhs);
}

// Diagnostics print
void SettingsModel::print() {
	getRoot()->print();
}




