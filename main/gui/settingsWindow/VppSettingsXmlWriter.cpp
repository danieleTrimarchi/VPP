#include "VppSettingsXmlWriter.h"
#include "SettingsItem.h"
#include <iostream>
#include <QtWidgets>
#include "VPPException.h"
#include "Units.h"
#include "VppTags.h"

VppSettingsXmlWriter::VppSettingsXmlWriter(QIODevice *device) :
			pOutDevice_(device) {

	// Where am I writing to?
	setDevice(pOutDevice_);

	// How to format?
	setAutoFormatting(true);

	writeStartDocument();
	writeDTD("<!DOCTYPE vppSettings>");
	writeStartElement(vppSettingsTag.c_str());
	writeAttribute(vppSettingsVersionTag.c_str(), vppSettingsVersion.c_str());
}

// Dtor
VppSettingsXmlWriter::~VppSettingsXmlWriter() {

	writeEndDocument();
}

//====================================================================

// Ctor
VPPSettingsXmlWriterVisitor::VPPSettingsXmlWriterVisitor(VppSettingsXmlWriter* pWriter) :
		pXmlWriter_(pWriter){

	// Write the header for this section of the xml document
	pXmlWriter_->writeStartElement(vppSettingTreeTag.c_str());
}

// Dtor
VPPSettingsXmlWriterVisitor::~VPPSettingsXmlWriterVisitor() {
	pXmlWriter_->writeEndElement();
}

void VPPSettingsXmlWriterVisitor::visitBegin(SettingsItemBase* item) {

	pXmlWriter_->writeStartElement(QString(Item::itemTag_.c_str()));

	// --Write the part common to all items--
	pXmlWriter_->writeAttribute(ExpandedTag.c_str(), item->expanded() ? "yes" : "no");
	pXmlWriter_->writeAttribute(internalNameTag.c_str(),item->getInternalName());
	pXmlWriter_->writeAttribute(displayNameTag.c_str(),item->getDisplayName());
	pXmlWriter_->writeAttribute(Variable::variableNameTag_.c_str(),item->getVariableName());

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
	pXmlWriter_->writeAttribute(classNameTag.c_str(),SettingsItemBase::className_.c_str());

	// Instantiate some NoUnits just to get the name string
	NoUnit myUnit;
	pXmlWriter_->writeAttribute(unitTag.c_str(), myUnit.getUnitName().c_str());

  visitEnd(item);

	return true;
}

// Visit a SettingsItemRoot
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemRoot* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute(classNameTag.c_str(),SettingsItemRoot::className_.c_str());

	// Instantiate some NoUnits just to get the name string
	NoUnit myUnit;
	pXmlWriter_->writeAttribute(unitTag.c_str(), myUnit.getUnitName().c_str());

  visitEnd(item);

	return true;
}

// Visit a SettingsItemGroup
bool VPPSettingsXmlWriterVisitor::visit(SettingsItemGroup* item ) {

	// Do all is common to all item
	visitBegin(item);

	// Write the class name
	pXmlWriter_->writeAttribute(classNameTag.c_str(),SettingsItemGroup::className_.c_str());

	// Instantiate some NoUnits just to get the name string
	NoUnit myUnit;
	pXmlWriter_->writeAttribute(unitTag.c_str(), myUnit.getUnitName().c_str());

	visitEnd(item);

	return true;
}

