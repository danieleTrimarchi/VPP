#ifndef GENERAL_TAB_H
#define GENERAL_TAB_H

#include <string>
#include <QWidget>
#include <QtWidgets/QComboBox>
#include <QtCore/QXmlStreamWriter>
#include "VppSettingsXmlWriter.h"
#include "VppSettingsXmlReader.h"

using namespace std;

/// Forward declarations
class QFile;
class GeneralTab;

/// Enum expressing the solver choice made by the user
enum solverChoice {
	nlOpt,
	ipOpt,
	noOpt,
	saoa
};

/// XML writer class, contains a QXmlStreamWriter that
/// actually does the job of writing the XML file.
/// See similitude with VppSettingsXmlWriter, from which
/// this class derives
class VppGeneralTabXmlWriter {

	public:

		/// Ctor
		VppGeneralTabXmlWriter(VppSettingsXmlWriter* pWriter);

		/// Dtor
		~VppGeneralTabXmlWriter();

		/// Write the content of the general tab to xml
		void write(const GeneralTab*);

	private:

		/// Ptr to the xml writer, not owned by me
		VppSettingsXmlWriter* pXmlWriter_;

};


class VppGeneralTabXmlReader {

	public:

		/// Ctor
		VppGeneralTabXmlReader(VppSettingsXmlReader* pReader,const GeneralTab* parentTab);

		/// Read from file
		bool read();

	private:

		/// Read the content of the file
		void readItems();

		/// Ptr to the xml reader
		VppSettingsXmlReader* pXmlReader_;

		/// Ptr to my parent
		const GeneralTab* pGenTab_;
};

//====================================================================

/// Base Class for the tabs to be inserted into the VPPSettingsDialog
/// Directly derived from the Qt tab dialog example
class GeneralTab : public QWidget {

		Q_OBJECT

	public:

		explicit GeneralTab(QWidget *parent = 0);

		/// Returns the index of the solver currently being
		/// selected in the solver combo box
		int getSolver() const;

		/// Sets the index of the solver to be
		/// selected in the solver combo box
		void setSolver(int index) const;

		/// Returns the name of the solver currently being
		/// selected in the solver combo box
		QString getSolverName() const;

		/// Save the settings to xml file
		void save(VppSettingsXmlWriter* pWriter);

		/// Read the settings from xml file
		void read(VppSettingsXmlReader* pReader);

	private:

		/// Size of the font for the text of this plot
		qreal fontSize_;

		/// Combo-box listing all of the solver that can be selected by the user.
		QComboBox* pSolverComboBox_;

};

#endif
