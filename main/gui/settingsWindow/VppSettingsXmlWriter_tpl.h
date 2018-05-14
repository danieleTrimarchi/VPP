#include "DataColumn.h"
#include "Units.h"

// Visit a SettingsItem
template <class TUnit>
bool VPPSettingsXmlWriterVisitor::visit(SettingsItem<TUnit>* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute("ClassName","SettingsItem");
	// Write the data stored in this item.
	pXmlWriter_->writeAttribute("Value", item->data(columnNames::value).toString() );
	// Write the data stored in this item.
	pXmlWriter_->writeAttribute("Unit", item->data(columnNames::unit).toString() );
	// Write the tooltip for this item.
	pXmlWriter_->writeAttribute("ToolTip", item->getToolTip().toString() );

  visitEnd(item);

	return true;
}

/// Visit a SettingsItemInt
template <class TUnit>
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemInt<TUnit>* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute("ClassName","SettingsItemInt");
	// Write the data stored in this item.
	pXmlWriter_->writeAttribute("Value", item->data(columnNames::value).toString() );
	// Write the unit for this item.
	pXmlWriter_->writeAttribute("Unit", item->data(columnNames::unit).toString() );
	// Write the tooltip for this item.
	pXmlWriter_->writeAttribute("ToolTip", item->getToolTip().toString() );

  visitEnd(item);

  return true;

}

// Visit a SettingsItemComboBox
template <class TUnit>
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemComboBox<TUnit>* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute("ClassName","SettingsItemComboBox");

	// Write the number of options
	pXmlWriter_->writeAttribute("numOpts",QString::number(item->getNumOpts()));

	// Write the available options
	for(size_t i=0; i<item->getNumOpts(); i++)
		pXmlWriter_->writeAttribute(QString("Option")+QString::number(i),item->getOption(i));

	// Write the active index
	pXmlWriter_->writeAttribute("ActiveIndex",QString::number(item->getActiveIndex()));

	// Write the unit of this item.
	pXmlWriter_->writeAttribute("Unit", item->data(columnNames::unit).toString() );

	// Write the tooltip for this item.
	pXmlWriter_->writeAttribute("ToolTip", item->getToolTip().toString() );

  visitEnd(item);

	return true;
}

// Visit a SettingsItemBounds
template <class TUnit>
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemBounds<TUnit>* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute("ClassName","SettingsItemBounds");

	// Write the unit of this item.
	TUnit myUnit;
	pXmlWriter_->writeAttribute("Unit", myUnit.getUnitName().c_str() );

  visitEnd(item);

	return true;
}

