#ifndef VPPTABLE_DOCK_WIDGET_H
#define VPPTABLE_DOCK_WIDGET_H

#include <QTreeView>
#include <QMainWindow>
#include <QMenu>
#include "boost/shared_ptr.hpp"

#include "VppTabDockWidget.h"
#include "VppTableModel.h"

//QT_FORWARD_DECLARE_CLASS(QAction)
//QT_FORWARD_DECLARE_CLASS(QActionGroup)
//QT_FORWARD_DECLARE_CLASS(QMenu)

class VppTableDockWidget : public VppTabDockWidget {

    Q_OBJECT

public:

	/// Explicit Constructor
	explicit VppTableDockWidget(QMainWindow *parent = Q_NULLPTR, Qt::WindowFlags flags = 0);

	/// virtual Dtor
	virtual ~VppTableDockWidget();

	/// Get the toggle view action to be added to the menubar
	QAction* getMenuToggleViewAction();

private:

	/// The VariableDockWidget contains a tree model with the variables
	boost::shared_ptr<VppTableModel> pTreeModel_;
	boost::shared_ptr<QTreeView> pTreeView_;

};


#endif
