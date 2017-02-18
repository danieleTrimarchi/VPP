#ifndef VARIABLES_DOCK_WIDGET_H
#define VARIABLES_DOCK_WIDGET_H

#include <QDockWidget>
#include <QTreeView>
#include <QMainWindow>
#include <QMenu>
#include "boost/shared_ptr.hpp"

#include "VariableTreeModel.h"

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QActionGroup)
QT_FORWARD_DECLARE_CLASS(QMenu)

class VariablesDockWidget : public QDockWidget {

    Q_OBJECT

public:

	/// Explicit Constructor (why explicit..?? This was in the initial Qt example, see
	/// colorswatch in MainWindow example)
	explicit VariablesDockWidget(QMainWindow *parent = Q_NULLPTR, Qt::WindowFlags flags = 0);

	/// virtual Dtor
	virtual ~VariablesDockWidget();

	/// Get the toggle view action to be added to the menubar
	QAction* getMenuToggleViewAction();

private:

	/// The VariableDockWidget contains a tree model with the variables
	boost::shared_ptr<VariableTreeModel> pTreeModel_;
	boost::shared_ptr<QTreeView> pTreeView_;

};


#endif
