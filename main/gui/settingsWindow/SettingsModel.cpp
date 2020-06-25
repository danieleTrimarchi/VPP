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
  pVPPSettings->appendChild( new SettingsItemBounds<MetersPerSec>(Var::vBounds_,0,15));
	pVPPSettings->appendChild( new SettingsItemBounds<Degrees>(Var::heelBounds_,-1e-5,85));
	pVPPSettings->appendChild( new SettingsItemBounds<Meters>(Var::crewBounds_,0,3));
	pVPPSettings->appendChild( new SettingsItemBounds<NoUnit>(Var::flatBounds_,0.4,1));

	//	//-- Wind
	SettingsItemGroup* pWindSettings = new SettingsItemGroup("Wind Settings");
	pWindSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pWindSettings);

	pWindSettings->appendChild( new SettingsItemBounds<MetersPerSec>(Var::vtwBounds_,2,6));
	pWindSettings->appendChild( new SettingsItemInt<NoUnit>(Var::ntw_,5) );
	pWindSettings->appendChild( new SettingsItemBounds<Degrees>(Var::atwBounds_,35,179));
	pWindSettings->appendChild( new SettingsItemInt<NoUnit>(Var::nta_,10) );

	//-- Hull Settings...
	SettingsItemGroup* pHullSettings = new SettingsItemGroup("Hull Settings");
	pHullSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pHullSettings);

	pHullSettings->appendChild( new SettingsItem<Meters>(Var::lwl_,0) 	);
	pHullSettings->appendChild( new SettingsItem<Meters>(Var::bwl_,0) );
	pHullSettings->appendChild( new SettingsItem<Meters>(Var::b_,0) );
	pHullSettings->appendChild( new SettingsItem<Meters>(Var::t_,0) );
	pHullSettings->appendChild( new SettingsItem<CubeMeters>(Var::divCan_,0) );
	pHullSettings->appendChild( new SettingsItem<Meters>(Var::avgfreb_,0) );
	pHullSettings->appendChild( new SettingsItem<Meters>(Var::xfb_,0) );
	pHullSettings->appendChild( new SettingsItem<Meters>(Var::xff_,0) );
	pHullSettings->appendChild( new SettingsItem<NoUnit>(Var::cpl_,0) );
	pHullSettings->appendChild( new SettingsItem<NoUnit>(Var::hullff_,0) );
	pHullSettings->appendChild( new SettingsItem<SquareMeters>(Var::aw_,0) );
	pHullSettings->appendChild( new SettingsItem<SquareMeters>(Var::sc_,0) );
	pHullSettings->appendChild( new SettingsItem<NoUnit>(Var::cms_,0) );
	pHullSettings->appendChild( new SettingsItem<Meters>(Var::tcan_,0) );
	pHullSettings->appendChild( new SettingsItem<SquareMeters>(Var::alt_,0) );
	pHullSettings->appendChild( new SettingsItem<Meters>(Var::kg_,0) );
	pHullSettings->appendChild( new SettingsItem<Meters>(Var::km_,0) );

	//-- Crew Settings...
	SettingsItemGroup* pCrewSettings = new SettingsItemGroup("Crew Settings");
	pCrewSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pCrewSettings);

	pCrewSettings->appendChild( new SettingsItem<Kilograms>(Var::mmvblcrw_,0) 	);

	// Keel Settings...
	SettingsItemGroup* pKeelSettings = new SettingsItemGroup("Keel Settings");
	pKeelSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pKeelSettings);

	pKeelSettings->appendChild( new SettingsItem<CubeMeters>(Var::dvk_,0) );
	pKeelSettings->appendChild( new SettingsItem<SquareMeters>(Var::apk_,0) );
	pKeelSettings->appendChild( new SettingsItem<SquareMeters>(Var::ask_,0) );
	pKeelSettings->appendChild( new SettingsItem<SquareMeters>(Var::sk_,0) );
	pKeelSettings->appendChild( new SettingsItem<Meters>(Var::zcbk_,0) );
	pKeelSettings->appendChild( new SettingsItem<Meters>(Var::chmek_,0) );
	pKeelSettings->appendChild( new SettingsItem<Meters>(Var::chrtk_,0) );
	pKeelSettings->appendChild( new SettingsItem<Meters>(Var::chtpk_,0) );
	pKeelSettings->appendChild( new SettingsItem<NoUnit>(Var::keelff_,1) );
	pKeelSettings->appendChild( new SettingsItem<NoUnit>(Var::delttk_,0) );
	pKeelSettings->appendChild( new SettingsItem<NoUnit>(Var::tak_,0) );

	// Rudder Settings...
	SettingsItemGroup* pRudderSettings = new SettingsItemGroup("Rudder Settings");
	pRudderSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pRudderSettings);

	pRudderSettings->appendChild( new SettingsItem<CubeMeters>(Var::dvr_,0) );
	pRudderSettings->appendChild( new SettingsItem<SquareMeters>(Var::apr_,0) );
	pRudderSettings->appendChild( new SettingsItem<SquareMeters>(Var::sr_,0) );
	pRudderSettings->appendChild( new SettingsItem<Meters>(Var::chmer_,0) );
	pRudderSettings->appendChild( new SettingsItem<Meters>(Var::chrtr_,0) );
	pRudderSettings->appendChild( new SettingsItem<Meters>(Var::chtpr_,0) );
	pRudderSettings->appendChild( new SettingsItem<Meters>(Var::delttr_,0) );
	pRudderSettings->appendChild( new SettingsItem<NoUnit>(Var::ruddff_,1) );

	// Sail Settings...
	SettingsItemGroup* pSailSettings = new SettingsItemGroup("Sail Settings");
	pSailSettings->setParent(pRootItem_.get());
	pRootItem_->appendChild(pSailSettings);

	pSailSettings->appendChild( new SettingsItem<Meters>(Var::p_,0) );
	pSailSettings->appendChild( new SettingsItem<Meters>(Var::e_,0) );
	pSailSettings->appendChild( new SettingsItem<NoUnit>(Var::mroach_,0) );
	pSailSettings->appendChild( new SettingsItem<Meters>(Var::bad_,0) );
	pSailSettings->appendChild( new SettingsItem<Meters>(Var::i_,0) );
	pSailSettings->appendChild( new SettingsItem<Meters>(Var::j_,0) );
	pSailSettings->appendChild( new SettingsItem<Meters>(Var::lpg_,0) );
	pSailSettings->appendChild( new SettingsItem<Meters>(Var::sl_,0) );
	pSailSettings->appendChild( new SettingsItem<Meters>(Var::ehm_,0) );
	pSailSettings->appendChild( new SettingsItem<Meters>(Var::emdc_,0) );

	// Define combo-box opts for the SailSet
	QList<QString> options;
	options.append("Standard battens");
	options.append("Full battens");
	SettingsItemComboBox<NoUnit>* pBattensCombo = new SettingsItemComboBox<NoUnit>(Var::mflb_,options);
	pSailSettings->appendChild( pBattensCombo );

	options.clear();
	options.append("Main only");
	options.append("Main and jib");
	options.append("Main and spinnaker");
	options.append("Main, jib and spinnaker");
	SettingsItemComboBox<NoUnit>* pSettingsCombo = new SettingsItemComboBox<NoUnit>(Var::sailSet_,options);
	pSailSettings->appendChild( pSettingsCombo );

	////////////////////////////////////////////////
	//// THIS PART MUST REMAIN COMMENTED!
	//	 Get a variable by name and modify its unit
	//		GetSettingsItemByPathVisitor v(pRootItem_);
	//		SettingsItemBase* pItem = v.get("/.Sail_Settings.MROACH");
	//
	//		pItem->getColumn(columnNames::unit)->setData("newUnit");
	//		std::cout<<"MROACH value= "<<pItem->data(colNames::value_.idx_).toDouble()<<"\n";
	//
	//		GetSettingsItemByNameVisitor vn(pRootItem_);
	//		SettingsItemBase* pNItem = vn.get("KEELFF");
	//		std::cout<<"KEELFF value= "<<pNItem->data(colNames::value_.idx_).toDouble()<<"\n";
	//
	//		pNItem = vn.get("SailSet");
	//		std::cout<<"SailSet value= "<<pNItem->data(colNames::value_.idx_).toString().toStdString()<<"\n";
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




