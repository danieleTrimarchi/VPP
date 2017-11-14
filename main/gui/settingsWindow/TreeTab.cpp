#include "TreeTab.h"
#include <QtWidgets/QVBoxLayout>
#include <iostream>

//---------------------------------------------------------------
// This widget includes several setting trees, from which the
// individual settings can be expanded and visualized. To be
// inserted in the VPPSettingsDialog
TreeTab::TreeTab(QWidget* parent):
QWidget(parent) {

	//SettingsModel hullSettings(this);
	pTreeView_= new SettingsWindowView(this);
	pTreeModel_= new SettingsModel(pTreeView_,this);

	// Instantiate a model view, assign a model and show it
	pTreeView_->setModel(pTreeModel_);
	pTreeView_->setWindowTitle(QObject::tr("Dialog Tree Model"));

	// Adding a delegate...
	pDelegate_ = new QLineEditDelegate(this);
	pTreeView_->setItemDelegate(pDelegate_);

	// Expand all the items.
	//pTreeView_->expandAll();

	// Assign the tree view to a VBoxLayout
	QVBoxLayout* centralLayout = new QVBoxLayout(this);

	centralLayout->setMargin(5);

	centralLayout->addWidget(pTreeView_);

	this->setLayout(centralLayout);

	// connect(exitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
	connect(pTreeView_->selectionModel(), &QItemSelectionModel::selectionChanged,
			this, &TreeTab::updateActions);

	// Resize the window in order to show as much as possible
	resize(500,500);

	updateActions();

}

// Dtor
TreeTab::~TreeTab() {
	delete pTreeModel_;
	delete pTreeView_;
}

void TreeTab::updateActions() {

	bool hasCurrent = pTreeView_->selectionModel()->currentIndex().isValid();

	if (hasCurrent)
		pTreeView_->closePersistentEditor(pTreeView_->selectionModel()->currentIndex());

	// Make sure the columns are wide enough for the text that must be contained
	for (int iColumn = 0; iColumn < pTreeModel_->columnCount(); ++iColumn)
		pTreeView_->resizeColumnToContents(iColumn);

}
