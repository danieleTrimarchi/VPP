#ifndef VPP_XY_CUSTOMPLOTWIDGET_H
#define VPP_XY_CUSTOMPLOTWIDGET_H

#include "qcustomplot.h"

/* Class defining a XY plot. It contains a VPPXYChart, which
 * is where the data to plot are actually set   */
class VppXYCustomPlotWidget : public QCustomPlot
{
    Q_OBJECT

public:

	/// Explicit Constructor
	explicit VppXYCustomPlotWidget(QString title, QString xAxisLabel, QString yAxisLabel,
																	QWidget* parent= Q_NULLPTR);

	/// Dtor
	~VppXYCustomPlotWidget();

	/// Add some data to the plot
	void addData(QVector<double>& x, QVector<double>& y);

	/// Set the bounds for this plot
	void setBounds(double minX, double maxX, double minY, double maxY );

Q_SIGNALS:

	void requestFullScreen(const VppXYCustomPlotWidget*);

protected :

	/// Override the parent class method called on double click
	virtual void mouseDoubleClickEvent(QMouseEvent*);

private:

};

#endif // VPP_XY_CUSTOMPLOTWIDGET_H
