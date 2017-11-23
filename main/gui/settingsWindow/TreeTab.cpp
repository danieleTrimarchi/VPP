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
	pTreeReferenceModel_ = new SettingsModel;

	// Fill the reference model with the items
	pTreeReferenceModel_->setupModelData();
	pTreeReferenceModel_->setParent(this);

    // Instantiate the model that will be visualized in the
	// tab as a clone of the reference model
	pTreeModel_= new SettingsModel(*pTreeReferenceModel_);

	// Instantiate a view that will be used to visualize the model
	pTreeView_= new SettingsWindowView(pTreeModel_,this);

	connect(
			pTreeView_,&SettingsWindowView::expanded,
			pTreeView_,&SettingsWindowView::resizeColumnsToContents
			);

	connect(
			pTreeModel_,&SettingsModel::dataChanged,
			pTreeView_,&SettingsWindowView::resizeColumnsToContents
			);

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

// Save the model the user has edited to the underlying (permanent) model
void TreeTab::save() {
	// Dereference the ptr to make sure we are actually calling
	// the operator= of SettingsModel
	*pTreeReferenceModel_ = *pTreeModel_;
}

// When the user hits 'cancel' in the main dialog, we
// revert the model: so that any change the user has
// done is erased.
void TreeTab::revert() {
	// Dereference the ptr to make sure we are actually calling
	// the operator= of SettingsModel
	std::cout<<"Reverting...\n";
	*pTreeModel_ = *pTreeReferenceModel_;
}



void TreeTab::updateActions() {

	bool hasCurrent = pTreeView_->selectionModel()->currentIndex().isValid();

	if (hasCurrent)
		pTreeView_->closePersistentEditor(pTreeView_->selectionModel()->currentIndex());

	// Make sure the columns are wide enough for the text that must be contained
	for (int iColumn = 0; iColumn < pTreeModel_->columnCount(); ++iColumn)
		pTreeView_->resizeColumnToContents(iColumn);

}
