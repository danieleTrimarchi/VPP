#include <QtWidgets/QStyle>
#include "VppSettingsXmlReader.h"
#include "VPPException.h"
#include "SettingsItem.h"
#include "VppSettingsXmlReader.h"
#include <iostream>
#include <set>

using namespace std;

// Ctor
XmlAttribute::XmlAttribute(const string& attName, const string& attValue) :
		attributeName_(attName),
		attributeValue_(attValue){

}

// Dtor
XmlAttribute::~XmlAttribute() {

}

// Comparison operator - based on the alphabetic order
// for the attributeName
bool XmlAttribute::operator < (const XmlAttribute& rhs) {
	return attributeName_<rhs.attributeName_;
}

// Overload operator == to compare in set
bool XmlAttribute::operator==(const XmlAttribute& rhs) const {
	return (	attributeName_ == rhs.attributeName_ &&
					attributeValue_==rhs.attributeValue_);
}

/// Assignment operator
XmlAttribute& XmlAttribute::operator = ( string attVal ) {
	attributeValue_=attVal;
	return *this;
}

// Assignment operator
XmlAttribute& XmlAttribute::operator = ( const XmlAttribute& rhs ) {
	attributeName_=rhs.attributeName_;
	attributeValue_=rhs.attributeValue_;
	return *this;
}

// Returns the underlying value
string XmlAttribute::get() const {
	return attributeValue_;
}

// Returns the underlying value as a QString
QString XmlAttribute::toQString() const {
	return QString(attributeValue_.c_str());
}

// Self cast operator, returns the underlying value
// See https://msdn.microsoft.com/en-us/library/wwywka61.aspx
XmlAttribute::operator string() const {
	return attributeValue_;
}

// Self cast operator, returns the underlying value
// See https://msdn.microsoft.com/en-us/library/wwywka61.aspx
XmlAttribute::operator double() const {
	return std::stod(attributeValue_);
}

////////////////////////////////////////////////////////////////////////

// Overload operator [] - non const variety
XmlAttribute& XmlAttributeSet::operator [] (string varName){

	XmlAttributeSet::iterator it = find(varName);
	if(it == XmlAttributeSet::end() ) {
		char msg[256];
		sprintf(msg,"Cannot find attribute named: %s",varName.c_str());
		throw VPPException(HERE,msg);
	}

	// The set only has const iterators, because  alter variables already
	// in the set might alter the order of the set and thus invalidate it.
	// Here, we force to return a non-const reference because we will never
	// modify the key of the object in the set, but its value. This is
	// guaranteed by the automatic cast to double of class Variable.
	return const_cast<XmlAttribute&>(*it);
}

// Overload operator [] - const variety
const XmlAttribute& XmlAttributeSet::operator [] (string varName) const {
	XmlAttributeSet::iterator it = find(varName);
	if(it == XmlAttributeSet::end() ) {
		char msg[256];
		sprintf(msg,"Cannot find attribute named: %s",varName.c_str());
		throw VPPException(HERE,msg);
	}
	return *it;
}

////////////////////////////////////////////////////////////////////////

// Ctor
VppSettingsXmlReader::VppSettingsXmlReader(QIODevice* pFile) :
		pRootItem_(0),
		pFile_(pFile) {

	// Instantiate a xml reader. The device will be assigned later on
	pXml_.reset(new QXmlStreamReader);

	// One could assign icons here (see xbelReader example) but I do not
	// need icons for the vpp settings
}

bool VppSettingsXmlReader::read(QIODevice *device) {

	// If there is a new source, use it
	if(device)
		pFile_ = device;

	// who am I reading from?
	pXml_->setDevice(pFile_);

	// Verify this is suitable file (vppSettings v.1.0). Otherwise throw
	if (pXml_->readNextStartElement()) {

		std::cout<<"TEXT= "<<pXml_->text().toString().toStdString()<<std::endl;
		std::cout<<"pXml_->name()="<<pXml_->name().toString().toStdString()<<
				" attributes="<<pXml_->attributes().value("version").toString().toStdString()<<std::endl;

		// If everything is fine, read the item
		if (pXml_->name() == "vppSettings" && pXml_->attributes().value("version") == "1.0")
			read(pRootItem_);
		else
			pXml_->raiseError(QObject::tr("The file is not a VppSettings version 1.0 file."));
	}

	return !pXml_->error();
}

// Return the tree populated with the items from the xml
SettingsItemBase* VppSettingsXmlReader::get() {
	return pRootItem_;
}

void VppSettingsXmlReader::read(SettingsItemBase* parentItem) {

	//Q_ASSERT(pXml_->isStartElement() && pXml_->name() == "vppSettings");

	while (pXml_->readNextStartElement()) {

		// Instantiate a xmlAttributeSet and push all the attributes
		XmlAttributeSet attSet;

		for(size_t i=0; i<pXml_->attributes().size(); i++){

			// Insert the attribute into the attribute set
			attSet.insert(	XmlAttribute(pXml_->attributes().at(i).name().toString().toStdString(),
										pXml_->attributes().at(i).value().toString().toStdString()));

			if(pXml_->attributes().at(i).name().toString().toStdString() == "ClassName"){
				std::cout<<"-> Instantiate a "<<pXml_->attributes().at(i).value().toString().toStdString()<<"\n";

			}
		}

		SettingsItemBase* pItem = SettingsItem::settingsItemFactory(attSet);

		parentItem->appendChild(pItem);

		// Recursive call to make sure we get all the children
		read(parentItem);
	}
}

QString VppSettingsXmlReader::errorString() const {

	return QObject::tr("%1\nLine %2, column %3")
	.arg(pXml_->errorString())
	.arg(pXml_->lineNumber())
	.arg(pXml_->columnNumber());
}

//====================================================================

// Ctor
VPPSettingsXmlReaderVisitor::VPPSettingsXmlReaderVisitor(QIODevice* pFile) {

	// Instantiate the xml reader
	pXmlReader_.reset(new VppSettingsXmlReader(pFile));

}

// Dtor
VPPSettingsXmlReaderVisitor::~VPPSettingsXmlReaderVisitor() {

}

void VPPSettingsXmlReaderVisitor::visit(SettingsItemBase* item) {

	// Read the content of the xml file
	if(pXmlReader_->read())
		item->appendChild(pXmlReader_->get());

}


