#ifndef STATEVECTORDIALOG_H
#define STATEVECTORDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QGridLayout>
#include "boost/shared_ptr.hpp"
#include <Eigen/Core>
#include "VppAeroItem.h"

QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE

/// Base class for widget dialog used to prompt values
class DialogBase : public QDialog {

		Q_OBJECT

	public:

	protected:

		/// Explicit protected Ctor
		explicit DialogBase(QWidget *parent = Q_NULLPTR);

		/// Add the 'Ok' and 'Cancel' buttons at the bottom of the widget
		void addOkCancelButtons(size_t);

		/// Layout of the Dialog (place holders for the elements the dialog is made of)
		boost::shared_ptr<QGridLayout> pGridLayout_;

};

//---------------------------------------------------------------

/// Base class used to prompt the values for the VPP state vector:
/// V, Phi, Crew, Flat. Used mainly for plotting purposes
class StateVectorDialog : public DialogBase {

		Q_OBJECT

	public:

		/// Returns the value of V entered by the user
		double getV() const;

		/// Returns the value of Phi entered by the user
		double getPhi() const;

		/// Returns the value of Crew entered by the user
		double getCrew() const;

		/// Returns the value of Flat entered by the user
		double getFlat() const;

		/// Return the state vector under the form of a VectorXd
		Eigen::VectorXd getStateVector() const;

	protected:

		/// Explicit protected Ctor
		explicit StateVectorDialog(QWidget *parent = Q_NULLPTR);

		boost::shared_ptr<QLineEdit> pV_Edit_, pPhi_Edit_, pCrew_Edit_, pFlat_Edit_;

};

///---------------------------------------------------------------
/// Prompt the full state vector : V, Phi, Crew, Flat
class FullStateVectorDialog : public StateVectorDialog {

		Q_OBJECT

	public:

		/// Explicit Ctor
		explicit FullStateVectorDialog(QWidget *parent = Q_NULLPTR);

};

///---------------------------------------------------------------
/// Prompt the optimization variables of the state vector : Crew, Flat
class OptimVarsStateVectorDialog : public StateVectorDialog {

		Q_OBJECT

	public:

		/// Explicit Ctor
		explicit OptimVarsStateVectorDialog(QWidget *parent = Q_NULLPTR);

};

//---------------------------------------------------------------
/// Prompts the wind indexes : TWA, TWV
class WindIndicesDialog : public DialogBase {

		Q_OBJECT

	public:

		/// Explicit Ctor with a wind item
		explicit WindIndicesDialog(WindItem*, QWidget *parent = Q_NULLPTR);

		/// Returns the index of the TWV prompted by the user
		int getTWV() const;

		/// Returns the index of the TWA prompted by the user
		int getTWA() const;

		/// Return a ptr to the wind
		WindItem* getWind() const;

	private:

		/// raw ptr to the wind
		WindItem* pWind_;

		boost::shared_ptr<QLineEdit> pTWV_Edit_, pTWA_Edit_;

};

///---------------------------------------------------------------
/// Class that implements a dialog with a button.
/// The button label is initially set to 'default'
/// Pressing the button opens a file browser. Once
/// a file has been selected, the button label changes
/// from 'default' to path/selectedFileName
class VPPDefaultFileBrowser : public DialogBase {

		Q_OBJECT

	public:

		/// Ctor
		VPPDefaultFileBrowser(QString title,
				QString fileExtension,
				QWidget *parent = Q_NULLPTR);

		/// Returns the name (with abs path) of the file selected
		/// by the user
		string getSelectedFileName() const;

		private slots:

		/// Opens a file browser to select a file
		void selectFile();

		private:

		/// File type and name
		QString fileExtensionFilter_, selectedFileName_;

};

///---------------------------------------------------------------
/// Class implementing the VPP interface used to prompt all of the
/// values required by the program. These values were originally
/// stored in the *.vppin file. The class allows for ui-based filling
/// of these values.
class VPPSettingsDialog : public DialogBase {

		Q_OBJECT

	public:

		/// Ctor
		VPPSettingsDialog(QWidget *parent = Q_NULLPTR);

	private:

		/// Temporary place-holder that will be removed
		boost::shared_ptr<QLineEdit> pV_Edit_;

};

///---------------------------------------------------------------
/// Base Class for the tabs to be inserted into the VPPSettingsDialog
/// Directly derived from the Qt tab dialog example
class GeneralTab : public QWidget {

		Q_OBJECT

	public:

		explicit GeneralTab(QWidget *parent = 0);

};

///---------------------------------------------------------------
/// Class implementing the VPP Config Tab, living in the VPPSettingsDialog
/// The VPP Config tab contains the bounds for the state vector variables
class TabDialog : public QDialog {

		Q_OBJECT

	public:

		explicit TabDialog(const QString &fileName, QWidget *parent = 0);

	private:

		QTabWidget* pTabWidget_;
		QDialogButtonBox* pButtonBox_;
};


#endif
