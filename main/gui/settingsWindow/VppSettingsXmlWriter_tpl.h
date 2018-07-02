#include "DataColumn.h"
#include "Units.h"
#include "VppTags.h"

// Visit a SettingsItem
template <class TUnit>
bool VPPSettingsXmlWriterVisitor::visit(SettingsItem<TUnit>* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute(classNameTag.c_str(),SettingsItem<TUnit>::className_.c_str());
	// Write the data stored in this item.
	pXmlWriter_->writeAttribute(valueTag.c_str(), item->data(colNames::value_.idx_).toString() );
	// Write the data stored in this item.
	pXmlWriter_->writeAttribute(unitTag.c_str(), item->data(colNames::unit_.idx_).toString() );
	// Write the tooltip for this item.
	pXmlWriter_->writeAttribute(tooltipTag.c_str(), item->getToolTip().toString() );

  visitEnd(item);

	return true;
}

/// Visit a SettingsItemInt
template <class TUnit>
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemInt<TUnit>* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute(classNameTag.c_str(),SettingsItemInt<TUnit>::className_.c_str());
	// Write the data stored in this item.
	pXmlWriter_->writeAttribute(valueTag.c_str(), item->data(colNames::value_.idx_).toString() );
	// Write the unit for this item.
	pXmlWriter_->writeAttribute(unitTag.c_str(), item->data(colNames::unit_.idx_).toString() );
	// Write the tooltip for this item.
	pXmlWriter_->writeAttribute(tooltipTag.c_str(), item->getToolTip().toString() );

  visitEnd(item);

  return true;

}

// Visit a SettingsItemComboBox
template <class TUnit>
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemComboBox<TUnit>* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute(classNameTag.c_str(),SettingsItemComboBox<TUnit>::className_.c_str());

	// Write the number of options
	pXmlWriter_->writeAttribute(SettingsItemComboBox<TUnit>::numOptsTag_.c_str(),QString::number(item->getNumOpts()));

	// Write the available options
	for(size_t i=0; i<item->getNumOpts(); i++)
		pXmlWriter_->writeAttribute(QString("Option")+QString::number(i),item->getOption(i));

	// Write the active index
	pXmlWriter_->writeAttribute(
			SettingsItemComboBox<TUnit>::activeIndexTag_.c_str(),
			QString::number(item->getActiveIndex()));

	// Write the unit of this item.
	pXmlWriter_->writeAttribute(unitTag.c_str(), item->data(colNames::unit_.idx_).toString() );

	// Write the tooltip for this item.
	pXmlWriter_->writeAttribute(tooltipTag.c_str(), item->getToolTip().toString() );

  visitEnd(item);

	return true;
}

// Visit a SettingsItemBounds
template <class TUnit>
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemBounds<TUnit>* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute(classNameTag.c_str(),SettingsItemBounds<TUnit>::className_.c_str());

	// Write the unit of this item.
	TUnit myUnit;
	pXmlWriter_->writeAttribute(unitTag.c_str(), myUnit.getUnitName().c_str() );

  visitEnd(item);

	return true;
}

