#include "VppSettingsXmlWriter.h"
#include "SettingsItem.h"
#include <iostream>
#include <QtWidgets>

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
	pXml_.reset(new VppSettingsXmlWriter(pTreeModel,device));
}

// Dtor
VPPSettingsXmlWriterVisitor::~VPPSettingsXmlWriterVisitor() {

}

void VPPSettingsXmlWriterVisitor::visitBegin(SettingsItemBase* item) {
	pXml_->writeStartElement(QString("Item"));

	// --Write the part common to all items--
	pXml_->writeAttribute("InternalName",item->getInternalName());
	pXml_->writeAttribute("Name",item->getName());
	pXml_->writeAttribute("Expanded", item->expanded() ? "yes" : "no");

}

void VPPSettingsXmlWriterVisitor::visitEnd(SettingsItemBase* item) {

	// And now visit my children
	for(size_t iChild=0; iChild<item->childCount(); iChild++) {
		item->child(iChild)->accept(*this);
	}

	// This item is finished
	pXml_->writeEndElement();

}

// Visit a SettingsItemBase
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemBase* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXml_->writeAttribute("ClassName","SettingsItemBase");

  visitEnd(item);

	return true;
}

// Visit a SettingsItemRoot
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemRoot* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXml_->writeAttribute("ClassName","SettingsItemRoot");

  visitEnd(item);

	return true;
}

// Visit a SettingsItemGroup
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemGroup* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXml_->writeAttribute("ClassName","SettingsItemGroup");

	visitEnd(item);

	return true;
}

/// Visit a SettingsItemBounds
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemBounds* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXml_->writeAttribute("ClassName","SettingsItemBounds");

  visitEnd(item);

	return true;
}

/// Visit a SettingsItem
bool VPPSettingsXmlWriterVisitor::visit(SettingsItem* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXml_->writeAttribute("ClassName","SettingsItem");
	// Write the data stored in this item.
	pXml_->writeAttribute("Value", item->data(columnNames::value).toString() );

  visitEnd(item);

	return true;
}

/// Visit a SettingsItemInt
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemInt* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXml_->writeAttribute("ClassName","SettingsItemInt");
	// Write the data stored in this item.
	pXml_->writeAttribute("Value", item->data(columnNames::value).toString() );

  visitEnd(item);

  return true;

}

/// Visit a SettingsItemComboBox
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemComboBox* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXml_->writeAttribute("ClassName","SettingsItemComboBox");
	// Write the active choice
	pXml_->writeAttribute("Value",item->getActiveLabel());

  visitEnd(item);

	return true;
}

