#ifndef VPPTABLEMODEL_H
#define VPPTABLEMODEL_H

#include <QAbstractTableModel>
#include "Results.h"

using namespace Results;

/* Class defining the model underlying table view.
 * This is mainly used to visualize results
 */
class VppTableModel : public QAbstractTableModel {

		Q_OBJECT

	public:

		/// Ctor
		VppTableModel(ResultContainer* results, QObject *parent);

		/// Accept a container for results to plot
		void accept(ResultContainer*);

		/// Number of cols of this table
		int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

		/// Data contained in the table
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

		QVariant headerData(int section, Qt::Orientation orientation,
		                                int role = Qt::DisplayRole) const override;

		/// Number of rows of this table
		int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;

	private:

		/// Result container containing the results to visualize
		ResultContainer* pResultContainer_;

};

#endif
