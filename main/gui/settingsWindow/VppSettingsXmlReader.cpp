#include "SettingsItem.h"
#include <QtWidgets/QStyle>
#include "VppSettingsXmlReader.h"
#include "VPPException.h"
#include "VppSettingsXmlReader.h"
#include "GetSettingsItemVisitor.h"
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

		// If everything is fine, read the item
		if (pXml_->name() == "vppSettings" && pXml_->attributes().value("version") == "1.0")
			read(pRootItem_.get());

		else
			pXml_->raiseError(QObject::tr("The file is not a VppSettings version 1.0 file."));
	}

	return !pXml_->error();
}

// Return the tree populated with the items from the xml
boost::shared_ptr<SettingsItemBase> VppSettingsXmlReader::getRoot() {
	return pRootItem_;
}

void VppSettingsXmlReader::read(SettingsItemBase* parentItem) {

	//Q_ASSERT(pXml_->isStartElement() && pXml_->name() == "vppSettings");

	while (pXml_->readNextStartElement()) {

		// Instantiate a xmlAttributeSet and push all the attributes
		XmlAttributeSet attSet;

		for(size_t i=0; i<pXml_->attributes().size(); i++){

			string attName= pXml_->attributes().at(i).name().toString().toStdString();
			string attValue= pXml_->attributes().at(i).value().toString().toStdString();

			// Insert the attribute into the attribute set
			try{
				XmlAttribute myAtt(attName,attValue);
				attSet.insert(myAtt);

			} catch (...){
				std::cout<<"Something went wrong when inserting attribute "<<attName<<" in set"<<std::endl;
			}
		}

		// Build the item specified by this attribute set
		SettingsItemBase* pItem = SettingsItem::settingsItemFactory(attSet);
//		std::cout<<"Instantiating a... "<<pItem->getDisplayName().toStdString()<<std::endl;
//		std::cout<<"   variableName... "<<pItem->getVariableName().toStdString()<<std::endl;

		if(dynamic_cast<SettingsItemRoot*>(pItem)){
			// Substitute the root with the new brand new root we just created
			read(pRootItem_.get());
		}	else {
			// Append the child to its parent
			parentItem->appendChild(pItem);
			// Read all the children of the current item
			read(parentItem->child(parentItem->childCount()-1));
		}
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

// Visit a SettingsItemBase. Do nothing
void VPPSettingsXmlReaderVisitor::visit(SettingsItemBase* item){
	// Make nothing
}

void VPPSettingsXmlReaderVisitor::visit(SettingsItemRoot* pRoot) {

	// Assign the children of the xml reader root to the
	// current root
	if(pXmlReader_->read())
		pXmlReader_->getRoot()->assign(pRoot);

}

