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

	public slots:

			void updateActions();

	private :

		SettingsModel* pTreeModel_;
		SettingsWindowView* pTreeView_;
		QStyledItemDelegate* pDelegate_;
};

#endif
