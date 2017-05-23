#ifndef VPP_XY_CUSTOMPLOTWIDGET_H
#define VPP_XY_CUSTOMPLOTWIDGET_H

#include "VppCustomPlotWidgetBase.h"

/// Class defining a XY plot. It contains a VPPXYChart, which
/// is where the data to plot are actually set
class VppXYCustomPlotWidget : public VppCustomPlotWidgetBase {

		Q_OBJECT

public:

	/// Explicit Constructor
	explicit VppXYCustomPlotWidget(QString title, QString xAxisLabel, QString yAxisLabel,
																	QWidget* parent= Q_NULLPTR);

	/// Dtor
	~VppXYCustomPlotWidget();

protected:

  /// Select a curve - in this case a QCPGraphs
  virtual void select(QCPAbstractPlottable *);

};

#endif // VPP_XY_CUSTOMPLOTWIDGET_H
