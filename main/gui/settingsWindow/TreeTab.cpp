#include "TreeTab.h"
#include <QtWidgets/QVBoxLayout>
#include <iostream>
#include "VppSettingsXmlWriter.h"
#include <QtCore/QFile>
#include <QtWidgets/QMessageBox>
#include "../MainWindow.h"

#include "GetItemVisitor.h"
#include "VppSettingsXmlReader.h"
#include "VppTags.h"

//---------------------------------------------------------------
// This widget includes several setting trees, from which the
// individual settings can be expanded and visualized. To be
// inserted in the VPPSettingsDialog
TreeTab::TreeTab(QWidget* myparent):
QWidget(myparent) {

	//SettingsModel hullSettings(this);
	pTreeReferenceModel_ = new SettingsModel;

	// Fill the reference model with the items
	pTreeReferenceModel_->setupModelData();
	pTreeReferenceModel_->setParent(this);

	// Instantiate the model that will be visualized in the
	// tab as a clone of the reference model
	pTreeModel_= new SettingsModel;

	// Now populate the model with some items
	pTreeModel_->setupModelData();
	pTreeModel_->setParent(this);

	// Instantiate a view that will be used to visualize the model
	pTreeView_= new SettingsWindowView(pTreeModel_,this);

	// Instantiate a model view, assign a model and show it
	pTreeView_->setModel(pTreeModel_);
	pTreeView_->setWindowTitle(QObject::tr("Dialog Tree Model"));

	// Adding a delegate...
	pDelegate_ = new VPPSettingsDelegate(this);
	pTreeView_->setItemDelegate(pDelegate_);

	// Set mouse tracking on for this widget. This will allow the delegate
	// to detect when the mouse hovers over the various objects of the widget
	pTreeView_->setMouseTracking(true);

	// Select on single click
	pTreeView_->setEditTriggers(QAbstractItemView::AllEditTriggers);

	// Assign the tree view to a VBoxLayout
	QVBoxLayout* centralLayout = new QVBoxLayout(this);

	centralLayout->setMargin(5);

	centralLayout->addWidget(pTreeView_);

	this->setLayout(centralLayout);

	// Connect all the required signals
	connectSignals();

	// Resize the window in order to show as much as possible
	resize(500,500);

	updateActions();

}

void TreeTab::setParent(QWidget* parent ) {
	QWidget::setParent(parent);
}

void TreeTab::setParent(QWidget* parent, QFlags<enum Qt::WindowType> flags) {
	QWidget::setParent(parent,flags);
}

// Dtor
TreeTab::~TreeTab() {
	delete pTreeModel_;
	delete pTreeView_;
}

// Save the model the user has edited to the underlying (permanent) model
void TreeTab::save() {

	// Save does something only if the current model has been
	// edited. Otherwise do nothing and preserve the current state
	if( *pTreeModel_ != *pTreeReferenceModel_ ){

		// Dereference the ptr to make sure we are actually calling
		// the operator= of SettingsModel
		*pTreeReferenceModel_ = *pTreeModel_;

		// Make sure the view is up to date
		updateActions();

		// Also update the external dependencies (in this case: the VariableTreeWidget)
		updateDependencies();
	}
}

// Save the settings to file
void TreeTab::save(VppSettingsXmlWriter* pWriter) {

	// Write the settings Tree
	VPPSettingsXmlWriterVisitor v(pWriter);
	pTreeReferenceModel_->getRoot()->accept(v);

}

// Read the settings to file

void TreeTab::read(VppSettingsXmlReader* pReader){

	// Instantiate a file reader visitor. This asks
	// the reader to read the xml file and construct
	// a settingsTree. Then, this settingsTree will
	// be assigned to the reference model tree root
	VPPSettingsXmlReaderVisitor v(pReader);

	// Assign the tree model instantiated by the reader
	// to the reference tree root
	if(pTreeReferenceModel_->getRoot())
		pTreeReferenceModel_->getRoot()->accept(v);
	else
		throw VPPException(HERE,"Reference model root not found");

	// Copy the reference model to the current one
	revert();
}

// When the user hits 'cancel' in the main dialog, we
// revert the model: so that any change the user has
// done is erased.
void TreeTab::revert() {
    
	if( *pTreeModel_ != *pTreeReferenceModel_ ){

		// Do something only if the model is different to the
		// reference model. Dereference the ptr to make sure
		// we are actually calling the operator= of SettingsModel
		*pTreeModel_ = *pTreeReferenceModel_;

		updateActions();

		// Also update the external dependencies (in this case: the VariableTreeWidget)
		updateDependencies();
	}
}

// Get the reference model. This is where the settings
// are stored
const SettingsModel* TreeTab::getReferenceSettingsModel() const {
	return pTreeReferenceModel_;
}

void TreeTab::updateActions() {

	bool hasCurrent = pTreeView_->selectionModel()->currentIndex().isValid();

	if (hasCurrent)
		pTreeView_->closePersistentEditor(pTreeView_->selectionModel()->currentIndex());

	// Make sure the columns are wide enough for the text that must be contained
	for (int iColumn = 0; iColumn < pTreeModel_->columnCount(); ++iColumn)
		pTreeView_->resizeColumnToContents(iColumn);
}

// Update external dependencies, such as the VariableTreeModel
void TreeTab::updateDependencies() {

	// Get a ptr to the MainWindow. Connect any change of the tree model
	// to an update of the variable item tree widget. This is a TreeTab,
	// the parent of which is a VPPSettingsDialog. Parent of the
	// VPPSettingsDialog is the MainWindow.
	QObject* myAncestor=parent();
	while(!qobject_cast<MainWindow*>(myAncestor)){
		myAncestor = myAncestor->parent();
		if(!myAncestor)
			throw VPPException(HERE, "A ptr to MainWindow could not be found!");
	}

	MainWindow* pMainWindow = qobject_cast<MainWindow*>(myAncestor);
	pMainWindow->udpateVariableTree();

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

