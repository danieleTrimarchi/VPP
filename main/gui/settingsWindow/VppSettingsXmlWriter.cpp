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
	writeDTD("<!DOCTYPE vpp>");
	writeStartElement("vpp");
	writeAttribute("version", "1.0");

}

// Dtor
VppSettingsXmlWriter::~VppSettingsXmlWriter() {

	writeEndDocument();

}

//bool VppSettingsXmlWriter::writeFile(QIODevice *device) {
//	setDevice(device);
//
//	writeStartDocument();
//	writeDTD("<!DOCTYPE vpp>");
//	writeStartElement("vpp");
//	writeAttribute("version", "1.0");
//
//	// Write from root. The call is recursive therefore it will write
//	// all children in one go
//	writeItem(pTreeModel_->getRoot());
//
//	writeEndDocument();
//	return true;
//}

//void VppSettingsXmlWriter::writeItem(SettingsItemBase *item) {
//
//	// Get the internal name of this item (path)
//	QString internalName = item->getInternalName();
//	// Get the display name of this item
//	QString itemName = item->getName();
//	// Is this item expanded or folded?
//	bool expanded = item-> expanded();
//	// Get the data stored in this item.
//	QString value = item->data(columnNames::value).toString();
//	// Get the class name of the item. This is no good because it returns
//	// the base class, which is not enough!
//	QString className( typeid(item).name() );
//
//	// And now write this guy!
//	writeStartElement(className);
//	writeAttribute("InternalName",internalName);
//	writeAttribute("Name",itemName);
//	writeAttribute("ClassName",className);
//	writeAttribute("Expanded", expanded ? "yes" : "no");
//	writeAttribute("Value", value );
//
//	// Recurse to write the children
//	for (int i = 0; i < item->childCount(); ++i)
//		writeItem(item->child(i));
//
//	// This item is finished
//	writeEndElement();
//
//}

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

void VPPSettingsXmlWriterVisitor::visitEnd() {

	// This item is finished
	pXml_->writeEndElement();

}

// Visit a SettingsItemBase
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemBase* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXml_->writeAttribute("ClassName",typeid(item).name());
	return true;
}

// Visit a SettingsItemRoot
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemRoot* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXml_->writeAttribute("ClassName",typeid(item).name());
	return true;
}

// Visit a SettingsItemGroup
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemGroup* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXml_->writeAttribute("ClassName",typeid(item).name());
	return true;
}

/// Visit a SettingsItemBounds
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemBounds* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXml_->writeAttribute("ClassName",typeid(item).name());
	return true;
}

/// Visit a SettingsItem
bool VPPSettingsXmlWriterVisitor::visit(SettingsItem* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXml_->writeAttribute("ClassName",typeid(item).name());
	// Write the data stored in this item.
	pXml_->writeAttribute("Value", item->data(columnNames::value).toString() );
	return true;
}

/// Visit a SettingsItemInt
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemInt* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXml_->writeAttribute("ClassName",typeid(item).name());
	// Write the data stored in this item.
	pXml_->writeAttribute("Value", item->data(columnNames::value).toString() );
	return true;

}

/// Visit a SettingsItemComboBox
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemComboBox* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXml_->writeAttribute("ClassName",typeid(item).name());
	// Write the active choice
	pXml_->writeAttribute("Value",item->getActiveLabel());
	return true;
}

