#include <QtWidgets/QStyle>
#include "VppSettingsXmlReader.h"
#include <iostream>

// Ctor
VppSettingsXmlReader::VppSettingsXmlReader() //QTreeWidget *treeWidget)
//: pTtreeWidget_(treeWidget) {
{
	// Instantiate a xml reader. The device will be assigned later on
	pXml_.reset(new QXmlStreamReader);

	// One could assign icons here (see xbelReader example) but I do not
	// need icons for the vpp settings
}

bool VppSettingsXmlReader::read(QIODevice *device) {

	// who am I reading from?
	pXml_->setDevice(device);

	// Verify this is suitable file (vppSettings v.1.0). Otherwise throw
	if (pXml_->readNextStartElement()) {
			std::cout<<"TEXT= "<<pXml_->text().toString().toStdString()<<std::endl;
        	std::cout<<"pXml_->name()="<<pXml_->name().toString().toStdString()<<
        		" attributes="<<pXml_->attributes().value("version").toString().toStdString()<<std::endl;
        if (pXml_->name() == "vppSettings" && pXml_->attributes().value("version") == "1.0")
			read();
		else
			pXml_->raiseError(QObject::tr("The file is not a VppSettings version 1.0 file."));
	}

	return !pXml_->error();
}

void VppSettingsXmlReader::read() {

	//Q_ASSERT(pXml_->isStartElement() && pXml_->name() == "vppSettings");

	std::cout<<"xml NAME = "<<pXml_->name().toString().toStdString()<<std::endl;

//	while (pXml_->readNextStartElement()) {
//		std::cout<<"xml NAME = "<<pXml_->name().toString().toStdString()<<std::endl;
////		pXml_->readNext();
////		std::cout<<"Name of this item : "<<pXml_->name().toString().toStdString()<<std::endl;
		std::cout<<"Attribute of this item : \n";
		for(size_t i=0; i<pXml_->attributes().size(); i++)
    		std::cout<<pXml_->attributes().at(i).name().toString().toStdString()<<
				" : "<<pXml_->attributes().at(i).value().toString().toStdString()<<std::endl;
//    QString title = pXml_->readElementText();
//    std::cout<<"Title of this guy= "<<title.toStdString()<<std::endl;
//
////    QString title = pXml_->readElementText();
//
//		// Do we need this to read all elements? -> no
////		pXml_->skipCurrentElement();
//		}
  while (pXml_->readNextStartElement()) {
  		read();
  		//pXml_->skipCurrentElement();
  }

	std::cout<<"End of read"<<std::endl;
}

QString VppSettingsXmlReader::errorString() const {

	return QObject::tr("%1\nLine %2, column %3")
	.arg(pXml_->errorString())
	.arg(pXml_->lineNumber())
	.arg(pXml_->columnNumber());
}

//void VppSettingsXmlReader::readTitle(QTreeWidgetItem* item) {
//
//	Q_ASSERT(pXml_->isStartElement() && pXml_->name() == "title");
//
//	QString title = pXml_->readElementText();
//	item->setText(0, title);
//}
//
//void VppSettingsXmlReader::readSeparator(QTreeWidgetItem* item) {
//
//	Q_ASSERT(pXml_->isStartElement() && pXml_->name() == "separator");
//
//	QTreeWidgetItem *separator = createChildItem(item);
//	separator->setFlags(item->flags() & ~Qt::ItemIsSelectable);
//	separator->setText(0, QString(30, 0xB7));
//	pXml_->skipCurrentElement();
//}
//
//void VppSettingsXmlReader::readFolder(QTreeWidgetItem *item) {
//
//	Q_ASSERT(pXml_->isStartElement() && pXml_->name() == "folder");
//
//	QTreeWidgetItem *folder = createChildItem(item);
//	bool folded = (pXml_->attributes().value("folded") != "no");
//	pTtreeWidget_->setItemExpanded(folder, !folded);
//
//	while (pXml_->readNextStartElement()) {
//		if (pXml_->name() == "title")
//			readTitle(folder);
//		else if (pXml_->name() == "folder")
//			readFolder(folder);
//		else if (pXml_->name() == "bookmark")
//			readBookmark(folder);
//		else if (pXml_->name() == "separator")
//			readSeparator(folder);
//		else
//			pXml_->skipCurrentElement();
//	}
//}
//
//void VppSettingsXmlReader::readBookmark(QTreeWidgetItem *item) {
//
//	Q_ASSERT(pXml_->isStartElement() && pXml_->name() == "bookmark");
//
//	QTreeWidgetItem *bookmark = createChildItem(item);
//	bookmark->setFlags(bookmark->flags() | Qt::ItemIsEditable);
//	bookmark->setIcon(0, bookmarkIcon);
//	bookmark->setText(0, QObject::tr("Unknown title"));
//	bookmark->setText(1, pXml_->attributes().value("href").toString());
//
//	while (pXml_->readNextStartElement()) {
//		if (pXml_->name() == "title")
//			readTitle(bookmark);
//		else
//			pXml_->skipCurrentElement();
//	}
//}

//QTreeWidgetItem *VppSettingsXmlReader::createChildItem(QTreeWidgetItem *item) {
//
//	QTreeWidgetItem *childItem;
//	if (item) {
//		childItem = new QTreeWidgetItem(item);
//	} else {
//		childItem = new QTreeWidgetItem(pTtreeWidget_);
//	}
//	childItem->setData(0, Qt::UserRole, pXml_->name().toString());
//	return childItem;
//}
