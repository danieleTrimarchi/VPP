#include "StateVectorDialog.h"
#include <QGridLayout>
#include <QLabel>
#include <QDoubleValidator>

StateVectorDialog::StateVectorDialog(QWidget *parent)
    : QDialog(parent),
	  pV_Edit_(new QLineEdit(this)),
	  pPhi_Edit_(new QLineEdit(this)),
	  pCrew_Edit_(new QLineEdit(this)),
	  pFlat_Edit_(new QLineEdit(this))	{

    setWindowTitle(tr("Enter the state vector"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QGridLayout *layout = new QGridLayout(this);

    // Set local settings for the validators
    QLocale localSettings = QLocale::c();
    localSettings.setNumberOptions(QLocale::RejectGroupSeparator | QLocale::OmitGroupSeparator);

    // Set all the line edit with double validators
    QDoubleValidator* velocityValueValidator= new QDoubleValidator(-999.0, 999.0, 2, pV_Edit_ );
    velocityValueValidator->setLocale(localSettings);
    pV_Edit_->setValidator(velocityValueValidator);

    QDoubleValidator* phiValueValidator= new QDoubleValidator(-999.0, 999.0, 2, pPhi_Edit_ );
    phiValueValidator->setLocale(localSettings);
    pPhi_Edit_->setValidator(phiValueValidator);

    QDoubleValidator* crewValueValidator= new QDoubleValidator(-999.0, 999.0, 2, pCrew_Edit_ );
    crewValueValidator->setLocale(localSettings);
    pCrew_Edit_->setValidator(crewValueValidator);

    QDoubleValidator* flatValueValidator= new QDoubleValidator(-999.0, 999.0, 2, pFlat_Edit_ );
    flatValueValidator->setLocale(localSettings);
    pFlat_Edit_->setValidator(flatValueValidator);

    size_t vPos=0;
    layout->addWidget(new QLabel(tr("V [m/s]:")), vPos, 0);
    layout->addWidget(pV_Edit_, vPos++, 1);

    layout->addWidget(new QLabel(tr("Phi [rad]:")), vPos, 0);
    layout->addWidget(pPhi_Edit_, vPos++, 1);

    layout->addWidget(new QLabel(tr("Crew [m]:")), vPos, 0);
    layout->addWidget(pCrew_Edit_, vPos++, 1);

    layout->addWidget(new QLabel(tr("Flat [-]:")), vPos, 0);
    layout->addWidget(pFlat_Edit_, vPos++, 1);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    layout->addWidget(buttonBox, vPos, 0, 1, 2);
}

double StateVectorDialog::getV() const {
	return pV_Edit_->text().toDouble();
}

QString StateVectorDialog::getPhi() const {
	return pPhi_Edit_->text();
}

QString StateVectorDialog::getCrew() const {
	return pCrew_Edit_->text();
}

QString StateVectorDialog::getFlat() const {
	return pFlat_Edit_->text();
}

