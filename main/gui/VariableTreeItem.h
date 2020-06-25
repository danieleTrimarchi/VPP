#ifndef TREEITEM_H
#define TREEITEM_H

//#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include "Item.h"

class VariableTreeItemBase : public Item {

	public:

		/// Ctor
		explicit VariableTreeItemBase(Item *parentItem = 0);

		/// Dtor
		virtual ~VariableTreeItemBase();

	protected:

};

///////////////////////////////////////////////////////////////////////
/// Root class for the variable items living under the variable
/// tree model in the UI
class VariableTreeRoot : public VariableTreeItemBase {

	public:

		/// Ctor
		explicit VariableTreeRoot(Item *parentItem = 0);

		/// Dtor
		~VariableTreeRoot();

		/// Returns the icon associated with this variable tree item
		virtual QVariant getIcon(size_t row=0);

	private:

};

///////////////////////////////////////////////////////////////////////

/// Item living under the variable tree. It visualizes the variableName,
/// the variable value and some icons
class VariableTreeItem : public VariableTreeItemBase {

	public:

		/// Ctor
		explicit VariableTreeItem(const QVariant& variableName, const QVariant& value, Item *parentItem = 0);

		/// Dtor
		~VariableTreeItem();

		/// Returns the icon associated with this variable tree item
		virtual QVariant getIcon(size_t row=0);

	private:
};

///////////////////////////////////////////////////////////////////////

/// Item living under the variable tree root. It is the visible placeHolder
/// for the all the variables. It is not excluded in the future there could
/// be several groups. For example, one group for hull-related variables, one
/// for sail related variables...
class VariableTreeItemGroup : public VariableTreeItemBase {

	public:

		/// Ctor
		explicit VariableTreeItemGroup(const QVariant& grooupName, Item *parentItem = 0);

		/// Dtor
		~VariableTreeItemGroup();

		/// Returns the icon associated with this variable tree item
		virtual QVariant getIcon(size_t row=0);

	private:
};

#endif // TREEITEM_H
