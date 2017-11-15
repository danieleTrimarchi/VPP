#ifndef VPP_SETTINGS_DIALOG_H
#define VPP_SETTINGS_DIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTabWidget>

/// Implements the VPP Settings Dialog, or the settings window where it is
/// possible to edit all properties requested to run the VPP
class VPPSettingsDialog : public QDialog {

		Q_OBJECT

	public:

		/// Ctor
		explicit VPPSettingsDialog(const QString &fileName, QWidget *parent = 0);

	private:

		/// The Dialog has a tab structure. Tabs are added to the tabWidget
		QTabWidget* pTabWidget_;

		/// The Dialog also has two buttons to save or cancel the editing
		QDialogButtonBox* pButtonBox_;
};

#endif
