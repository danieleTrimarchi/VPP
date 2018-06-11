#ifndef GENERAL_TAB_H
#define GENERAL_TAB_H

#include <QWidget>
#include <QtWidgets/QComboBox>

/// Enum expressing the solver choice made by the user
enum solverChoice {
	nlOpt,
	ipOpt,
	noOpt,
	saoa
};

/// Base Class for the tabs to be inserted into the VPPSettingsDialog
/// Directly derived from the Qt tab dialog example
class GeneralTab : public QWidget {

		Q_OBJECT

	public:

		explicit GeneralTab(QWidget *parent = 0);

		int getSolver() const;

	private:

		/// Size of the font for the text of this plot
		qreal fontSize_;

		/// Combo-box listing all of the solver that can be selected by the user.
		QComboBox* pSolverComboBox_;

};

#endif
