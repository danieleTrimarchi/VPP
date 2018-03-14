#ifndef VPP_SETTINGS_XML_READER_H
#define VPP_SETTINGS_XML_READER_H

#include <QtCore/QXmlStreamReader>
#include "boost/shared_ptr.hpp"
#include <string>
#include <set>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

using namespace std;

/// Container class for key, value of the attributes we read from
/// the xml
class XmlAttribute {

	public:

		/// Ctor
		XmlAttribute(const string& ="",const string& value ="");

		/// Dtor
		~XmlAttribute();

		/// Comparison operator - based on the alphabetic order
		/// for the attributeName
		bool operator < ( XmlAttribute& rhs) const;

		/// Comparison operator - based on the alphabetic order
		/// for the attributeName
		bool operator < (const XmlAttribute& rhs) const;

		/// Overload operator == to compare in set
		bool operator == (const XmlAttribute& rhs) const;

		/// Assignment operator
		XmlAttribute& operator = ( string attVal );

		/// Assignment operator
		XmlAttribute& operator = ( const string& attVal );

		/// Assignment operator
		XmlAttribute& operator = ( const XmlAttribute& attVal );

		/// Returns the underlying value
		string getString() const;

		/// Returns the underlying value
		int getInt() const;

		/// Returns the underlying value as a QString
		QString toQString() const;

//		/// Self cast operator, returns the underlying value
//		/// See https://msdn.microsoft.com/en-us/library/wwywka61.aspx
//		operator string() const;
//
//		/// Self cast operator, returns the underlying value
//		/// See https://msdn.microsoft.com/en-us/library/wwywka61.aspx
//		operator int() const;

	private:

		/// Name of this attribute
		string attributeName_;
		/// Value of this attribute
		string attributeValue_;

};

//====================================================================

class XmlAttributeSet : public set<XmlAttribute> {

	public:

		/// Overload operator [] - non const variety
		XmlAttribute& operator [] (string attName);

		/// Overload operator [] - const variety
		const XmlAttribute& operator [] (string attName) const;

	private:

};

//====================================================================

class VppSettingsXmlReader {

	public:

		/// Ctor
		VppSettingsXmlReader(QIODevice* pFile =Q_NULLPTR);

		/// Read from file
		bool read(QIODevice *device =Q_NULLPTR);

		/// Return the tree populated with the items from the xml
		boost::shared_ptr<SettingsItemBase> getRoot();

		/// Produce an error string reporting the location of the failure
		QString errorString() const;

	private:

		/// Read the content of the file
		void read(Item*);

		/// Underlying xml reader
		boost::shared_ptr<QXmlStreamReader> pXml_;

		/// Root for the settings item read from xml
		boost::shared_ptr<SettingsItemBase> pRootItem_;

		/// XML file the items should be read from
		QIODevice* pFile_;
};

///======================================================

/// XML writer visitor, visits the items and
/// for each of them writes the xml file entries
class VPPSettingsXmlReaderVisitor {

	public:

		/// Ctor
		VPPSettingsXmlReaderVisitor(QIODevice *device);

		/// Dtor
		~VPPSettingsXmlReaderVisitor();

		/// Visit a SettingsItemBase. Do nothing
		void visit(SettingsItemBase* item);

		/// Visit a SettingsItemRoot. Appends all the children to the root
		void visit(SettingsItemRoot* item);

	private:

		/// Ptr to the xml writer
		boost::shared_ptr<VppSettingsXmlReader> pXmlReader_;

};

#endif
