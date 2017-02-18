#include "LogDockWidget.h"

// Explicit Constructor (why explicit..?? This was in the initial Qt example, see
// colorswatch in MainWindow example)
LogDockWidget::LogDockWidget(QMainWindow* parent/*= Q_NULLPTR*/, Qt::WindowFlags flags/*=0*/) :
	QDockWidget(parent, flags),
	widgetTitle_("LogView"){

    setObjectName(widgetTitle_);
    setWindowTitle(widgetTitle_);

    // Instantiate the log window that widget is made for
    pLogWindow_.reset(new LogWindow);

	// This must be an output redirect, not a text edit
	pLogWindow_->append("Pre instantiation...");

    // Assign the LogWindow to this widget
    setWidget(pLogWindow_.get());

	pLogWindow_->append("Post instantiation...");
	for(size_t i=0; i<20; i++){
		char msg[256];
		sprintf(msg,"Message %zu", i);
		pLogWindow_->append(msg);
	}
}

// Virtual dtor
LogDockWidget::~LogDockWidget() {

}

// Get the toggle view action to be added to the menubar
QAction* LogDockWidget::getMenuToggleViewAction() {

	// This menu entry drives the visibility of the
	// dockWidget
	QAction* pToggleViewAction = toggleViewAction();
	pToggleViewAction->setShortcut(QKeySequence::New);
	pToggleViewAction->setStatusTip("Opens the Log widget");

	return pToggleViewAction;

}

// Append some message to the log
void LogDockWidget::append( const std::string& msg ) {
	pLogWindow_->append(msg.c_str());
}



