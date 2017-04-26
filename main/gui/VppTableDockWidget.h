#ifndef VPPTABLE_DOCK_WIDGET_H
#define VPPTABLE_DOCK_WIDGET_H

#include <QTreeView>
#include <QMainWindow>
#include <QMenu>
#include "boost/shared_ptr.hpp"

#include "VppTabDockWidget.h"
#include "VppTableModel.h"
#include "VPPSolverBase.h"

class VppTableDockWidget : public VppTabDockWidget {

    Q_OBJECT

public:

	/// Explicit Constructor
	explicit VppTableDockWidget(VPPSolverBase* pSolver, QMainWindow *parent = Q_NULLPTR, Qt::WindowFlags flags = 0);

	/// virtual Dtor
	virtual ~VppTableDockWidget();

	/// Get the toggle view action to be added to the menubar
	QAction* getMenuToggleViewAction();

	/// Returns the underlying table model
	VppTableModel* getTableModel();

private:

	/// The VariableDockWidget contains a tree model with the variables
	boost::shared_ptr<VppTableModel> pTableModel_;
	boost::shared_ptr<QTableView> pTableView_;

	/// Ptr to the solver that stores the results
	VPPSolverBase* pSolver_;

};


#endif
