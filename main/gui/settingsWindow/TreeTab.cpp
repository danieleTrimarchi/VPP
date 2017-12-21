#include "TreeTab.h"
#include <QtWidgets/QVBoxLayout>
#include <iostream>
#include "GetSettingsItemVisitor.h"
#include "VppSettingsXmlWriter.h"
#include <QtCore/QFile>
#include <QtWidgets/QMessageBox>
#include "VppSettingsXmlReader.h"

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
	pTreeModel_= new SettingsModel; //(*pTreeReferenceModel_);
	pTreeModel_->setParent(this);

	// Instantiate a view that will be used to visualize the model
	pTreeView_= new SettingsWindowView(pTreeModel_,this);

	// Instantiate a model view, assign a model and show it
	pTreeView_->setModel(pTreeModel_);
	pTreeView_->setWindowTitle(QObject::tr("Dialog Tree Model"));

	// Adding a delegate...
	pDelegate_ = new QLineEditDelegate(this);
	pTreeView_->setItemDelegate(pDelegate_);

	// Expand all the items.
	// pTreeView_->expandAll();

	// Assign the tree view to a VBoxLayout
	QVBoxLayout* centralLayout = new QVBoxLayout(this);

	centralLayout->setMargin(5);

	centralLayout->addWidget(pTreeView_);

	this->setLayout(centralLayout);

	// Connect all the required signals
	connectSignals();

	// Now populate the model with some items
	pTreeModel_->setupModelData();

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

	// Make sure the view is up to date
	updateActions();
}

// Save the settings to file
void TreeTab::save(QFile& file) {

	// Get the settings
	VPPSettingsXmlWriterVisitor v(pTreeReferenceModel_,&file);
	pTreeReferenceModel_->getRoot()->accept(v);

}

// Read the settings to file
void TreeTab::read(QFile& file){

	VPPSettingsXmlReaderVisitor v(&file);
	pTreeReferenceModel_->getRoot()->accept(v);
}

// When the user hits 'cancel' in the main dialog, we
// revert the model: so that any change the user has
// done is erased.
void TreeTab::revert() {

	// Dereference the ptr to make sure we are actually calling
	// the operator= of SettingsModel
	*pTreeModel_ = *pTreeReferenceModel_;

	updateActions();
}


void TreeTab::updateActions() {

	bool hasCurrent = pTreeView_->selectionModel()->currentIndex().isValid();

	if (hasCurrent)
		pTreeView_->closePersistentEditor(pTreeView_->selectionModel()->currentIndex());

	// Make sure the columns are wide enough for the text that must be contained
	for (int iColumn = 0; iColumn < pTreeModel_->columnCount(); ++iColumn)
		pTreeView_->resizeColumnToContents(iColumn);

}

// Connect all the required signals
// todo dtrimarchi, this should be moved to
// the view, that owns a ptr to the model
void TreeTab::connectSignals() {

	// Change in selection triggers an update
	connect(pTreeView_->selectionModel(), &QItemSelectionModel::selectionChanged,
			this, &TreeTab::updateActions);

	// Changing data trigger columns resize
	connect(
			pTreeModel_,&SettingsModel::dataChanged,
			pTreeView_,&SettingsWindowView::resizeColumnsToContents
	);

	// Expanding items triggers columns resize
	connect(
			pTreeView_,&SettingsWindowView::expanded,
			pTreeView_,&SettingsWindowView::resizeColumnsToContents
	);

	// --

	// Expanding an item triggers the method that stores the item state
	connect( pTreeView_,&SettingsWindowView::expanded,
			pTreeModel_,&SettingsModel::setItemExpanded);

	// Folding an item triggers the method that stores the item state
	connect( pTreeView_,&SettingsWindowView::collapsed,
			pTreeModel_,&SettingsModel::setItemCollapsed);

	// --

	// An item, the state of which is expanded, must notify the view
	connect( pTreeModel_,&SettingsModel::mustExpand,
				pTreeView_,&SettingsWindowView::doExpand);

	// An item, the state of which is collapsed, must notify the view
	connect( pTreeModel_,&SettingsModel::mustCollapse,
 			pTreeView_,&SettingsWindowView::doCollapse);

}

