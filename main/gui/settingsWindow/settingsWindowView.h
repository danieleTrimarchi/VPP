#ifndef SETTINGS_WINDOW_VIEW_H
#define SETTINGS_WINDOW_VIEW_H

#include <QtCore/QModelIndex>
#include <QtWidgets/QTreeView>
#include "SettingsModel.h"

class SettingsWindowView : public QTreeView {

		Q_OBJECT

	public:

		/// Ctor
		SettingsWindowView(SettingsModel* pModel,QWidget *parent = Q_NULLPTR);

		/// Dtor
		~SettingsWindowView();

	Q_SIGNALS :

		/// On copy, emit a signal that will notify the view that a
		/// given item must be expanded
		void mustExpand(const QModelIndex& );

		/// On copy, emit a signal that will notify the view that a
		/// given item must be collapsed
		void mustCollapse(const QModelIndex& );

	public slots:

		/// Call the resizeColumnToContents of the view
		void resizeColumnsToContents(const QModelIndex& );

		/// Override expand signal from the base class
    void doExpand(const QModelIndex &index);

		/// Override collapse signal from the base class
    void doCollapse(const QModelIndex &index);

	private:

		/// Ptr to the model this view is about to show
		SettingsModel* pModel_;
};

#endif

