#ifndef SETTINGS_WINDOW_VIEW_H
#define SETTINGS_WINDOW_VIEW_H

#include <QtCore/QModelIndex>
#include <QtWidgets/QTreeView>

class SettingsWindowView : public QTreeView {

public:

	/// Ctor
	SettingsWindowView(QWidget *parent = Q_NULLPTR);

	/// Dtor
	~SettingsWindowView();

private:

};

#endif

