#include "GetSettingsItemVisitor.h"

// Ctor
GetSettingsItemVisitor::GetSettingsItemVisitor(SettingsItemBase* root):
pRoot_(root) {

}

// Dtor
GetSettingsItemVisitor::~GetSettingsItemVisitor() {

}

// ===================================================================

// Disallowed Default Ctor
GetSettingsItemByPathVisitor::GetSettingsItemByPathVisitor() :
		GetSettingsItemVisitor(0) {
}

// Ctor
GetSettingsItemByPathVisitor::GetSettingsItemByPathVisitor(SettingsItemBase* root) :
		GetSettingsItemVisitor(root) {
}

// Dtor
GetSettingsItemByPathVisitor::~GetSettingsItemByPathVisitor() {
 // Do nothing
}

// Items are accepting the visitor starting from root
SettingsItemBase* GetSettingsItemByPathVisitor::get(QString varName) const {

	if(!pRoot_)
		return 0;

	return pRoot_->accept(*this,varName);

}

// ===================================================================

// Disallowed default Ctor
GetSettingsItemByNameVisitor::GetSettingsItemByNameVisitor() :
		GetSettingsItemVisitor(0) {

}

// Ctor
GetSettingsItemByNameVisitor::GetSettingsItemByNameVisitor(SettingsItemBase* root):
		GetSettingsItemVisitor(root){

}

// Dtor
GetSettingsItemByNameVisitor::~GetSettingsItemByNameVisitor() {

}

// Items are accepting the visitor starting from root
SettingsItemBase* GetSettingsItemByNameVisitor::get(QString varName) const {

	if(!pRoot_)
		return 0;

	return pRoot_->accept(*this,varName);

}

