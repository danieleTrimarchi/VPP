#include "VppTableDockWidget.h"
#include "VppTableModel.h"

VppTableDockWidget::VppTableDockWidget( QMainWindow* parent/*=Q_NULLPTR*/, Qt::WindowFlags flags/*=0*/) :
	VppTabDockWidget(parent, flags) {

    setObjectName("Results");
    setWindowTitle("Results");

    // Build the tree model and the TreeView this dock widget contains
	pTreeModel_.reset( new VppTableModel(this) );
	pTreeView_.reset( new QTreeView );
	pTreeView_->setModel(pTreeModel_.get());
	pTreeView_->setWindowTitle(QObject::tr("Vpp Results"));
	pTreeView_->show();

	// Set the treeView as what is shown by this widget
	setWidget(pTreeView_.get());

}

// virtual Dtor
VppTableDockWidget::~VppTableDockWidget() {

}

// Get the toggle view action to be added to the menubar
QAction* VppTableDockWidget::getMenuToggleViewAction() {

	// This menu entry drives the visibility of the
	// dockWidget
	QAction* pToggleViewAction = toggleViewAction();
	pToggleViewAction->setShortcut(QKeySequence::Paste);
	pToggleViewAction->setStatusTip("Opens the tabular result widget");

	return pToggleViewAction;

}



