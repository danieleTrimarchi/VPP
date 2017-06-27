#include "LogWindow.h"


// Ctor
LogWindow::LogWindow() {

	// The log window is not user editable
	this->setReadOnly(true);

}

// Virtual Dtor
LogWindow::~LogWindow() {

}

void LogWindow::appendMessages(const QString& text) {

	QTextEdit::append(text);

}
