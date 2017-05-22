#ifndef VPP_POLAR_CUSTOMPLOTWIDGET_H
#define VPP_POLAR_CUSTOMPLOTWIDGET_H

#include "VppCustomPlotWidgetBase.h"

/// Class defining a XY plot. It contains a VPPXYChart, which
/// is where the data to plot are actually set
class VppPolarCustomPlotWidget : public VppCustomPlotWidgetBase {

		Q_OBJECT

public:

	/// Explicit Constructor
	explicit VppPolarCustomPlotWidget(QString title, QWidget* parent= Q_NULLPTR);

	/// Dtor
	~VppPolarCustomPlotWidget();

	/// Add some data to the polar plot. Derived by the qCustomPlot parametric curve demo
	virtual void addData(QVector<double>& x, QVector<double>& y, QString dataLabel /*=""*/);

private:

  /// Number of velocity markers (circles) of the polar plot
	size_t numCircles_;

private slots:

	/// Place the axes in the centre of the plot
	void centreAxes();

};

#endif // VPP_POLAR_CUSTOMPLOTWIDGET_H
