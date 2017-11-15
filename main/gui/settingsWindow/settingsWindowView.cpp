#include "SettingsWindowView.h"
#include <QtCore/QAbstractItemModel>

// Ctor
SettingsWindowView::SettingsWindowView(SettingsModel* pModel, QWidget *parent) :
	pModel_(pModel),
	QTreeView(parent){

	setStyleSheet(	"background-color: rgb(228,228,228);"
					"selection-color: white;");

	setWindowModality(Qt::ApplicationModal);

	setFrameStyle(QFrame::NoFrame);
	setLineWidth(10);

}


// Dtor
SettingsWindowView::~SettingsWindowView() {
	/* make nothing */
}

void SettingsWindowView::resizeColumnsToContents(const QModelIndex& index) {

	// Take a chance to resize all columns
	for(size_t iCol=0; iCol<pModel_->columnCount(); iCol++ )
		resizeColumnToContents(iCol);
}


