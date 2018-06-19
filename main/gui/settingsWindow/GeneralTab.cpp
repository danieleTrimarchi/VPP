#include "GeneralTab.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtCore/QFile>
#include <iostream>
#include "VppTags.h"

VppGeneralTabXmlWriter::VppGeneralTabXmlWriter(VppSettingsXmlWriter* pWriter) :
pXmlWriter_(pWriter) {

	// Write the header for this section of the xml document
	pXmlWriter_->writeStartElement(vppGeneralSettingTag.c_str());

}

// Dtor
VppGeneralTabXmlWriter::~VppGeneralTabXmlWriter() {
	pXmlWriter_->writeEndElement();
}

// Write the content of the general tab to xml
void VppGeneralTabXmlWriter::write(const GeneralTab* pGenTab) {

	pXmlWriter_->writeStartElement(QString("Item"));

	// Simply write the index of the choice selected by the user for
	// the solver combo-box
	pXmlWriter_->writeAttribute("Solver",pGenTab->getSolverName());
	pXmlWriter_->writeAttribute("ActiveIndex",QString::number(pGenTab->getSolver()));

	// This item is finished
	pXmlWriter_->writeEndElement();

}

//====================================================================

/// Ctor
VppGeneralTabXmlReader::VppGeneralTabXmlReader(VppSettingsXmlReader* pReader) :
		pXmlReader_(pReader) {
}

/// Read from file
bool VppGeneralTabXmlReader::read(const GeneralTab* pGenTab,QIODevice* device ) {

	// Verify this is suitable file (vppSettings v.1.0). Otherwise throw
	while (pXmlReader_->readNextStartElement()) {

		std::cout<<"pXml_->name()= "<<pXmlReader_->name().toString().toStdString() <<std::endl;
		readRecursive();
		//        if(!(pXml_->attributes().value("version") == "1.0"))
		//            pXml_->raiseError(QObject::tr("The file is not a VppSettings version 1.0 file."));

		std::cout<<"before the second readRecursive "<<std::endl;
		readRecursive();
		// If everything is fine, read the item
		//	if( pXml_->name() == "generalSettings" )
		//			readItems();
	}

	return !pXmlReader_->error();

}

void VppGeneralTabXmlReader::readRecursive(){
	while (pXmlReader_->readNextStartElement()) {

		std::cout<<"pXml_->name()= "<<pXmlReader_->name().toString().toStdString() <<std::endl;
		readRecursive();
	}
}

void VppGeneralTabXmlReader::readItems() {

	//Q_ASSERT(pXml_->isStartElement() && pXml_->name() == "vppSettings");

	while (pXmlReader_->readNextStartElement()) {

		for(size_t i=0; i<pXmlReader_->attributes().size(); i++){

			string attName= pXmlReader_->attributes().at(i).name().toString().toStdString();
			string attValue= pXmlReader_->attributes().at(i).value().toString().toStdString();

			std::cout<<"Reading : "<<attName<<"  with a value of : "<<attValue<<std::endl;
			//pGenTab->setSolver(strtoi(attValue.c_str()));
		}
	}
}

//====================================================================

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

// Returns the index of the solver currently being
// selected in the solver combo box
int GeneralTab::getSolver() const {
	return	pSolverComboBox_->currentIndex();
}

// Returns the name of the solver currently being
// selected in the solver combo box
QString GeneralTab::getSolverName() const {
	return pSolverComboBox_->currentText();
}

// Save the settings to file
void GeneralTab::save(VppSettingsXmlWriter* pWriter) {

	// Write the settings stored in the General tab
	// as this is a fixed structure - not a tree like
	// the SettingsTreeTab where we use a visitor
	VppGeneralTabXmlWriter w(pWriter);
	w.write(this);

}

// Read the settings from xml file
void GeneralTab::read(VppSettingsXmlReader* pReader) {

	VppGeneralTabXmlReader r(pReader);
	r.read(this);
}


