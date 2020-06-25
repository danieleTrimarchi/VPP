#ifndef VPP_SETTINGS_DIALOG_H
#define VPP_SETTINGS_DIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTabWidget>
#include "TreeTab.h"

/// Forward declarations
class GeneralTab;

/// Implements the VPP Settings Dialog, or the settings window where it is
/// possible to edit all properties requested to run the VPP
/// Implements a singleton
class VPPSettingsDialog : public QDialog {

		Q_OBJECT

	public:

		/// Singleton-style constructor
		static VPPSettingsDialog* getInstance(QWidget *parent = 0);

		/// Save the settings to file
		void save(QFile& file);

		/// Read some settings from file
		void read(QFile& file);

		/// Handle on the settingsTree tab
		TreeTab* getSettingsTreeTab() const;

		/// Handle on the settingsTree tab
		GeneralTab* getGeneralTab() const;

	public Q_SLOTS:

		/// Slot called when the user hits the button "Ok" in the bottom of the widget
  		virtual void accept();

		/// Slot called when the user hits the button "Cancel" in the bottom of the widget
		virtual void reject();

	private:

		/// Private Ctor - instantiated by the getInstance -> this is a Singleton!
		explicit VPPSettingsDialog(QWidget *parent = 0);

		/// Disallow default constructor
		VPPSettingsDialog();

		/// Active instance of this class
		static VPPSettingsDialog* pInstance_;

		/// The Dialog has a tab structure. Tabs are added to the tabWidget
		QTabWidget* pTabWidget_;

		/// Ptr to the tree tab. This needs to be known for synchronization
		TreeTab* pSettingsTreeTab_;

		/// Ptr to the general tab.
		GeneralTab* pGeneralTab_;

		/// The Dialog also has two buttons to save or cancel the editing
		QDialogButtonBox* pButtonBox_;
};

#endif
