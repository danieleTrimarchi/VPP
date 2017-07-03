// Copyright (c) 2016, Coventor, Inc. All rights reserved.
// The contents of this file are Coventor Confidential.

#include <QPushButton>
#include <QGraphicsGridLayout>
#include <QLabel>
#include <QFileDialog>
#include <QDialogButtonBox>
#include "VPPDefaultFileBrowser.h"

VPPDefaultFileBrowser::VPPDefaultFileBrowser(QWidget *parent):
 QDialog(parent) {

	// Set the window title
	setWindowTitle("Sail coefficient file browser");

	// Set the minimum width - in order for the title to be visible
	setMinimumWidth(250);

	// Declare a DefaultFileBrowserLayout to be filled with the data
	pGridLayout_.reset( new QGridLayout );

	// Declare labels that are children of the layout
	pGridLayout_->addWidget( new QLabel(QStringLiteral("Choose file..."), this), 0,0);
	QPushButton* b = new QPushButton("default...", this);
	pGridLayout_->addWidget(b, 0, 1);
	connect(b,SIGNAL(clicked()), this, SLOT(selectFile()) );

	addOkCancelButtons(1);

	setLayout(pGridLayout_.get());
}

void VPPDefaultFileBrowser::selectFile() {

	QString caption;
	QString dir;

	QString fileName = QFileDialog::getOpenFileName(this,caption,dir,
			tr("Sail Coefficient Input File(*.sailCoeff);; All Files (*.*)"));

	QPushButton* pButton= qobject_cast<QPushButton*>(sender());
	if(fileName.size())
		pButton->setText(fileName);
	else
		pButton->setText("default...");
}

// Add the 'Ok' and 'Cancel' buttons at the bottom of the widget
void VPPDefaultFileBrowser::addOkCancelButtons(size_t vPos) {

  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  pGridLayout_->addWidget(buttonBox, vPos, 0, 1, 2);

}

