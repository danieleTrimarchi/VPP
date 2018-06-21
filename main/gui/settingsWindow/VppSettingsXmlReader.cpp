#include "VppSettingsXmlReader.h"

#include <QtWidgets/QStyle>
#include "VPPException.h"
#include "VppSettingsXmlReader.h"
#include <iostream>
#include <sstream>
#include <set>
#include <string>
#include "GetItemVisitor.h"
#include "SettingsItem.h"
#include "VppTags.h"

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
bool XmlAttribute::operator < ( XmlAttribute& rhs) const {
	return attributeName_<rhs.attributeName_;
}

// Comparison operator - based on the alphabetic order
// for the attributeName
bool XmlAttribute::operator < (const XmlAttribute& rhs) const {
	return attributeName_<rhs.attributeName_;
}

// Overload operator == to compare in set
bool XmlAttribute::operator == (const XmlAttribute& rhs) const {
	return (	attributeName_ == rhs.attributeName_ &&
			attributeValue_==rhs.attributeValue_);
}

/// Assignment operator
XmlAttribute& XmlAttribute::operator = ( string attVal ) {
	attributeValue_=attVal;
	return *this;
}

// Assignment operator
XmlAttribute& XmlAttribute::operator = ( const string& attVal ) {
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
string XmlAttribute::getString() const {
	return attributeValue_;
}

// Returns the underlying value
int XmlAttribute::getInt() const {
	// Try to convert the string to an integer.
	// Return the integer if the conversion was successful
	try{
		return stoi(attributeValue_);
	}catch(invalid_argument& e ){
		char msg[256];
		sprintf(msg,"Invalid argument when attempting to convert \'%s\' to integer",attributeValue_.c_str());
		throw VPPException(HERE,msg);
	}
}

// Returns the underlying value as a QString
QString XmlAttribute::toQString() const {
	return QString(attributeValue_.c_str());
}

// Self cast operator, returns the underlying value
XmlAttribute::operator double() const {
	// Try to convert the string to a double.
	// Return the double if the conversion was successful
	try{
		return stod(attributeValue_);
	}catch(invalid_argument& e ){
		char msg[256];
		sprintf(msg,"Invalid argument when attempting to convert \'%s\' to double",attributeValue_.c_str());
		throw VPPException(HERE,msg);
	}
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
																pFile_(pFile) {

	// Generate a new root that will be used to store the items
	pRootItem_.reset(new SettingsItemRoot);

	// who am I reading from?
	setDevice(pFile_);

	// Verify this is suitable file (vppSettings v.1.0). Otherwise throw
	if (readNextStartElement()) {

		// If everything is fine, read the item
		if (!(name() == vppSettingsTag.c_str() && attributes().value(vppSettingsVersionTag.c_str()) == vppSettingsVersion.c_str()))
			raiseError(QObject::tr("The file is not a VppSettings version 1.0 file."));
	}

	// One could assign icons here (see xbelReader example) but I do not
	// need icons for the vpp settings
}

bool VppSettingsXmlReader::readSubSection(string& sectionHeader) {

	// Verify this is suitable file (vppSettings v.1.0). Otherwise throw
	if (readNextStartElement()) {

		string name(QXmlStreamReader::name().toString().toStdString());

		// If everything is fine, read the item
		if ( QXmlStreamReader::name() == sectionHeader.c_str())
			readTreeItems(pRootItem_.get());

	}

	return !error();
}

// Return the tree populated with the items from the xml
boost::shared_ptr<SettingsItemBase> VppSettingsXmlReader::getRoot() {
	return pRootItem_;
}

void VppSettingsXmlReader::readTreeItems(Item* parentItem) {

	//Q_ASSERT(pXml_->isStartElement() && pXml_->name() == vppSettingsTag.c_str());

	while (readNextStartElement()) {

		// Instantiate a xmlAttributeSet and push all the attributes
		XmlAttributeSet attSet;

		for(size_t i=0; i<attributes().size(); i++){

			string attName= attributes().at(i).name().toString().toStdString();
			string attValue= attributes().at(i).value().toString().toStdString();
			//std::cout<<"Reading attName: "<<attName<<" and attValue: "<<attValue<<std::endl;

			// Insert the attribute into the attribute set
			try{
				XmlAttribute myAtt(attName,attValue);
				attSet.insert(myAtt);

			} catch (...) {
				std::cout<<"Something went wrong when inserting attribute "<<attName<<" in set"<<std::endl;
			}
		}

		// Build the item specified by this attribute set
		SettingsItemBase* pItem = SettingsItemBase::settingsItemFactory(attSet);
		std::cout<<"Instantiating a... "<<pItem->getDisplayName().toStdString()<<std::endl;
		std::cout<<"   variableName... "<<pItem->getVariableName().toStdString()<<std::endl;
		if(!pItem){
			string msg;
			msg += "Item named " + pItem->getDisplayName().toStdString() + " not recognized";
			throw VPPException(HERE,msg.c_str());
		}

		if(dynamic_cast<SettingsItemRoot*>(pItem)){
			// Substitute the root with the new brand new root we just created
			readTreeItems(pRootItem_.get());
		}	else {
			// Append the child to its parent
			parentItem->appendChild(pItem);
			// Read all the children of the current item
			readTreeItems(parentItem->child(parentItem->childCount()-1));
		}
	}
}

QString VppSettingsXmlReader::errorString() const {

	return QObject::tr("%1\nLine %2, column %3")
	.arg(errorString())
	.arg(lineNumber())
	.arg(columnNumber());
}

//====================================================================

// Ctor
VPPSettingsXmlReaderVisitor::VPPSettingsXmlReaderVisitor(VppSettingsXmlReader* pReader) :
	pXmlReader_(pReader) {

}

// Dtor
VPPSettingsXmlReaderVisitor::~VPPSettingsXmlReaderVisitor() {

}

// Visit a SettingsItemBase. Do nothing
void VPPSettingsXmlReaderVisitor::visit(SettingsItemBase* item){
	// Make nothing
}

void VPPSettingsXmlReaderVisitor::visit(SettingsItemRoot* pRoot) {

	// Assign the children of the xml reader root to the
	// current root
	if(pXmlReader_->readSubSection(vppSettingTreeTag))
		pXmlReader_->getRoot()->assign(pRoot);

}

