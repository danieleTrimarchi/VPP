#ifndef TREETAB_H
#define TREETAB_H

#include <QtWidgets/QWidget>
#include "SettingsModel.h"
#include "SettingsWindowView.h"
#include "Delegate.h"

class TreeTab : public QWidget {

		Q_OBJECT

	public:

		/// Ctor
		explicit TreeTab(QWidget *parent = 0);

		/// Dtor
		~TreeTab();

		/// Save the model the user has edited to the underlying (permanent) model
		void save();

		/// When the user hits 'cancel' in the main dialog, we
		/// revert the model: so that any change the user has
		/// done is erased.
		void revert();

		/// Save the
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
