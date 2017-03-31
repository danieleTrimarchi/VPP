#ifndef THREEDPLOTWIDGET_H
#define THREEDPLOTWIDGET_H

#include "VppTabDockWidget.h"
#include "Surfacegraph.h"

class ThreeDPlotWidget : public VppTabDockWidget {

	Q_OBJECT

public:

	/// Explicit Constructor
	explicit ThreeDPlotWidget(QWidget* parent= Q_NULLPTR, Qt::WindowFlags flags =0);

	/// Dtor
	virtual ~ThreeDPlotWidget();

	/// Get the underlying surfaceGraph, trough which we will be adding
	/// the data to plot
	SurfaceGraph* getSurfaceGraph();

private:

	/// Underlying surfaceGraph, trough which we will
	/// be adding the data to plot
	SurfaceGraph* modifier_;

	// Size of the font for the text of this plot
	qreal fontSize_;

};

#endif
