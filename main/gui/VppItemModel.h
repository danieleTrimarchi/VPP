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

		/// Remove the children of this model
		void clearChildren();

		/// How many cols?
		virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

		/// Called by Qt, this method returns the data to visualize or some
		/// display options
		virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

		/// Called by Qt, returns specific flags
		virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

		/// Returns a ptr to the root of this model
		virtual Item* getRoot() const;

		/// Called by Qt, returns the header
		virtual QVariant headerData(int section,
															Qt::Orientation orientation,
															int role = Qt::DisplayRole
															) const Q_DECL_OVERRIDE;

		/// Returns an index given row and column
		virtual QModelIndex index(int row, int column,
														const QModelIndex& parent= QModelIndex()
														) const Q_DECL_OVERRIDE;

		/// Get my parent
		QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;

		/// How many rows?
		virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

	protected:

		/// Get an item of this model
		Item* getItem(const QModelIndex &index) const;

		/// Root of the model tree
		boost::shared_ptr<Item> pRootItem_;

	private:

};

#endif // TREEMODEL_H
