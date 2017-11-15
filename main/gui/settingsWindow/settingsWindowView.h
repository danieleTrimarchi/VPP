#ifndef SETTINGS_WINDOW_VIEW_H
#define SETTINGS_WINDOW_VIEW_H

#include <QtCore/QModelIndex>
#include <QtWidgets/QTreeView>
#include "SettingsModel.h"

class SettingsWindowView : public QTreeView {

	public:

		/// Ctor
		SettingsWindowView(SettingsModel* pModel,QWidget *parent = Q_NULLPTR);

		/// Dtor
		~SettingsWindowView();

	public slots:

		/// Call the resizeColumnToContents of the view
		void resizeColumnsToContents(const QModelIndex& );

	private:

		/// Ptr to the model this view is about to show
		SettingsModel* pModel_;
};

#endif

