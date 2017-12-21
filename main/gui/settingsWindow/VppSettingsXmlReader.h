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
		bool operator < (const XmlAttribute& rhs);

		/// Overload operator == to compare in set
		bool operator==(const XmlAttribute& rhs) const;

		/// Assignment operator
		XmlAttribute& operator = ( string attVal );

		/// Assignment operator
		XmlAttribute& operator = ( const XmlAttribute& attVal );

		/// Returns the underlying value
		string get() const;

		/// Returns the underlying value as a QString
		QString toQString() const;

		/// Self cast operator, returns the underlying value
		/// See https://msdn.microsoft.com/en-us/library/wwywka61.aspx
		operator string() const;

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

class VppSettingsXmlReader {

	public:

		/// Ctor
		//VppSettingsXmlReader(QTreeWidget *treeWidget);
		VppSettingsXmlReader();

		/// Read from file
		bool read(QIODevice *device);

		/// Produce an error string reporting the location of the failure
		QString errorString() const;

	private:

		/// Read the content of the file
		void read();


//		/// Read items based on their name
//		/// All this will be somehow taken into account by visitors
//		void readTitle(QTreeWidgetItem *item);
//		void readSeparator(QTreeWidgetItem *item);
//		void readFolder(QTreeWidgetItem *item);
//		void readBookmark(QTreeWidgetItem *item);

		/// Create an item provided the info that have been read into
		/// the xml
//		QTreeWidgetItem* createChildItem(QTreeWidgetItem *item);

		/// Underlying xml reader
		boost::shared_ptr<QXmlStreamReader> pXml_;
		//QTreeWidget* pTtreeWidget_;

};

#endif
