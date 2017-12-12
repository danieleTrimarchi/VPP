#ifndef VPP_SETTINGS_XML_WRITER_H
#define VPP_SETTINGS_XML_WRITER_H

#include <QtCore/QXmlStreamWriter>
#include "SettingsModel.h"
#include "boost/shared_ptr.hpp"

/// Forward declarations
class SettingsItemBase;

/// XML writer class, contains a QXmlStreamWriter that
/// actually does the job of writing the XML file
class VppSettingsXmlWriter : public QXmlStreamWriter {

	public:

		/// Ctor
		VppSettingsXmlWriter(SettingsModel* pModel,QIODevice *device);

		/// Dtor
		~VppSettingsXmlWriter();

		/// Write to a given file
		//bool writeFile(QIODevice *device);

	private:

		/// Write a specific item
		//void writeItem(SettingsItemBase* item);

		/// Ptr to the file where the xml is to be written
		QIODevice* pOutDevice_;

		SettingsModel* pTreeModel_;
};

///======================================================

/// XML writer visitor, visits the items and
/// for each of them writes the xml file entries
class VPPSettingsXmlWriterVisitor {

	public:

		/// Ctor
		VPPSettingsXmlWriterVisitor(SettingsModel* pModel,QIODevice *device);

		/// Dtor
		~VPPSettingsXmlWriterVisitor();

		/// Begin visiting an item
		void visitBegin(SettingsItemBase* item);

		/// End visiting an item
		void visitEnd(SettingsItemBase* item);

		/// Visit a SettingsItemBase
		bool visit(SettingsItemBase*);

		/// Visit a SettingsItemBase
		bool visit(SettingsItemRoot*);

		/// Visit a SettingsItemGroup
		bool visit(SettingsItemGroup*);

		/// Visit a SettingsItemBounds
		bool visit(SettingsItemBounds*);

		/// Visit a SettingsItem
		bool visit(SettingsItem*);

		/// Visit a SettingsItemInt
		bool visit(SettingsItemInt*);

		/// Visit a SettingsItemComboBox
		bool visit(SettingsItemComboBox*);

	private:

		/// Ptr to the xml writer
		boost::shared_ptr<VppSettingsXmlWriter> pXml_;

};

#endif
