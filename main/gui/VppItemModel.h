#ifndef VPP_ITEM_MODEL_H
#define VPP_ITEM_MODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class VariableTreeItemBase;

/// This class describes the item model for the variable tree
/// The class directly derives from the Qt example
class VppItemModel : public QAbstractItemModel {

		Q_OBJECT

	public:

		/// Ctor
		explicit VppItemModel(QObject *parent = 0);

		/// Virtual Dtor
		virtual ~VppItemModel();

	private:

};

#endif // TREEMODEL_H
