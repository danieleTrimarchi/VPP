/****************************************************************************
 **
 ** Copyright (C) 2016 The Qt Company Ltd.
 ** Contact: https://www.qt.io/licensing/
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and The Qt Company. For licensing terms
 ** and conditions see https://www.qt.io/terms-conditions. For further
 ** information use the contact form at https://www.qt.io/contact-us.
 **
 ** BSD License Usage
 ** Alternatively, you may use this file under the terms of the BSD license
 ** as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of The Qt Company Ltd nor the names of its
 **     contributors may be used to endorse or promote products derived
 **     from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

/*
    treemodel.cpp

    Provides a simple tree model to show how to create and use hierarchical
    models.
 */

#include "VariableTreeModel.h"

#include <QStringList>
#include "VariableTreeItem.h"

VariableTreeModel::VariableTreeModel(QObject *parent)
: QAbstractItemModel(parent)
{
	QList<QVariant> rootData;
	rootData <<  "Name" << "Value";
	rootItem_ = new VariableTreeItem(rootData);
	setupModelData();

}

VariableTreeModel::~VariableTreeModel()
{
	delete rootItem_;
}

int VariableTreeModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return static_cast<VariableTreeItemBase*>(parent.internalPointer())->columnCount();
	else
		return rootItem_->columnCount();
}

QVariant VariableTreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()){
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		VariableTreeItemBase *item = static_cast<VariableTreeItemBase*>(index.internalPointer());
		return item->data(index.column());
	}
	else if (role == Qt::DecorationRole ) {
		VariableTreeItemBase *item = static_cast<VariableTreeItemBase*>(index.internalPointer());
		if(index.column()==0)
			return item->getIcon();
		else
			return QVariant();
	}

	return QVariant();

}

Qt::ItemFlags VariableTreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	if(index.column()==0)
		return QAbstractItemModel::flags(index);

	return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QVariant VariableTreeModel::headerData(int section, Qt::Orientation orientation,
		int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem_->data(section);

	return QVariant();
}

QModelIndex VariableTreeModel::index(int row, int column, const QModelIndex &parent)
const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	VariableTreeItemBase *parentItem;

	if (!parent.isValid())
		parentItem = rootItem_;
	else
		parentItem = static_cast<VariableTreeItemBase*>(parent.internalPointer());

	VariableTreeItemBase *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex VariableTreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	VariableTreeItemBase *childItem = static_cast<VariableTreeItemBase*>(index.internalPointer());
	VariableTreeItemBase *parentItem = childItem->parentItem();

	if (parentItem == rootItem_)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int VariableTreeModel::rowCount(const QModelIndex &parent) const
{
	VariableTreeItemBase *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = rootItem_;
	else
		parentItem = static_cast<VariableTreeItemBase*>(parent.internalPointer());

	return parentItem->childCount();
}

void VariableTreeModel::setupModelData() {

	// Instantiate an item list and assign the root as
	// the first item of the list
	QList<VariableTreeItemBase*> parents;
	parents << rootItem_;

	// Append variables to the tree
	QList<QVariant> columnData;
	columnData << "Variables"<<" ";

	// Place the fake root under the real root
	parents.last()->appendChild(new VariableTreeFakeRoot(columnData, parents.last()));

	columnData.clear();
	columnData << "A" << "8.57";

	parents.last()->child(0)->appendChild(new VariableTreeItem(columnData, parents.last()->child(0)));

	columnData.clear();
	columnData << "B" << "3.43";
	parents.last()->child(0)->appendChild(new VariableTreeItem(columnData, parents.last()->child(0)));

	columnData.clear();
	columnData << "T" << "0.43";
	parents.last()->child(0)->appendChild(new VariableTreeItem(columnData, parents.last()->child(0)));


}
