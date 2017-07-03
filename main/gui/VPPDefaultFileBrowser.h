// Copyright (c) 2016, Coventor, Inc. All rights reserved.
// The contents of this file are Coventor Confidential.

#ifndef DefaultFileBrowser_H
#define DefaultFileBrowser_H

#include "boost/shared_ptr.hpp"
#include <QDialog>
#include <QGridLayout>

class VPPDefaultFileBrowser : public QDialog
{
		Q_OBJECT

	public:
		VPPDefaultFileBrowser(QWidget *parent = Q_NULLPTR);

	private slots:

		void selectFile();

	private:

		/// Add the 'Ok' and 'Cancel' buttons at the bottom of the widget
		void addOkCancelButtons(size_t);

		/// Layout of the Dialog (place holders for the elements the dialog is made of)
		boost::shared_ptr<QGridLayout> pGridLayout_;

};

#endif // DefaultFileBrowser_H
