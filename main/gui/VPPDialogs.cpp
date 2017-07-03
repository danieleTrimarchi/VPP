#include <QLabel>
#include <QIntValidator>
#include <QDoubleValidator>
#include "VPPDialogs.h"

StateVectorDialog::StateVectorDialog(QWidget *parent)
    : QDialog(parent),
	  pV_Edit_(new QLineEdit(this)),
	  pPhi_Edit_(new QLineEdit(this)),
	  pCrew_Edit_(new QLineEdit(this)),
	  pFlat_Edit_(new QLineEdit(this))	{

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

// Add the 'Ok' and 'Cancel' buttons at the bottom of the widget
void StateVectorDialog::addOkCancelButtons(size_t vPos) {

  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  pGridLayout_->addWidget(buttonBox, vPos, 0, 1, 2);

}

//====================================================

// Ctor
FullStateVectorDialog::FullStateVectorDialog(QWidget* parent /*=Q_NULLPTR*/) :
		StateVectorDialog(parent) {

  size_t vPos=0;
  pGridLayout_.reset( new QGridLayout(this) );

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
  pGridLayout_.reset( new QGridLayout(this) );

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
    : QDialog(parent),
			pWind_(pWind),
			pTWV_Edit_(new QLineEdit(this)),
			pTWA_Edit_(new QLineEdit(this))	{

    setWindowTitle(tr("Enter the wind indices"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    pGridLayout_.reset( new QGridLayout(this) );

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

// Add the 'Ok' and 'Cancel' buttons at the bottom of the widget
void WindIndicesDialog::addOkCancelButtons(size_t vPos) {

  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  pGridLayout_->addWidget(buttonBox, vPos, 0, 1, 2);

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


