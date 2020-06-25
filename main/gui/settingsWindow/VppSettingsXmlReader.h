#ifndef VPP_SETTINGS_XML_READER_H
#define VPP_SETTINGS_XML_READER_H

#include <QtCore/QXmlStreamReader>

#include <string>
#include <set>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

using namespace std;

class Item;
class SettingsItemBase;
class SettingsItemRoot;

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

		/// Self cast operator, returns the underlying value
		/// See https://msdn.microsoft.com/en-us/library/wwywka61.aspx
		operator double() const;

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

class VppSettingsXmlReader : public QXmlStreamReader {

	public:

		/// Ctor
		VppSettingsXmlReader(QIODevice* pFile =Q_NULLPTR);

		/// Read from file a sub-section of the xml file
		bool readSubSection(string& sectionHeader);

		/// Return the tree populated with the items from the xml
		std::shared_ptr<SettingsItemBase> getRoot();

		/// Produce an error string reporting the location of the failure
		QString errorString() const;

	private:

		/// Read the content of the file
		void readTreeItems(Item*);

		/// Root for the settings item read from xml
		std::shared_ptr<SettingsItemBase> pRootItem_;

		/// XML file the items should be read from
		QIODevice* pFile_;
};

///======================================================

/// XML writer visitor, visits the items and
/// for each of them writes the xml file entries
class VPPSettingsXmlReaderVisitor {

	public:

		/// Ctor
		VPPSettingsXmlReaderVisitor(VppSettingsXmlReader* pReader);

		/// Dtor
		~VPPSettingsXmlReaderVisitor();

		/// Visit a SettingsItemBase. Do nothing
		void visit(SettingsItemBase* item);

		/// Visit a SettingsItemRoot. Appends all the children to the root
		void visit(SettingsItemRoot* item);

	private:

		/// Row ptr to the xml writer. The writer is
		/// not owned by this visitor
		VppSettingsXmlReader* pXmlReader_;

};

#endif
