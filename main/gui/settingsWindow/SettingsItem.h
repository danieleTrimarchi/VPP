#ifndef SETTINGS_ITEM_H
#define SETTINGS_ITEM_H

/* This class implements the itemTree for the settings window */

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include "SettingsColumn.h"

/// Forward declarations
class GetSettingsItemByPathVisitor;
class GetSettingsItemByNameVisitor;

class SettingsItemBase {

public:

	/// Dtor
	virtual ~SettingsItemBase();

	/// Accept a visitor, the role of which is to iterate and
	/// return an item by path
	SettingsItemBase* accept(const GetSettingsItemByPathVisitor&,QString);

	/// Accept a visitor, the role of which is to iterate and
	/// return an item by name
	SettingsItemBase* accept(const GetSettingsItemByNameVisitor&, QString);

	/// Append a child under me
	void appendChild(SettingsItemBase* child);

	/// Remove all children under me
	void clearChildren();

	/// Get the i-th child
	SettingsItemBase* child(int row);

	/// Get a child by name
	SettingsItemBase* child(QString& childName);

	/// Get a child by path - inclusive of the child name, of course
	SettingsItemBase* childPath(QString& childPath);

	/// How many children do I have?
	int childCount() const;

	/// What child number am I?
	int childNumber() const;

	/// Clone this item, which is basically equivalent to calling the copy ctor
	virtual SettingsItemBase* clone() const;

	/// Number of cols required for this item.
	int columnCount() const;

	/// The item will create its editor. The delegate
	/// is in charge to manage the brand new editor
	virtual QWidget* createEditor(QWidget *parent);

	/// Return the data stored in the i-th column of
	/// this item. In this case, either the label
	/// or the numerical value
	QVariant data(int column) const;

	/// Is this item editable?
	Qt::ItemFlag editable() const;

	/// Only meaningful for the combo-box item,
	/// returns zero for all the other items
	virtual size_t getActiveIndex() const;

	/// Only meaningful for the combo-box item,
	/// returns an empty QString for the base-class
	virtual QString getActiveLabel() const;

	/// Return the backGround color for this item
	/// The backGroundRole is grey for all, but white
	/// for the editable entry of the editable items
	virtual QColor getBackGroundColor(int iColumn) const;

	/// Get a reference to the columns vector
	std::vector<SettingsColumn*>& getColumnVector();

	/// Get a reference to the i-th column in the vector
	SettingsColumn* getColumn(const int);

	/// Returns the font this item should be visualized
	/// with in the item tree
	virtual QFont getFont() const;

	/// Returns the associated icon - in this case an empty QVariant
	QVariant getIcon();

	/// Get the internal name of this item, used to locate it in the tree
	QString getInternalName();

	/// Returns the tooltip for this item, if any
	QVariant getToolTip();

	/// Visual options - requested by the Delegate
	virtual void paint(QPainter* painter, const QStyleOptionViewItem &option,
			const QModelIndex &index) const;

	/// Return the parent item
	SettingsItemBase* parentItem();

	/// Remove a child by position
	void removeChild(size_t iChild);

	/// What child number am I for my parent?
	int row() const;

	/// Set the data for this item
	bool setData(int column, const QVariant &value);

	/// Set if this item is editable
	void setEditable(bool);

	/// Edit the data in the editor - requested by the Delegate
	virtual void setEditorData(QWidget *editor,const QModelIndex&);

	/// Set the data in the model - requested by the Delegate
	virtual void setModelData(	QWidget* editor,
			QAbstractItemModel* model,
			const QModelIndex& index) const;

	/// Assignment operator
	virtual const SettingsItemBase& operator=(const SettingsItemBase& rhs);

	/// Set the parent of this item
	void setParent(SettingsItemBase* parentItem);

	/// Recursively set the parent of this item and of all its children
	void setParentRecursive(SettingsItemBase* parentItem);

	/// Return the flag 'expanded'
	bool expanded() const;

public slots:

	/// This slot is triggered when the item is expanded in the view
	void setExpanded(bool expanded);

protected:

	/// Ctor to be called from child classes
	SettingsItemBase();

	/// Copy Ctor
	SettingsItemBase(const 	SettingsItemBase&);

	/// Set the internal name of this item
	virtual void setInternalName(const QVariant&);

	/// Columns the SettingsItem must be able to fill
	/// The model is build on top of these columns
	std::vector<SettingsColumn*> columns_;

	/// Internal name of the Item, used to locate it in the tree
	QString path_;

	/// Stores the tooltip for this item
	QVariant tooltip_;

	/// Parent of this item
	SettingsItemBase* pParent_;

private:

	/// Children of this item
	QList<SettingsItemBase*> children_;

	/// Is this item editable?
	Qt::ItemFlag editable_;

	/// Is this item expanded in the view?
	bool expanded_;

};

/// ---------------------------------------------------------

class SettingsItemRoot : public SettingsItemBase {

public:

	/// Default Ctor
	SettingsItemRoot();

	/// Dtor
	virtual ~SettingsItemRoot();

	/// Clone this item, which is basically equivalent to calling the copy ctor
	SettingsItemRoot* clone() const;

private:

	/// Clone this item, which is basically equivalent to calling the copy ctor
	SettingsItemRoot(const SettingsItemRoot&);

};

/// ---------------------------------------------------------

class SettingsItemGroup : public SettingsItemBase {

public:

	/// Ctor
	SettingsItemGroup(const QVariant& name);

	/// Dtor
	virtual ~SettingsItemGroup();

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
	SettingsItemBounds(const QVariant& name,double min, double max, const QVariant& unit, const QVariant& tooltip);

	/// Dtor
	virtual ~SettingsItemBounds();

	/// Clone this item, which is basically equivalent to calling the copy ctor
	virtual SettingsItemBounds* clone() const;

	/// Returns the font this item should be visualized
	/// with in the item tree
	virtual QFont getFont() const;

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
	SettingsItem(const QVariant&, const QVariant&, const QVariant&, const QVariant&);

	/// Dtor
	virtual ~SettingsItem();

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
	SettingsItemInt(const QVariant&, const QVariant&, const QVariant&, const QVariant&);

	/// Dtor
	virtual ~SettingsItemInt();

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
	SettingsItemComboBox(const QVariant&, const QVariant&, const QList<QString>&, const QVariant&);

	/// Dtor
	virtual ~SettingsItemComboBox();

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

	// Returns the label of the active (selected) item
	virtual QString getActiveLabel() const;

	/// Assignment operator
	virtual const SettingsItemComboBox& operator=(const SettingsItemComboBox& rhs);

private:

	/// Copy Ctor
	SettingsItemComboBox(const SettingsItemComboBox& rhs);

	/// Options shown in this combo box
	QList<QString> opts_;

	/// Store the index of the choice selected by the
	/// user when editing the combo-box editor
	size_t activeIndex_;
};

#endif
