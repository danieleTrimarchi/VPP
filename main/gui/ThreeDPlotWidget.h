#ifndef THREEDPLOTWIDGET_H
#define THREEDPLOTWIDGET_H

#include "VppTabDockWidget.h"

class ThreeDPlotWidget : public VppTabDockWidget {

	Q_OBJECT

public:

	/// Explicit Constructor
	explicit ThreeDPlotWidget(QWidget* parent= Q_NULLPTR, Qt::WindowFlags flags =0);

	/// Dtor
	virtual ~ThreeDPlotWidget();

private:

	// Size of the font for the text of this plot
	qreal fontSize_;

};

#endif
