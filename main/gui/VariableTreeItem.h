#ifndef TREEITEM_H
#define TREEITEM_H

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtGui/QIcon>

class VariableTreeItemBase
{
public:
    explicit VariableTreeItemBase(const QList<QVariant> &data, VariableTreeItemBase *parentItem = 0);

    virtual ~VariableTreeItemBase();

    void appendChild(VariableTreeItemBase *child);

    VariableTreeItemBase *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;

    /// Returns the icon associated with this variable tree item
    virtual QIcon getIcon() =0;

    int row() const;
    VariableTreeItemBase *parentItem();

protected:
    QList<VariableTreeItemBase*> m_childItems;
    QList<QVariant> m_itemData;
    VariableTreeItemBase *m_parentItem;

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
