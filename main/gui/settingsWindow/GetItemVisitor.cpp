#include "GetItemVisitor.h"

// Ctor
GetItemVisitor::GetItemVisitor(Item* root):
pRoot_(root) {

}

// Dtor
GetItemVisitor::~GetItemVisitor() {

}

// ===================================================================

// Disallowed Default Ctor
GetItemByPathVisitor::GetItemByPathVisitor() :
		GetItemVisitor(0) {
}

// Ctor
GetItemByPathVisitor::GetItemByPathVisitor(Item* root) :
		GetItemVisitor(root) {
}

// Dtor
GetItemByPathVisitor::~GetItemByPathVisitor() {
 // Do nothing
}

// Items are accepting the visitor starting from root
Item* GetItemByPathVisitor::get(QString varName) const {

	if(!pRoot_)
		return 0;

	return pRoot_->accept(*this,varName);

}

// ===================================================================

// Disallowed default Ctor
GetItemByNameVisitor::GetItemByNameVisitor() :
		GetItemVisitor(0) {

}

// Ctor
GetItemByNameVisitor::GetItemByNameVisitor(Item* root):
		GetItemVisitor(root){

}

// Dtor
GetItemByNameVisitor::~GetItemByNameVisitor() {

}

// Items are accepting the visitor starting from root
Item* GetItemByNameVisitor::get(QString varName) const {

	if(!pRoot_)
		return 0;

	return pRoot_->accept(*this,varName);

}

