#ifndef TREETAB_H
#define TREETAB_H

#include <QtCore/QFile>
#include <QtWidgets/QWidget>
#include "SettingsModel.h"
#include "Delegate.h"
#include "SettingsWindowView.h"

/// Forward Declarations
class MainWindow;

/// This class represents a tab of a property widget. This
/// Tab stores an item tree, the content of which are the
/// settings to be used to drive the VPP
class TreeTab : public QWidget {

		Q_OBJECT

	public:

		/// Ctor
		explicit TreeTab(QWidget *parent = 0);

		/// Dtor
		~TreeTab();

		/// Save the model the user has edited to the underlying (permanent) model
		void save();

		/// Save the settings to xml file
		void save(VppSettingsXmlWriter* pWriter);

		/// Read the settings from xml file
		void read(VppSettingsXmlReader* pReader);

		/// When the user hits 'cancel' in the main dialog, we
		/// revert the model: so that any change the user has
		/// done is erased.
		void revert();

		/// Get the reference model. This is where the settings
		/// are stored
		const SettingsModel* getReferenceSettingsModel() const;

		/// Update external dependencies, such as the VariableTreeModel
		void updateDependencies();

		//-------------
		// Who calls these methods?
		virtual void setParent(QWidget *);
		virtual void setParent(QWidget *, QFlags<enum Qt::WindowType>);
		//-------------

	public slots:

		void updateActions();

	private :

		/// Connect all the required signals
		/// todo dtrimarchi, this should be moved to
		/// the view, that owns a ptr to the model
		void connectSignals();

		SettingsModel *pTreeReferenceModel_, *pTreeModel_;
		SettingsWindowView* pTreeView_;
		QStyledItemDelegate* pDelegate_;
};

#endif
