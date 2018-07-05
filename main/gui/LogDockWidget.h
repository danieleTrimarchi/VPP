#ifndef LOGDOCKWIDGET_H
#define LOGDOCKWIDGET_H

#include <QDockWidget>
#include <QMainWindow>
#include <QMenu>
#include "LogWindow.h"

class LogDockWidget : public QDockWidget {

	Q_OBJECT

public:

	/// Ctor
	explicit LogDockWidget(QMainWindow *parent = Q_NULLPTR, Qt::WindowFlags flags = 0);

	/// Virtual dtor
	virtual ~LogDockWidget();

	/// Get the toggle view action to be added to the menu-bar
	QAction* getMenuToggleViewAction();

	/// Append some message to the log
	void append( const std::string& msg );

private:

	/// Log window contained in this widget
	std::shared_ptr<LogWindow> pLogWindow_;

	/// Menu in the menu-bar that drives the appeareance of this widget
	/// and all other actions related to the log widget
    std::shared_ptr<QMenu> pMenu_;

    const QString widgetTitle_;
};

#endif
