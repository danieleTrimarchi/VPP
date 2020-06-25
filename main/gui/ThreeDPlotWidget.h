#ifndef THREEDPLOTWIDGET_H
#define THREEDPLOTWIDGET_H

#include "VppTabDockWidget.h"
#include "SurfaceGraph.h"

class ThreeDPlotWidget : public VppTabDockWidget {

	Q_OBJECT

public:

	/// Explicit Constructor
	explicit ThreeDPlotWidget(QWidget* parent= Q_NULLPTR, Qt::WindowFlags flags =0);

	/// Dtor
	virtual ~ThreeDPlotWidget();

	/// Add a surface chart to this ThreeDPlotWidget
	void addChart( vector<ThreeDDataContainer> );

private:

	/// Underlying surfaceGraph, trough which we will
	/// be adding the data to plot
	SurfaceGraph* surfaceGraph_;

	/// Widget wrapped by the ThreeDPlotWidget, it is effectively the widget
	/// contained in the 3d plot view
	QWidget* pWidget_;

	/// Combo-box listing all of the surfaces that can be selected by the user.
	/// The list is driven by the data that have been added to the plot
	QComboBox* surfaceList_;

	/// Size of the font for the text of this plot
	qreal fontSize_;

};

#endif
