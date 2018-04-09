#ifndef VPP_ACTION
#define VPP_ACTION

#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>

/// Forward declarations
class MainWindow;

class VppToolbarMenu : public QMenu {

		Q_OBJECT

	public:

		/// Ctor - Attach the menu to the toolbar of the mainWindow
		VppToolbarMenu(const QString& name,
									const QString &iconFile,
									MainWindow* pParent);

		/// Dtor
		~VppToolbarMenu();

	private:

};

////////////////////////////////////////////////////

/// Class derived from QAction. It constitutes
/// a simple interface to be used in the context
/// of setting the menu bar of the main Window
class VppToolbarAction : public QAction {

		Q_OBJECT

	public:

		/// Ctor - Attach the action to the toolbar of the mainWindow
		VppToolbarAction(const QString& name,
										const QString &iconFile,
										MainWindow* pParent);

		/// Ctor - Attach the action to the a menu of the mainWindow
		VppToolbarAction(const QString& name,
										const QString &iconFile,
										VppToolbarMenu* pParent);

		/// Dtor
		~VppToolbarAction();

	private:

};

#endif
