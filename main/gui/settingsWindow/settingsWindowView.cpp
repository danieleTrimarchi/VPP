#include "SettingsWindowView.h"
#include <QtCore/QAbstractItemModel>
#include <iostream>

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

/// Override expand signal from the base class
void SettingsWindowView::doExpand(const QModelIndex &index){
	QTreeView::expand(index);
}

/// Override collapse signal from the base class
void SettingsWindowView::doCollapse(const QModelIndex &index) {
	QTreeView::collapse(index);
}


