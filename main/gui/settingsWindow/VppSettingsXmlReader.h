#ifndef VPP_SETTINGS_XML_READER_H
#define VPP_SETTINGS_XML_READER_H

#include <QtCore/QXmlStreamReader>
#include "boost/shared_ptr.hpp"

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

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
