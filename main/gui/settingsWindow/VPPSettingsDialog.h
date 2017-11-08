#ifndef VPP_SETTINGS_DIALOG_H
#define VPP_SETTINGS_DIALOG_H

#include <QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTabWidget>

class VPPSettingsDialog : public QDialog {

		Q_OBJECT

	public:

		explicit VPPSettingsDialog(const QString &fileName, QWidget *parent = 0);

	private:

		QTabWidget* pTabWidget_;
		QDialogButtonBox* pButtonBox_;
};

#endif
