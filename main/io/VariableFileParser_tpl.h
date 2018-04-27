#include "VPPException.h"
#include "DataColumn.h"
#include "SettingsItem.h"

// Visit a SettingsItem
template <class TUnit>
void VariableParserGetVisitor::visit(SettingsItem<TUnit>* pItem) {

	// Store the name and the value of this item in the parser
	pParser_->insert(pItem->getVariableName(), pItem->data(columnNames::value).toDouble());

	// Loop on the tree
	for(size_t iChild=0; iChild<pItem->childCount(); iChild++){
		SettingsItemBase* pChild= dynamic_cast<SettingsItemBase*>(pItem->child(iChild));
		if(!pChild)
			throw VPPException(HERE,"Target failed for dynamic_cast");
		pChild->accept(*this);
	}
}

////////////////////////////////////////////////////////////////////////////////

// Visit a SettingsItem
template <class TUnit>
void VariableParserGetVisitor::visit(SettingsItemComboBox<TUnit>* pItem) {

	// Store the name and the value of this item in the parser.
	// We use a convention here that maps the active index of the
	// combo-box to the value specified in SailConfig - see SailSet.h
	pParser_->insert(pItem->getVariableName(), 2*pItem->getActiveIndex()+1);

	// Loop on the tree
	for(size_t iChild=0; iChild<pItem->childCount(); iChild++){
		SettingsItemBase* pChild = dynamic_cast<SettingsItemBase*>(pItem->child(iChild));
		if(!pChild)
			throw VPPException(HERE,"Target failed for dynamic_cast");
		pChild->accept(*this);
	}
}

////////////////////////////////////////////////////////////////////////////////

/// Visit a SettingsItemBounds
template <class TUnit>
void VariableParserGetVisitor::visit(SettingsItemBounds<TUnit>* pItem) {

	// I will treat this item as a leaf, because I need to store min
	// and max with different names.

	// Get the min item in and store its value in the parser
	SettingsItemBase* pMinItem = pItem->getItemMin();
	pParser_->insert(pMinItem->getVariableName(), pMinItem->data(columnNames::value).toDouble());

	// Get the max item in and store its value in the parser
	SettingsItemBase* pMaxItem = pItem->getItemMax();
	pParser_->insert(pMaxItem->getVariableName(), pMaxItem->data(columnNames::value).toDouble());

}
