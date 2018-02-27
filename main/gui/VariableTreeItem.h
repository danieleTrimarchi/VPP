#ifndef TREEITEM_H
#define TREEITEM_H

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtGui/QIcon>

class VariableTreeItemBase {

	public:

		/// Ctor
		explicit VariableTreeItemBase(const QList<QVariant> &data, VariableTreeItemBase *parentItem = 0);

		/// Dtor
		virtual ~VariableTreeItemBase();

		/// Append a child under me
		void appendChild(VariableTreeItemBase *child);

		/// Delete all my children
		void deleteChildren();

		/// Get a ptr to my i-th child
		VariableTreeItemBase* child(int row);

		/// How many children do I have
		int childCount() const;

		/// How many columns does each child have
		int columnCount() const;

		/// Get me the underlying data
		QVariant data(int column) const;

		/// Returns the icon associated with this variable tree item
		virtual QIcon getIcon() =0;

		/// What child number am I for my parent?
		int row() const;

		/// Get a handle to my parent
		VariableTreeItemBase* parentItem();

	protected:

		QList<VariableTreeItemBase*> children_;
		QList<QVariant> data_;
		VariableTreeItemBase* pParent_;

};

///////////////////////////////////////////////////////////////////////

class VariableTreeFakeRoot : public VariableTreeItemBase {

	public:
		explicit VariableTreeFakeRoot(const QList<QVariant> &data, VariableTreeItemBase *parentItem = 0);
		~VariableTreeFakeRoot();

		/// Returns the icon associated with this variable tree item
		virtual QIcon getIcon();

	private:

};

///////////////////////////////////////////////////////////////////////

class VariableTreeItem : public VariableTreeItemBase {

	public:
		explicit VariableTreeItem(const QList<QVariant> &data, VariableTreeItemBase *parentItem = 0);
		~VariableTreeItem();

		/// Returns the icon associated with this variable tree item
		virtual QIcon getIcon();

	private:
};


#endif // TREEITEM_H
