#ifndef VPP_CUSTOMPLOT_WIDGET_H
#define VPP_CUSTOMPLOT_WIDGET_H

#include <QMainWindow>
#include "VppTabDockWidget.h"

/* Class defining a XY plot. It contains a VPPXYChart, which
 * is where the data to plot are actually set   */
class VppCustomPlotWidget : public VppTabDockWidget
{
    Q_OBJECT

public:

	/// Explicit Constructor
	explicit VppCustomPlotWidget(QMainWindow* parent= Q_NULLPTR, Qt::WindowFlags flags =0);

	/// Dtor
	~VppCustomPlotWidget();

private:

	/// Demo a simple quadratic plot
	void setupQuadraticDemo(QString& demoName);

	/// Demo multiple curves and mouse-driven zoom/pan and curve selection
	/// (the selected curve increases thickness and decorations)
  void setupSimpleDemo(QString& demoName);

  /// Demo cross points and error bars
  void setupSincScatterDemo(QString& demoName);

  /// Demo different markers types and colours. Xcl-style plot.
  void setupScatterStyleDemo(QString& demoName);

  /// Demo plot with underlying -integral- area.
  /// Not sure why the underlying images do not show, but who cares
  void setupScatterPixmapDemo(QString& demoName);

  /// Demo how to set a dashed line. Again the underlying image do
  /// not show.
  void setupTextureBrushDemo(QString& demoName);

  /// Wow! - multiple plots in the same area. Left axis and related
  /// curve are subjected to mouse actions. Right axis is fixed.
  /// Several line and marker styles shown
  void setupMultiAxisDemo(QString& demoName);

  /// Parametric curves with mouse actions (zoom, pan and select)
  void setupParametricCurveDemo(QString& demoName);

  /// ARROWS!
  void setupSimpleItemDemo(QString& demoName);

};

#endif // MAINWIDGET_H
