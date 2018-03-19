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

		/// Remove the children of this model
		void clearChildren();

		/// How many cols?
		int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

		/// Called by Qt, this method returns the data to visualize or some
		/// display options
		QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

		/// Called by Qt, returns specific flags
		Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

		/// Called by Qt, returns the header
		QVariant headerData(
				int section, Qt::Orientation orientation,
				int role = Qt::DisplayRole
				) const Q_DECL_OVERRIDE;

		/// Returns an index given row and col
		QModelIndex index(
				int row, int column,
				const QModelIndex &parent = QModelIndex()
				) const Q_DECL_OVERRIDE;

		/// Get my parent
		QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;

		/// How many rows?
		int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

	private:

		/// Actually generate the item tree
		void setupModelData();
};

#endif // TREEMODEL_H
