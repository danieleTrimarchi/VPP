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

	/// Add the circles that draw the coordinate system on the canvas. One circle per
	/// integer value
	void addCircles();

protected:

  /// Select a curve - in this case a QCPCurve
  virtual void select(QCPAbstractPlottable *);

protected slots:

		/// Show the coordinates of a point - connected to mouseMoveEvent.
		/// Implemented of the advice given in: https://stackoverflow.com/questions/
		/// 18140446/display-the-plot-values-on-mouse-over-detect-scatter-points
		/// Pure virtual because the format of this method must change according to
		/// the type of plot - polar plot must convert the xy coordinates to polar!
		virtual void showPointToolTip(QMouseEvent*);

	/// Axes equal after each modification of the plot
	/// Set axes equal, so that circles will appear as circles
	/// Not sure why, there is some interaction with the multiplotWidget (?)
	/// that makes this not functional to begin with. One needs a mousePressEvent
	/// to get the right plot...!
	void axesEqual();

private slots:

	/// Place the axes in the centre of the plot
	void centreAxes();

};

#endif // VPP_POLAR_CUSTOMPLOTWIDGET_H
