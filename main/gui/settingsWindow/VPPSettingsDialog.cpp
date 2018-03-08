#include "VPPSettingsDialog.h"
#include "GeneralTab.h"
#include <QtWidgets/QVBoxLayout>
#include <iostream>

// Init static (instance) member
VPPSettingsDialog* VPPSettingsDialog::pInstance_= 0;

// Singleton-style constructor
VPPSettingsDialog* VPPSettingsDialog::getInstance(QWidget *parent){

	if(!pInstance_)
		pInstance_= new VPPSettingsDialog(parent);
	return pInstance_;
}

// Save the settings to file
void VPPSettingsDialog::save(QFile& file) {

	// Save the content of the pTreeTab
	pSettingsTreeTab_->save(file);
}

// Read some settings from file
void VPPSettingsDialog::read(QFile& file) {

	// Save the content of the pTreeTab
	pSettingsTreeTab_->read(file);

}

// Handle on the settingsTree tab
TreeTab* VPPSettingsDialog::getSettingsTreeTab() {
	return pSettingsTreeTab_;
}

// Ctor
VPPSettingsDialog::VPPSettingsDialog(QWidget* myparent)
: QDialog(myparent) {

	// Instantiate a widget that will contain tabs
	pTabWidget_ = new QTabWidget(this);

	// Set the shape of the tab widget
	pTabWidget_->setContentsMargins(0,0,0,0);
	pTabWidget_->setTabShape(QTabWidget::TabShape::Triangular);

	// Add a tree tab (a tab containing an item tree)
	pSettingsTreeTab_ = new TreeTab(this);
	pTabWidget_->addTab(pSettingsTreeTab_, tr("VPP Settings"));

	// Add a general tab (a tab containing general settings, not sure if we'll keep this)
	pTabWidget_->addTab(new GeneralTab(this), tr("General"));

	// Add the buttons 'OK' and "Cancel'
	pButtonBox_ = new QDialogButtonBox(	QDialogButtonBox::Ok |
			QDialogButtonBox::Cancel);

	// Connect signals to the buttons
	connect(pButtonBox_, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(pButtonBox_, &QDialogButtonBox::rejected, this, &QDialog::reject);

	// Instantiate a VBoxLayout, the tab are on top and the buttons on the bottom
	// Set this layout for the Dialog
	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(pTabWidget_);
	mainLayout->addWidget(pButtonBox_);
	setLayout(mainLayout);

	// Resize the window in order to show as much as possible
	resize(pSettingsTreeTab_->width(),pSettingsTreeTab_->height());

	// Set the title of this widget
	setWindowTitle(tr("Tab Dialog"));
}

// Slot called when the user hits the button "Ok" in the bottom of the widget
void VPPSettingsDialog::accept(){

    QObject* myParent = parent();
    QObject* grandpa= myParent->parent();

	// Sync phase : store the 'floating' the user has just edited into the reference model
	pSettingsTreeTab_->save();

    myParent = parent();
    grandpa= myParent->parent();

	// Call mother class method
	QDialog::accept();

}

// Slot called when the user hits the button "Cancel" in the bottom of the widget
void VPPSettingsDialog::reject() {

	// Sync phase : store the 'floating' the user has just edited into the reference model
	pSettingsTreeTab_->revert();

	//Call mother class method
	QDialog::reject();

}

// Disallowed default constructor
VPPSettingsDialog::VPPSettingsDialog() :
		pTabWidget_(0),
		pSettingsTreeTab_(0),
		pButtonBox_(0){

}
