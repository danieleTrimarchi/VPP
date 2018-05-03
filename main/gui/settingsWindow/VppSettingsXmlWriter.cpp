#include "VppSettingsXmlWriter.h"
#include "SettingsItem.h"
#include <iostream>
#include <QtWidgets>
#include "VPPException.h"
#include "Units.h"

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
	pXmlWriter_->writeAttribute("ClassName",SettingsItemBase::className_.c_str());

	// Instantiate some NoUnits just to get the name string
	NoUnit myUnit;
	pXmlWriter_->writeAttribute("Unit", myUnit.getUnitName().c_str());

  visitEnd(item);

	return true;
}

// Visit a SettingsItemRoot
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemRoot* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute("ClassName","SettingsItemRoot");

	// Instantiate some NoUnits just to get the name string
	NoUnit myUnit;
	pXmlWriter_->writeAttribute("Unit", myUnit.getUnitName().c_str());

  visitEnd(item);

	return true;
}

// Visit a SettingsItemGroup
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemGroup* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute("ClassName","SettingsItemGroup");

	// Instantiate some NoUnits just to get the name string
	NoUnit myUnit;
	pXmlWriter_->writeAttribute("Unit", myUnit.getUnitName().c_str());

	visitEnd(item);

	return true;
}

