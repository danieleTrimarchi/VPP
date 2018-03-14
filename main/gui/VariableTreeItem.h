#ifndef TREEITEM_H
#define TREEITEM_H

//#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include "Item.h"

class VariableTreeItemBase : public Item {

	public:

		/// Ctor
		explicit VariableTreeItemBase(const QList<QVariant> &data, Item *parentItem = 0);

		/// Dtor
		virtual ~VariableTreeItemBase();

	protected:

};

///////////////////////////////////////////////////////////////////////

class VariableTreeFakeRoot : public VariableTreeItemBase {

	public:
		explicit VariableTreeFakeRoot(const QList<QVariant> &data, Item *parentItem = 0);
		~VariableTreeFakeRoot();

		/// Returns the icon associated with this variable tree item
		virtual QVariant getIcon();

	private:

};

///////////////////////////////////////////////////////////////////////

class VariableTreeItem : public VariableTreeItemBase {

	public:
		explicit VariableTreeItem(const QList<QVariant> &data, Item *parentItem = 0);
		~VariableTreeItem();

		/// Returns the icon associated with this variable tree item
		virtual QVariant getIcon();

	private:
};


#endif // TREEITEM_H
