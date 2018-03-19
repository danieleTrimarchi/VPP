#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QModelIndex>
#include <QVariant>
#include "VppItemModel.h"

class VariableTreeItemBase;

/// This class describes the item model for the variable tree
/// The class directly derives from the Qt example
class VariableTreeModel : public VppItemModel {

		Q_OBJECT

	public:

		/// Ctor
		explicit VariableTreeModel(QObject *parent = 0);

		/// Virtual Dtor
		virtual ~VariableTreeModel();

		/// Append a variable item to the tree
		void append( QList<QVariant>& columnData );

	private:

		/// Actually generate the item tree
		void setupModelData();
};

#endif // TREEMODEL_H
