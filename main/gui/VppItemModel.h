#ifndef VPP_ITEM_MODEL_H
#define VPP_ITEM_MODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "Item.h"
#include <boost/shared_ptr.hpp>

class VariableTreeItemBase;

/// This class describes the item model for the variable tree
/// The class directly derives from the Qt example
class VppItemModel : public QAbstractItemModel {

		Q_OBJECT

	public:

		/// Ctor
		explicit VppItemModel(QObject *parent = 0);

		/// Copy Ctor
		explicit VppItemModel(const VppItemModel&);

		/// Virtual Dtor
		virtual ~VppItemModel();

		/// Returns a ptr to the root of this model
		virtual Item* getRoot() const;

	protected:

		/// Get an item of this model
		Item* getItem(const QModelIndex &index) const;

		/// Root of the model tree
		boost::shared_ptr<Item> pRootItem_;

	private:

};

#endif // TREEMODEL_H
