#ifndef ITEM_H
#define ITEM_H

#include <QtCore/QList>
#include "DataColumn.h"

/// Forward declarations
class GetItemByPathVisitor;
class GetItemByNameVisitor;

/// Base class for all tree items that will populate
/// the Qt model trees. Items are composite, and this
/// means that they store a ptr to their parent items
class Item {

	public :

		/// Default Ctor
		Item(Item* pParent=0);

		/// Copy Ctor
		Item(const Item&);

		// Dtor
		virtual ~Item();

		/// Accept a visitor, the role of which is to iterate and
		/// return an item by path
		Item* accept(const GetItemByPathVisitor&,QString);

		/// Accept a visitor, the role of which is to iterate and
		/// return an item by name
		Item* accept(const GetItemByNameVisitor&, QString);

		/// Append a child under me
		void appendChild(Item *child);

		/// Get a ptr to my i-th child
		Item* child(int row);

		/// Get a child by name
		Item* child(QString& childName);

		/// How many children do I have
		int childCount() const;

		/// What child number am I?
		int childNumber() const;

		/// Remove all children under me
		void clearChildren();

		/// Clone this item, which is basically equivalent to
		/// calling the copy ctor
		virtual Item* clone() const;

		/// How many columns does each child have
		int columnCount() const;

		/// Return the data stored in the i-th column of
		/// this item. In this case, either the label
		/// or the numerical value
		QVariant data(int iColumn) const;

		/// Is this item editable?
		Qt::ItemFlag editable() const;

		/// Return the flag 'expanded'
		bool expanded() const;

		/// Return the backGround color for this item
		/// The default value is white
		virtual QColor getBackGroundColor(int iColumn) const;

		/// Get a child by path - inclusive of the child name, of course
		Item* getChildbyPath(QString& childPath);

		/// Returns a reference to the list of children
		/// for this item
		const QList<Item*>& getChildren() const;

		/// Get a reference to the columns vector
		std::vector<DataColumn*>& getColumnVector();

		/// Get a reference to the i-th column in the vector
		DataColumn* getColumn(const int);

		/// Get the display name of this item
		QString getDisplayName() const;

		/// Returns the font this item should be visualized
		/// with in the item tree
		virtual QFont getFont() const;

		/// Returns the associated icon as a QVariant
		virtual QVariant getIcon();

		/// Get the internal name of this item, used to locate it in the tree
		virtual QString getInternalName() const;

		/// Returns the tooltip for this item, if any
		virtual QVariant getToolTip();

		/// Assignment operator
		virtual const Item& operator=(const Item& rhs);

		/// Comparison operator
		virtual bool operator==(const Item& rhs);

		/// Inverse comparison operator
		virtual bool operator!=(const Item& rhs);

		/// Get a handle to my parent
		Item* parentItem();

		/// Remove a child by position
		void removeChild(size_t iChild);

		/// What child number am I for my parent?
		int row() const;

		/// Set the data for the i-th column of this item
		bool setData(int column, const QVariant &value);

		/// Set if this item is editable
		void setEditable(bool);

		/// Set the parent of this item
		void setParent(Item* parentItem);

		/// Recursively set the parent of this item and of all its children
		void setParentRecursive(Item* parentItem);

	public slots:

		/// This slot is triggered when the item is expanded in the view
		void setExpanded(bool expanded);

	protected :

		/// Parent of this (composite) item
		Item* pParent_;

		/// List of children
		QList<Item*> children_;

		/// Data columns the Item must be able to fill
		/// The model is build on top of these columns
		std::vector<DataColumn*> columns_;

		/// Is this item editable?
		Qt::ItemFlag editable_;

		/// Is this item expanded in the view?
		bool expanded_;

	private :


};


#endif
