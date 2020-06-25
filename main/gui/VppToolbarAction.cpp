#include "VppToolbarAction.h"

#include <string>
using namespace std;

#include "MainWindow.h"


// Ctor
VppToolbarMenu::VppToolbarMenu(	const QString& name,
															const QString &iconFile,
															MainWindow* pParent):
													QMenu(name,pParent) {
	setToolTipsVisible(true);
	setToolTip(name);
	setStatusTip(tr(name.toStdString().c_str()));
	setIcon( QIcon::fromTheme(name, QIcon(iconFile)) );
	// Add this menu as an action to the toolbar
	pParent->getToolBar()->addAction(this->menuAction());
}

// Dtor
VppToolbarMenu::~VppToolbarMenu() {

}

/////////////////////////////////////////////////////////

// Ctor - Attach the action to the toolbar of the mainWindow
VppToolbarAction::VppToolbarAction(	const QString& name,
																	const QString &iconFile,
																	MainWindow* pParent) :
																		QAction(	QIcon::fromTheme(name,QIcon(iconFile)),
																		tr((string("&") + name.toStdString() + string("...")).c_str()),
																		pParent){

	setStatusTip( (name.toStdString() + string("...")).c_str());

	// Populate the toolbar with the current action
	pParent->getToolBar()->addAction(this);
}

// Ctor - Attach the action to the a menu of the mainWindow
VppToolbarAction::VppToolbarAction(	const QString& name,
																	const QString &iconFile,
																	VppToolbarMenu* pParent) :
																		QAction(	QIcon::fromTheme(name,QIcon(iconFile)),
																		tr((string("&") + name.toStdString() + string("...")).c_str()),
																		pParent){

	setStatusTip( (name.toStdString() + string("...")).c_str());

	// Populate the toolbar with the current action
	pParent->addAction(this);
}

// Dtor
VppToolbarAction::~VppToolbarAction() {

}

