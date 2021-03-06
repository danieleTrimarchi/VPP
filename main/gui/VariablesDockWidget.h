#ifndef VARIABLES_DOCK_WIDGET_H
#define VARIABLES_DOCK_WIDGET_H

#include <QtWidgets/QDockWidget>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>

#include "VariableTreeModel.h"

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QActionGroup)
QT_FORWARD_DECLARE_CLASS(QMenu)

class VariableFileParser;

class VariablesDockWidget : public QDockWidget {

    Q_OBJECT

public:

	/// Explicit Constructor
	explicit VariablesDockWidget(QMainWindow *parent = Q_NULLPTR, Qt::WindowFlags flags = 0);

	/// virtual Dtor
	virtual ~VariablesDockWidget();

	/// Populate the item tree with the variables read by the variableFileParser
	void populate(VariableFileParser*);

	/// Returns the underlying item model
	VariableTreeModel* getTreeModel();

	/// Returns the view of the item model
	QTreeView* getView();

	/// Get the toggle view action to be added to the menubar
	QAction* getMenuToggleViewAction();

private:

	/// The VariableDockWidget contains a tree model with the variables
	std::shared_ptr<VariableTreeModel> pTreeModel_;
	std::shared_ptr<QTreeView> pTreeView_;

};


#endif
