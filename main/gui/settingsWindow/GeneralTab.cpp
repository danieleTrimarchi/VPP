#include "GeneralTab.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>

GeneralTab::GeneralTab(QWidget *parent) :
						QWidget(parent),
						fontSize_(12) {

	setStyleSheet("QLabel { background-color: rgb(228,228,228); "
							" selection-color: yellow; }"
				  "QLineEdit { background-color: white; "
							" selection-color: orange; }");

	QGroupBox* pSolverGroupBox = new QGroupBox(QStringLiteral("Numerical Solver"),this);

	// Set the font
	QFont modelFont = pSolverGroupBox->font();
	modelFont.setPointSizeF(fontSize_);
	pSolverGroupBox->setFont(modelFont);

	// Set the size. Just enough to contain the combo-box
	pSolverGroupBox->setFixedHeight(80);

	/// Solver choice vertical box, that encloses the combo box
	/// for the user to choose which surface to visualize among the
	/// available surfaces
	QVBoxLayout* pSolverVBox= new QVBoxLayout(pSolverGroupBox);
	pSolverGroupBox->setLayout(pSolverVBox);

	// Note that the order here MUST match the enum GeneralTab::Solver
	pSolverComboBox_ = new QComboBox(this);
	pSolverComboBox_->addItem("nlOpt");
	pSolverComboBox_->addItem("ipOpt");
	pSolverComboBox_->addItem("noOpt");
	pSolverComboBox_->addItem("SAOA");

	QFont font = pSolverComboBox_->font();
	font.setPointSizeF(fontSize_);
	pSolverComboBox_->setFont(font);
	pSolverVBox->addWidget(pSolverComboBox_);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(pSolverGroupBox,0,Qt::AlignTop);
	setLayout(mainLayout);
}

int GeneralTab::getSolver() const {
	return	pSolverComboBox_->currentIndex();
}

