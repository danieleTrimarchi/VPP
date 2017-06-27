#include "VppTableDockWidget.h"
#include "VppTableModel.h"

VppTableDockWidget::VppTableDockWidget(VPPSolverBase* pSolver, QMainWindow* parent/*=Q_NULLPTR*/, Qt::WindowFlags flags/*=0*/) :
	VppTabDockWidget(parent, flags),
	pSolver_(pSolver) {

    setObjectName("Result table");
    setWindowTitle("Result table");

    // Build the tree model and the TreeView this dock widget contains
	pTableModel_.reset( new VppTableModel(pSolver_->getResults(), this) );
	pTableView_.reset( new QTableView );
	pTableView_->setModel(pTableModel_.get());
	pTableView_->setWindowTitle(QObject::tr("Vpp Results"));

	// Set the treeView as what is shown by this widget
	setWidget(pTableView_.get());

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

// Returns the underlying table model
VppTableModel* VppTableDockWidget::getTableModel() {
	return pTableModel_.get();
}


