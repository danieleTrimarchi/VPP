#include "SettingsWindowView.h"

// Ctor
SettingsWindowView::SettingsWindowView(QWidget *parent) :
	QTreeView(parent){

	setStyleSheet(	"background-color: rgb(228,228,228);"
					"selection-color: white;");

	setWindowModality(Qt::ApplicationModal);

	setFrameStyle(QFrame::NoFrame);
	setLineWidth(10);
}


// Dtor
SettingsWindowView::~SettingsWindowView() {
	/* make nothing */
}
