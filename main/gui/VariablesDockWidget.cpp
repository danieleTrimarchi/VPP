#include "VariablesDockWidget.h"
#include <QtWidgets/QHeaderView>
#include "VariableFileParser.h"

VariablesDockWidget::VariablesDockWidget( QMainWindow* parent/*=Q_NULLPTR*/, Qt::WindowFlags flags/*=0*/) :
	QDockWidget(parent, flags) {

    setObjectName("Variables");
    setWindowTitle("Variables");

    // Build the tree model and the TreeView this dock widget contains
	pTreeModel_.reset( new VariableTreeModel );
	pTreeView_.reset( new QTreeView );

	pTreeView_->expandAll();

	pTreeView_->setModel(pTreeModel_.get());
	pTreeView_->setWindowTitle(QObject::tr("Simple Tree Model"));
	// Add this to make sure the string contained in the tree are visible
	pTreeView_->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	pTreeView_->show();

	// Set the treeView as what is shown by this widget
	setWidget(pTreeView_.get());

}

// virtual Dtor
VariablesDockWidget::~VariablesDockWidget() {

}

// Returns the underlying item model
VariableTreeModel* VariablesDockWidget::getTreeModel() {
	return pTreeModel_.get();
}

// Returns the view of the item model
QTreeView* VariablesDockWidget::getView() {
	return pTreeView_.get();
}

// Get the toggle view action to be added to the menubar
QAction* VariablesDockWidget::getMenuToggleViewAction() {

	// This menu entry drives the visibility of the
	// dockWidget
	QAction* pToggleViewAction = toggleViewAction();
	pToggleViewAction->setShortcut(QKeySequence::Paste);
	pToggleViewAction->setStatusTip("Opens the Variables widget");

	return pToggleViewAction;

}


