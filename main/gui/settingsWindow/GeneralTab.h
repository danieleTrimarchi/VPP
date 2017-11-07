#ifndef GENERAL_TAB_H
#define GENERAL_TAB_H

#include <QWidget>

/// Base Class for the tabs to be inserted into the VPPSettingsDialog
/// Directly derived from the Qt tab dialog example
class GeneralTab : public QWidget {

		Q_OBJECT

	public:

		explicit GeneralTab(QWidget *parent = 0);

};

#endif
