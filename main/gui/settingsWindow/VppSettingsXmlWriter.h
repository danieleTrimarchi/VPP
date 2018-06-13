#ifndef VPP_SETTINGS_XML_WRITER_H
#define VPP_SETTINGS_XML_WRITER_H

#include <QtCore/QXmlStreamWriter>
#include "boost/shared_ptr.hpp"

/// Forward declarations
class SettingsItemBase;
class SettingsItemRoot;
class SettingsItemGroup;
template <class TUnit>
class SettingsItemBounds;
template <class TUnit>
class SettingsItem;
template <class TUnit>
class SettingsItemInt;
template <class TUnit>
class SettingsItemComboBox;
class SettingsModel;

using namespace std;

/// XML writer class, contains a QXmlStreamWriter that
/// actually does the job of writing the XML file
class VppSettingsXmlWriter : public QXmlStreamWriter {

	public:

		/// Ctor
		VppSettingsXmlWriter(QIODevice *device);

		/// Dtor
		~VppSettingsXmlWriter();

	private:

		/// Ptr to the file where the xml is to be written
		QIODevice* pOutDevice_;

};

///======================================================

/// XML writer visitor, visits the items and
/// for each of them writes the xml file entries
class VPPSettingsXmlWriterVisitor {

	public:

		/// Ctor
		VPPSettingsXmlWriterVisitor(VppSettingsXmlWriter* pWriter);

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
		template <class TUnit>
		bool visit(SettingsItemBounds<TUnit>*);

		/// Visit a SettingsItem
		template <class TUnit>
		bool visit(SettingsItem<TUnit>*);

		/// Visit a SettingsItemInt
		template <class TUnit>
		bool visit(SettingsItemInt<TUnit>*);

		/// Visit a SettingsItemComboBox
		template <class TUnit>
		bool visit(SettingsItemComboBox<TUnit>*);

	private:

		/// Ptr to the xml writer
		VppSettingsXmlWriter* pXmlWriter_;

};

#include "VppSettingsXmlWriter_tpl.h"

#endif
