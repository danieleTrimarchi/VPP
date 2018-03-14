#ifndef SETTINGS_ITEM_H
#define SETTINGS_ITEM_H

/* This class implements the itemTree for the settings window */
//
//#include <QtCore/QLocale>
//#include <QtCore/QVariant>
//#include <QtCore/QString>
//#include <QtGui/QColor>
//#include <QtGui/QFont>
#include <QtGui/QPainter>
#include <QtWidgets>
///QApplication>
//#include <QtWidgets/QComboBox>
//#include <boost/shared_ptr.hpp>

#include "DataColumn.h"
#include "Item.h"

class VPPSettingsXmlWriterVisitor;
class VPPSettingsXmlReaderVisitor;
class VariableParserGetVisitor;

class XmlAttributeSet;
class XmlAttribute;

class SettingsItemBase : public Item {

	public:

		/// Dtor
		virtual ~SettingsItemBase();

		/// Accept a visitor that will write this item to XML
		virtual void accept( VPPSettingsXmlWriterVisitor& );

		/// Accept a visitor that will write this item to XML
		virtual void accept( VPPSettingsXmlReaderVisitor& );

		/// Accept a visitor that will write this item to the variableFileParser
		virtual void accept( VariableParserGetVisitor& );

		/// Assign my children to dstParent, who will 'adopt'
		/// my children. After this, I will have no more children
		void assign(SettingsItemBase* dstParent);

		/// What child number am I?
		int childNumber() const;

		/// Clone this item, which is basically equivalent to calling the copy ctor
		virtual SettingsItemBase* clone() const;

		/// The item will create its editor. The delegate
		/// is in charge to manage the brand new editor
		virtual QWidget* createEditor(QWidget *parent);

		/// Only meaningful for the combo-box item,
		/// returns zero for all the other items
		virtual size_t getActiveIndex() const;

		/// Returns the text that must be visualized
		virtual QString getActiveText(const QModelIndex &index) const;

		/// Return the backGround color for this item
		/// The backGroundRole is grey for all, but white
		/// for the editable entry of the editable items
		virtual QColor getBackGroundColor(int iColumn) const;

		/// Get the name of the variable as this will be registered
		/// in the VariableFileParser
		QString getVariableName() const;

		/// Returns the tooltip for this item, if any
		QVariant getToolTip();

		/// Visual options - requested by the Delegate
		virtual void paint(QPainter* painter, const QStyleOptionViewItem &option,
				const QModelIndex &index) const;

		/// Edit the data in the editor - requested by the Delegate
		virtual void setEditorData(QWidget *editor,const QModelIndex&);

		/// Set the data in the model - requested by the Delegate
		virtual void setModelData(	QWidget* editor,
				QAbstractItemModel* model,
				const QModelIndex& index) const;

		/// Assignment operator
		virtual const SettingsItemBase& operator=(const SettingsItemBase& rhs);

		/// Comparison operator
		virtual bool operator==(const SettingsItemBase& rhs);

		/// Factory method - builds a SettingsItem from the attributes
		/// read from xml and stored into an appropriate set
		static SettingsItemBase* settingsItemFactory(const XmlAttributeSet&);

		protected:

		/// Ctor to be called from child classes
		SettingsItemBase();

		/// Copy Ctor
		SettingsItemBase(const 	SettingsItemBase&);

		/// Name of the variable when this is used to populate the
		/// variableFileParser
		QString variableName_;

		/// Stores the tooltip for this item
		QVariant tooltip_;

		private:

};

/// ---------------------------------------------------------

class SettingsItemRoot : public SettingsItemBase {

	public:

		/// Default Ctor
		SettingsItemRoot();

		/// Ctor from xml
		SettingsItemRoot(const XmlAttributeSet&);

		/// Dtor
		virtual ~SettingsItemRoot();

		/// Accept a visitor that will write this item to XML
		virtual void accept( VPPSettingsXmlWriterVisitor& );

		/// Accept a visitor that will write this item to XML
		virtual void accept( VPPSettingsXmlReaderVisitor& );

		/// Accept a visitor that will write this item to XML
		virtual void accept( VariableParserGetVisitor& );

