#ifndef VPP_SETTINGS_DIALOG_H
#define VPP_SETTINGS_DIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QTabWidget>

class VPPSettingsDialog : public QDialog {

		Q_OBJECT

	public:

		explicit VPPSettingsDialog(const QString &fileName, QWidget *parent = 0);

	private:

		QTabWidget* pTabWidget_;
		QDialogButtonBox* pButtonBox_;
};

#endif
