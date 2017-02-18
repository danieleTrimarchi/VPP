#ifndef VPPPLOTWIDGET_H
#define VPPPLOTWIDGET_H

#include <QtWidgets/QWidget>
#include <QDockWidget>

/// Mother class for all the plot widgets of the VPP program
class VppTabDockWidget : public QDockWidget {

	Q_OBJECT

public:

	/// Explicit Constructor
	explicit VppTabDockWidget(QWidget* parent= Q_NULLPTR, Qt::WindowFlags flags =0);

	/// Dtor
	virtual ~VppTabDockWidget();

Q_SIGNALS:

	/// Send a signal on deletion, that will be used by the parent class
	/// to update the list of plots alive
	void itemDeleted(const VppTabDockWidget*);

private:

};

#endif