		/// Clone this item, which is basically equivalent to calling the copy ctor
		SettingsItemRoot* clone() const;

		/// Get the internal name of this item, used to locate it in the tree
		/// In this case the internal name is '/'
		virtual QString getInternalName() const;

		/// Get the internal name of this item, used to locate it in the tree
		/// In this case the internal name is '/'
		virtual QString getVariableName() const;

		/// Assignment operator
		virtual const SettingsItemRoot& operator=(const SettingsItemRoot& rhs);

	private:

		/// Clone this item, which is basically equivalent to calling the copy ctor
		SettingsItemRoot(const SettingsItemRoot&);

};

/// ---------------------------------------------------------

class SettingsItemGroup : public SettingsItemBase {

	public:

		/// Ctor
		SettingsItemGroup(const QVariant& name);

		/// Ctor from xml
		SettingsItemGroup(const XmlAttributeSet&);

		/// Dtor
		virtual ~SettingsItemGroup();

		/// Accept a visitor that will write this item to XML
		virtual void accept( VPPSettingsXmlWriterVisitor& );

		/// Clone this item, which is basically equivalent to calling the copy ctor
		virtual SettingsItemGroup* clone() const;

		/// Returns the font this item should be visualized
		/// with in the item tree
		virtual QFont getFont() const;

	protected:

		/// Copy Ctor, called by clone()
		SettingsItemGroup(const SettingsItemGroup&);

};

/// ---------------------------------------------------------

class SettingsItemBounds : public SettingsItemGroup {

	public:

		/// Ctor
		SettingsItemBounds(const QVariant& displayName,	//< Name visualized in the UI
				const QVariant& variableName,	//<	Name of the variable instantiated int the VariableFileParser
				double min,									//<	Min value of this variable bound
				double max,									//<	Max value of this variable bound
				const QVariant& unit,					//< Physical unit of this variable
				const QVariant& tooltip);			//< Tooltip

		/// Ctor from xml
		SettingsItemBounds(const XmlAttributeSet&);

		/// Dtor
		virtual ~SettingsItemBounds();

		/// Accept a visitor that will write this item to XML
		virtual void accept( VPPSettingsXmlWriterVisitor& );

		/// Accept a visitor that will write this item to the variableFileParser
		virtual void accept( VariableParserGetVisitor& );

		/// Clone this item, which is basically equivalent to calling the copy ctor
		virtual SettingsItemBounds* clone() const;

		/// Returns the font this item should be visualized
		/// with in the item tree
		virtual QFont getFont() const;

		/// Returns a handle on the item that represents the min in this bound
		SettingsItemBase* getItemMin();

		/// Returns a handle on the item that represents the max in this bound
		SettingsItemBase* getItemMax();

		/// Get the min value of this bound
		double getMin();

		/// Get the max value of this bound
		double getMax();

	private:

		/// Copy Ctor, called by clone()
		SettingsItemBounds(const SettingsItemBounds&);

};

/// ---------------------------------------------------------

class SettingsItem : public SettingsItemBase {

	public:

		/// Ctor
		SettingsItem(const QVariant& displayName,	//< Name visualized in the UI
				const QVariant& variableName, //<	Name of the variable instantiated int the VariableFileParser
				const QVariant&value, 				//<	Value of this variable
				const QVariant&unit,					//< Physical unit of this variable
				const QVariant& tooltip);			//< Tooltip

		/// Ctor from xml
		SettingsItem(const XmlAttributeSet&);

		/// Dtor
		virtual ~SettingsItem();

		/// Accept a visitor that will write this item to XML
		virtual void accept( VPPSettingsXmlWriterVisitor& );

		/// Accept a visitor that will write this item to XML
		virtual void accept( VariableParserGetVisitor& );

		/// Clone this item, which is basically equivalent to calling the copy ctor
		virtual SettingsItem* clone() const;

		/// Return the backGround color for this item
		/// The backGroundRole is grey for all, but white
		/// for the editable entry of the editable items
		virtual QColor getBackGroundColor(int iColumn) const;

		/// The item will give the Delegate the editor
		/// to be properly edited - in this case a QLineEdit with
		/// double validator
		virtual QWidget* createEditor(QWidget *parent);

