#ifndef MYLOGWINDOW_H
#define MYLOGWINDOW_H

#include <QTextEdit>
#include <QFile>

class LogWindow : public QTextEdit {

	Q_OBJECT

public:

	/// Ctor
	LogWindow();

	/// Virtual Dtor
	virtual ~LogWindow();

	/// Append the message to the end of the log window
	void appendMessages(const QString& text);

private:

};


#endif
