#include "GeneralTab.h"
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

GeneralTab::GeneralTab(QWidget *parent) :
						QWidget(parent) {

	//setStyleSheet("background-color: white;"
	//		"selection-color: yellow; ");

	setStyleSheet("QLabel { background-color: rgb(228,228,228); "
							" selection-color: yellow; }"
				  "QLineEdit { background-color: white; "
							" selection-color: orange; }");

	QLabel *fileNameLabel = new QLabel(tr("File Name:"));
	QLineEdit *fileNameEdit = new QLineEdit("FileName");

	QLabel *otherLabel = new QLabel(tr("Other data:"));
	QLineEdit *otherNameEdit = new QLineEdit("OtherName");

	QLabel *pathLabel = new QLabel(tr("Path:"));
	QLabel *pathValueLabel = new QLabel("FileName");
	pathValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(fileNameLabel);
	mainLayout->addWidget(fileNameEdit);
	// --
	mainLayout->addWidget(otherLabel);
	mainLayout->addWidget(otherNameEdit);
	// --
	mainLayout->addWidget(pathLabel);
	mainLayout->addWidget(pathValueLabel);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}