		/// Edit the data in the editor
		virtual void setEditorData(QWidget *editor,const QModelIndex&);

		/// Set the data in the model
		virtual void setModelData(	QWidget* editor,
				QAbstractItemModel* model,
				const QModelIndex& index) const;

		/// Visual options - requested by the Delegate - this directly derives from MEMS+
		/// Decorates the base class method paint
		virtual void paint(	QPainter* painter,
				const QStyleOptionViewItem &option,
				const QModelIndex &index) const;

	protected:

		/// Copy Ctor, called by clone()
		SettingsItem(const SettingsItem&);

	private:

};


/// ---------------------------------------------------------

class SettingsItemInt : public SettingsItem {

	public:

		/// Ctor
		SettingsItemInt(	const QVariant& displayName, 	//< Name visualized in the UI
				const QVariant& varName,			//<	Name of the variable instantiated int the VariableFileParser
				const QVariant& value,				//<	Value of this variable
				const QVariant& unit,					//< Physical unit of this variable
				const QVariant& tooltip);			//< Tooltip

		/// Ctor from xml
		SettingsItemInt(const XmlAttributeSet&);

		/// Dtor
		virtual ~SettingsItemInt();

		/// Accept a visitor that will write this item to XML
		virtual void accept( VPPSettingsXmlWriterVisitor& );

		/// Clone this item, which is basically equivalent to calling the copy ctor
		virtual SettingsItemInt* clone() const;

		/// The item will give the Delegate the editor
		/// to be properly edited - in this case a spinbox
		virtual QWidget* createEditor(QWidget *parent) override;

		/// Edit the data in the editor
		virtual void setEditorData(QWidget *editor,const QModelIndex& index) override;

		/// Set the data in the model
		virtual void setModelData(QWidget *editor, QAbstractItemModel *model,
				const QModelIndex &index) const override;

	private:

		/// Copy ctor, called by clone()
		SettingsItemInt(const SettingsItemInt&);

};

/// ---------------------------------------------------------

class SettingsItemComboBox : public SettingsItem {

	public:

		/// Ctor
		SettingsItemComboBox(const QVariant& displayName,	//< Name visualized in the UI
				const QVariant& variableName,	//<	Name of the variable instantiated int the VariableFileParser
				const QVariant& unit,					//<	Physical unit of this variable
				const QList<QString>& options,//<	List of options available in this combo-box
				const QVariant& tooltip);			//< ToolTip

		/// Ctor from xml
		SettingsItemComboBox(const XmlAttributeSet&);

		/// Dtor
		virtual ~SettingsItemComboBox();

		/// Accept a visitor that will write this item to XML
		virtual void accept( VPPSettingsXmlWriterVisitor& );

		/// Accept a visitor that will write this item to XML
		virtual void accept( VariableParserGetVisitor& );

		/// Clone this item, which is basically equivalent to calling the copy ctor
		virtual SettingsItemComboBox* clone() const;

		/// The item will give the Delegate the editor
		/// to be properly edited - in this case a spinbox
		virtual QWidget* createEditor(QWidget *parent) override;

		/// Edit the data in the editor
		virtual void setEditorData(QWidget *editor,const QModelIndex& index) override;

		/// Set the data in the model
		virtual void setModelData(QWidget *editor, QAbstractItemModel *model,
				const QModelIndex &index) const override;

		/// Only meaningful for the combo-box item, returns zero for all the
		/// other items
		virtual size_t getActiveIndex() const;

		///  Returns the label of the active (selected) item
		/// Called by the parent 'paint' method
		QString getActiveText(const QModelIndex &index) const;

		/// How many options are available to this combo-box?
		size_t getNumOpts() const;

		/// Get the i-th option for this Combo-Box
		QString getOption(size_t) const;

		/// Assignment operator
		virtual const SettingsItemComboBox& operator=(const SettingsItemComboBox& rhs);

	private:

		/// Copy Ctor
		SettingsItemComboBox(const SettingsItemComboBox& rhs);

		/// Options shown in this combo box
		QList<QString> opts_;

		/// Store the index of the choice selected by the
		/// user when editing the combo-box editor
		/// Declared mutable, as we need to set this inside
		/// a const method setModelData
		mutable size_t activeIndex_;
};


#endif
