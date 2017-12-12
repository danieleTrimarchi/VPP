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

void VPPSettingsXmlWriterVisitor::visitBegin() {
	pXml_->writeStartElement(QString("Item"));
}

void VPPSettingsXmlWriterVisitor::visitEnd() {

	// This item is finished
	pXml_->writeEndElement();

}

// Visit a SettingsItemBase
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemBase* item ) {

	std::cout<<"Visit Base\n";

	// Get the internal name of this item (path)
	QString internalName = item->getInternalName();
	// Get the display name of this item
	QString itemName = item->getName();
	// Is this item expanded or folded?
	bool expanded = item->expanded();
	// Get the class name of the item. This is no good because it returns
	// the base class, which is not enough!
	//QString className( typeid(item).name() );
	QString className("SettingsItemBase");

	// And now write this guy!
	// Write an header
	pXml_->writeAttribute("InternalName",internalName);
	pXml_->writeAttribute("Name",itemName);
	pXml_->writeAttribute("ClassName",className);
	pXml_->writeAttribute("Expanded", expanded ? "yes" : "no");

	return true;
}

// Visit a SettingsItemRoot
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemRoot* item ) {

	std::cout<<"Visit Root\n";

	// Get the internal name of this item (path)
	QString internalName = item->getInternalName();
	// Get the display name of this item
	QString itemName = item->getName();
	// Is this item expanded or folded?
	bool expanded = item->expanded();
	// Get the class name of the item. This is no good because it returns
	// the base class, which is not enough!
	//QString className( typeid(item).name() );
	QString className("SettingsItemRoot");

	// And now write this guy!
	// Write an header
	pXml_->writeAttribute("InternalName",internalName);
	pXml_->writeAttribute("Name",itemName);
	pXml_->writeAttribute("ClassName",className);
	pXml_->writeAttribute("Expanded", expanded ? "yes" : "no");

	return true;
}

// Visit a SettingsItemGroup
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemGroup* item ) {

	std::cout<<"Visit Group\n";

	return true;
}

/// Visit a SettingsItemBounds
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemBounds* item ) {

	std::cout<<"Visit Bounds\n";
return true;
}

/// Visit a SettingsItem
bool VPPSettingsXmlWriterVisitor::visit(SettingsItem* item ) {
	std::cout<<"Visit SettingsItem\n";
	return true;
}

/// Visit a SettingsItemInt
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemInt* item ) {

	std::cout<<"Visit Int\n";

	// Get the internal name of this item (path)
	QString internalName = item->getInternalName();
	// Get the display name of this item
	QString itemName = item->getName();
	// Is this item expanded or folded?
	bool expanded = item->expanded();
	// Get the class name of the item. This is no good because it returns
	// the base class, which is not enough!
	//QString className( typeid(item).name() );
	QString className("SettingsItemInt");
	// Get the data stored in this item.
	QString value = item->data(columnNames::value).toString();

	// And now write this guy!
	// Write an header
	pXml_->writeAttribute("InternalName",internalName);
	pXml_->writeAttribute("Name",itemName);
	pXml_->writeAttribute("ClassName",className);
	pXml_->writeAttribute("Expanded", expanded ? "yes" : "no");
	pXml_->writeAttribute("Value", value );

	return true;
}

/// Visit a SettingsItemComboBox
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemComboBox* item ) {

	std::cout<<"Visit ComboBox\n";
	return true;
}

