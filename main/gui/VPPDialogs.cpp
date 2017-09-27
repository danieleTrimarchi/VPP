#include <QLabel>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QPushButton>
#include <QFileDialog>
#include "VPPDialogs.h"
#include <QtWidgets>

DialogBase::DialogBase(QWidget *parent) :
QDialog(parent) {

	pGridLayout_.reset( new QGridLayout(this) );

}

// Add the 'Ok' and 'Cancel' buttons at the bottom of the widget
void DialogBase::addOkCancelButtons(size_t vPos) {

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	pGridLayout_->addWidget(buttonBox, vPos, 0, 1, 2);

}

///////////////////////////////////////////////////////

StateVectorDialog::StateVectorDialog(QWidget *parent)
: DialogBase(parent),
	pV_Edit_(new QLineEdit(this)),
	pPhi_Edit_(new QLineEdit(this)),
	pCrew_Edit_(new QLineEdit(this)),
	pFlat_Edit_(new QLineEdit(this)) {

	setWindowTitle(tr("Enter the state vector"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	// Set local settings for the validators
	QLocale localSettings = QLocale::c();
	localSettings.setNumberOptions(QLocale::RejectGroupSeparator | QLocale::OmitGroupSeparator);

	// Set all the line edit with double validators
	QDoubleValidator* velocityValueValidator= new QDoubleValidator(-999.0, 999.0, 2, pV_Edit_.get() );
	velocityValueValidator->setLocale(localSettings);
	pV_Edit_->setValidator(velocityValueValidator);

	QDoubleValidator* phiValueValidator= new QDoubleValidator(-999.0, 999.0, 2, pPhi_Edit_.get() );
	phiValueValidator->setLocale(localSettings);
	pPhi_Edit_->setValidator(phiValueValidator);

	QDoubleValidator* crewValueValidator= new QDoubleValidator(-999.0, 999.0, 2, pCrew_Edit_.get() );
	crewValueValidator->setLocale(localSettings);
	pCrew_Edit_->setValidator(crewValueValidator);

	QDoubleValidator* flatValueValidator= new QDoubleValidator(-999.0, 999.0, 2, pFlat_Edit_.get() );
	flatValueValidator->setLocale(localSettings);
	pFlat_Edit_->setValidator(flatValueValidator);

}

double StateVectorDialog::getV() const {
	return pV_Edit_->text().toDouble();
}

double StateVectorDialog::getPhi() const {
	return pPhi_Edit_->text().toDouble();
}

double StateVectorDialog::getCrew() const {
	return pCrew_Edit_->text().toDouble();
}

double StateVectorDialog::getFlat() const {
	return pFlat_Edit_->text().toDouble();
}

// Return the state vector under the form of a VectorXd
Eigen::VectorXd StateVectorDialog::getStateVector() const {

	Eigen::VectorXd v(4);
	v << getV(),
			getPhi(),
			getCrew(),
			getFlat();

	return v;
}

//====================================================

// Ctor
FullStateVectorDialog::FullStateVectorDialog(QWidget* parent /*=Q_NULLPTR*/) :
						StateVectorDialog(parent) {

	size_t vPos=0;

	pGridLayout_->addWidget(new QLabel(tr("V [m/s]:")), vPos, 0);
	pGridLayout_->addWidget(pV_Edit_.get(), vPos++, 1);

	pGridLayout_->addWidget(new QLabel(tr("Phi [rad]:")), vPos, 0);
	pGridLayout_->addWidget(pPhi_Edit_.get(), vPos++, 1);

	pGridLayout_->addWidget(new QLabel(tr("Crew [m]:")), vPos, 0);
	pGridLayout_->addWidget(pCrew_Edit_.get(), vPos++, 1);

	pGridLayout_->addWidget(new QLabel(tr("Flat [-]:")), vPos, 0);
	pGridLayout_->addWidget(pFlat_Edit_.get(), vPos++, 1);

	// And now add the 'Ok' or 'Cancel' buttons
	addOkCancelButtons(vPos);

}

//====================================================

// Ctor
OptimVarsStateVectorDialog::OptimVarsStateVectorDialog(QWidget* parent /*=Q_NULLPTR*/) :
						StateVectorDialog(parent) {

	size_t vPos=0;

	pV_Edit_->hide();
	pPhi_Edit_->hide();

	pGridLayout_->addWidget(new QLabel(tr("Crew [m]:")), vPos, 0);
	pGridLayout_->addWidget(pCrew_Edit_.get(), vPos++, 1);

	pGridLayout_->addWidget(new QLabel(tr("Flat [-]:")), vPos, 0);
	pGridLayout_->addWidget(pFlat_Edit_.get(), vPos++, 1);

	// And now add the 'Ok' or 'Cancel' buttons
	addOkCancelButtons(vPos);

}


//====================================================


WindIndicesDialog::WindIndicesDialog(WindItem* pWind, QWidget* parent)
: DialogBase(parent),
	pWind_(pWind),
	pTWV_Edit_(new QLineEdit(this)),
	pTWA_Edit_(new QLineEdit(this))	{

	setWindowTitle(tr("Enter the wind indices"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	// Set local settings for the validators
	QLocale localSettings = QLocale::c();
	localSettings.setNumberOptions(QLocale::RejectGroupSeparator | QLocale::OmitGroupSeparator);

	// Set all the line edit with double validators
	QIntValidator* twvValueValidator= new QIntValidator(0, pWind->getWVSize(), pTWV_Edit_.get() );
	twvValueValidator->setLocale(localSettings);
	pTWV_Edit_->setValidator(twvValueValidator);

	QIntValidator* twaValueValidator= new QIntValidator(0, pWind->getWASize(), pTWA_Edit_.get() );
	twaValueValidator->setLocale(localSettings);
	pTWA_Edit_->setValidator(twaValueValidator);

	size_t vPos=0;
	pGridLayout_->addWidget(new QLabel(tr("TWV:")), vPos, 0);
	pGridLayout_->addWidget(pTWV_Edit_.get(), vPos++, 1);

	pGridLayout_->addWidget(new QLabel(tr("TWA:")), vPos, 0);
	pGridLayout_->addWidget(pTWA_Edit_.get(), vPos++, 1);

	// And now add the 'Ok' or 'Cancel' buttons
	addOkCancelButtons(vPos);
}

int WindIndicesDialog::getTWV() const {
	return pTWV_Edit_->text().toInt();
}

int WindIndicesDialog::getTWA() const {
	return pTWA_Edit_->text().toInt();
}

// Return a ptr to the wind
WindItem* WindIndicesDialog::getWind() const {
	return pWind_;
}

//====================================================


VPPDefaultFileBrowser::VPPDefaultFileBrowser(
		QString title,
		QString fileExtension,
		QWidget *parent):
						fileExtensionFilter_(fileExtension),
						selectedFileName_(""),
						DialogBase(parent) {

	// Set the window title
	setWindowTitle(title);

	// Set the minimum width - in order for the title to be visible
	setMinimumWidth(250);

	// Declare labels that are children of the layout
	pGridLayout_->addWidget( new QLabel(QStringLiteral("Choose file..."), this), 0,0);
	QPushButton* b = new QPushButton("default...", this);
	pGridLayout_->addWidget(b, 0, 1);
	connect(b,SIGNAL(clicked()), this, SLOT(selectFile()) );

	addOkCancelButtons(1);

	setLayout(pGridLayout_.get());
}

// Returns the name (with abs path) of the file selected
// by the user
string VPPDefaultFileBrowser::getSelectedFileName() const {

	if(selectedFileName_.size())
		return selectedFileName_.toStdString();

	else
		return string("");

}

void VPPDefaultFileBrowser::selectFile() {

	QString caption;
	QString dir;

	selectedFileName_ = QFileDialog::getOpenFileName(this,caption,dir,fileExtensionFilter_);

	QPushButton* pButton= qobject_cast<QPushButton*>(sender());
	if(selectedFileName_.size())
		pButton->setText(selectedFileName_);
	else
		pButton->setText("default...");
}

//---------------------------------------------------------------

// Ctor
VPPSettingsDialog::VPPSettingsDialog(QWidget *parent)
: DialogBase(parent),
	pV_Edit_(new QLineEdit(this)) {

	setWindowTitle(tr("VPP Settings"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	// Set local settings for the validators
	QLocale localSettings = QLocale::c();
	localSettings.setNumberOptions(QLocale::RejectGroupSeparator | QLocale::OmitGroupSeparator);

	// Set all the line edit with double validators
	QDoubleValidator* velocityValueValidator= new QDoubleValidator(-999.0, 999.0, 2, pV_Edit_.get() );
	velocityValueValidator->setLocale(localSettings);
	pV_Edit_->setValidator(velocityValueValidator);

	size_t vPos=0;
	pGridLayout_->addWidget(new QLabel(tr("V [m/s]:")), vPos, 0);
	pGridLayout_->addWidget(pV_Edit_.get(), vPos++, 1);

	// And now add the 'Ok' or 'Cancel' buttons
	addOkCancelButtons(vPos);

}

// ----

GeneralTab::GeneralTab(QWidget *parent) :
						QWidget(parent) {

	QLabel *fileNameLabel = new QLabel(tr("File Name:"));
	QLineEdit *fileNameEdit = new QLineEdit("FileName");

	QLabel *pathLabel = new QLabel(tr("Path:"));
	QLabel *pathValueLabel = new QLabel("FileName");
	pathValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(fileNameLabel);
	mainLayout->addWidget(fileNameEdit);
	mainLayout->addWidget(pathLabel);
	mainLayout->addWidget(pathValueLabel);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}

// ----------------------------------------------------------------

TabDialog::TabDialog(const QString &fileName, QWidget *parent)
: QDialog(parent) {

	pTabWidget_ = new QTabWidget;
	pTabWidget_->addTab(new GeneralTab(), tr("General"));
	pTabWidget_->addTab(new FullStateVectorDialog(), tr("General1"));
	pTabWidget_->addTab(new GeneralTab(), tr("General2"));

	pButtonBox_ = new QDialogButtonBox(	QDialogButtonBox::Ok |
			QDialogButtonBox::Cancel);

	connect(pButtonBox_, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(pButtonBox_, &QDialogButtonBox::rejected, this, &QDialog::reject);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(pTabWidget_);
	mainLayout->addWidget(pButtonBox_);
	setLayout(mainLayout);

	setWindowTitle(tr("Tab Dialog"));
}
