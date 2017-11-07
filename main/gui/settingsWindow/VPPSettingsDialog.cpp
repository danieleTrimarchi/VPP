#include "VPPSettingsDialog.h"
#include "GeneralTab.h"
#include "TreeTab.h"

VPPSettingsDialog::VPPSettingsDialog(const QString &fileName, QWidget *parent)
: QDialog(parent) {

	pTabWidget_ = new QTabWidget(this);

	pTabWidget_->setContentsMargins(0,0,0,0);
	pTabWidget_->setTabShape(QTabWidget::TabShape::Triangular);

	TreeTab* pTreeTab = new TreeTab(this);
	pTabWidget_->addTab(pTreeTab, tr("TreeTab"));
	pTabWidget_->addTab(new GeneralTab(this), tr("General"));

	pButtonBox_ = new QDialogButtonBox(	QDialogButtonBox::Ok |
			QDialogButtonBox::Cancel);

	connect(pButtonBox_, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(pButtonBox_, &QDialogButtonBox::rejected, this, &QDialog::reject);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(pTabWidget_);
	mainLayout->addWidget(pButtonBox_);
	setLayout(mainLayout);

	// Resize the window in order to show as much as possible
	resize(pTreeTab->width(),pTreeTab->height());

	setWindowTitle(tr("Tab Dialog"));
}
