
#include "VppTableModel.h"

VppTableModel::VppTableModel(ResultContainer* pResults, QObject* parent)
:QAbstractTableModel(parent),
 pResultContainer_(pResults) {

}

int VppTableModel::rowCount(const QModelIndex & /*parent*/) const {

	if(pResultContainer_)
		return pResultContainer_->size();

	else
		return 1;

}

int VppTableModel::columnCount(const QModelIndex & /*parent*/) const {

	if(pResultContainer_)
		return pResultContainer_->get(0).getTableCols(); // iTWV  TWV  iTWa  TWA -- V  PHI  B  F -- dF dM
	else
		return 1;
}

// todo dtrimarchi : how to set col/row titles?? actually we only have
// one header (1, 2, ..) for cols
QVariant VppTableModel::data(const QModelIndex &index, int role) const {

	if (role == Qt::DisplayRole) {

		if(pResultContainer_) {

			// Get the result this index refers to
			const Result& res= pResultContainer_->get(index.row());

			return QString("%1").arg(
					res.getTableEntry(index.column())
					);

		} else
			return QString("No data loaded");
	}

	return QVariant();
}

// Set the header for this table
QVariant VppTableModel::headerData(
		int section, Qt::Orientation orientation,int role) const {

	if(role == Qt::DisplayRole && pResultContainer_){
        
		if(orientation == Qt::Orientation::Horizontal){
			return pResultContainer_->get(0).getColumnHeader(section);
		}

		// Nothing for row headers by now
	}

	// Default, do nothing
	return QVariant();

}


void VppTableModel::accept(ResultContainer* pResultContainer) {

	// Set the results container
	pResultContainer_ = pResultContainer;

}

