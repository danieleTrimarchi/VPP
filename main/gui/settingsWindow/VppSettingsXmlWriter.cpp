#include "VppSettingsXmlWriter.h"
#include "SettingsItem.h"
#include <iostream>
#include <QtWidgets>
#include "VPPException.h"

VppSettingsXmlWriter::VppSettingsXmlWriter(SettingsModel* pTreeModel, QIODevice *device) :
		pTreeModel_(pTreeModel),
			pOutDevice_(device) {

	// Where am I writing to?
	setDevice(pOutDevice_);

	// How to format?
	setAutoFormatting(true);

	writeStartDocument();
	writeDTD("<!DOCTYPE vppSettings>");
	writeStartElement("vppSettings");
	writeAttribute("version", "1.0");

}

// Dtor
VppSettingsXmlWriter::~VppSettingsXmlWriter() {

	writeEndDocument();

}

//====================================================================

// Ctor
VPPSettingsXmlWriterVisitor::VPPSettingsXmlWriterVisitor(SettingsModel* pTreeModel,QIODevice*device) {

	// Instantiate the xml writer
	pXmlWriter_.reset(new VppSettingsXmlWriter(pTreeModel,device));
}

// Dtor
VPPSettingsXmlWriterVisitor::~VPPSettingsXmlWriterVisitor() {

}

void VPPSettingsXmlWriterVisitor::visitBegin(SettingsItemBase* item) {

	pXmlWriter_->writeStartElement(QString("Item"));

	// --Write the part common to all items--
	pXmlWriter_->writeAttribute("Expanded", item->expanded() ? "yes" : "no");
	pXmlWriter_->writeAttribute("InternalName",item->getInternalName());
	pXmlWriter_->writeAttribute("DisplayName",item->getDisplayName());
	pXmlWriter_->writeAttribute("VariableName",item->getVariableName());

}

void VPPSettingsXmlWriterVisitor::visitEnd(SettingsItemBase* item) {

	// And now visit my children
	for(size_t iChild=0; iChild<item->childCount(); iChild++) {
		SettingsItemBase* pChild= dynamic_cast<SettingsItemBase*>(item->child(iChild));
		if(!pChild)
			throw VPPException(HERE,"Target for dynamic cast failed");
		pChild->accept(*this);
	}

	// This item is finished
	pXmlWriter_->writeEndElement();

}

// Visit a SettingsItemBase
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemBase* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute("ClassName","SettingsItemBase");

  visitEnd(item);

	return true;
}

// Visit a SettingsItemRoot
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemRoot* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute("ClassName","SettingsItemRoot");

  visitEnd(item);

	return true;
}

// Visit a SettingsItemGroup
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemGroup* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute("ClassName","SettingsItemGroup");

	visitEnd(item);

	return true;
}

/// Visit a SettingsItemBounds
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemBounds* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute("ClassName","SettingsItemBounds");

  visitEnd(item);

	return true;
}

/// Visit a SettingsItem
bool VPPSettingsXmlWriterVisitor::visit(SettingsItem* item ) {

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
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemInt* item ) {

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

/// Visit a SettingsItemComboBox
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemComboBox* item ) {

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

