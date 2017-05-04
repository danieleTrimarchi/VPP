#ifndef POLARPLOTWIDGET_H
#define POLARPLOTWIDGET_H

#include <QMainWindow>
#include <QtCore/QDebug>
#include "boost/shared_ptr.hpp"
#include <QtCharts/QChartView>
#include <QRubberBand>

#include "VppPolarChart.h"
#include "VppChartView.h"
#include "VppTabDockWidget.h"

class PolarPlotWidget : public VppTabDockWidget {

	Q_OBJECT

public:

	/// Explicit Constructor (why explicit..?? This was in the initial Qt example, see
	/// colorswatch in MainWindow example)
	explicit PolarPlotWidget(QString title, QMainWindow *parent = Q_NULLPTR, Qt::WindowFlags flags = 0);

	/// Virtual dtor
	virtual ~PolarPlotWidget();

	/// Returns the polar chart
	VppPolarChart* chart() const;

	/// Returns the chart view
	VppPolarChartView* getView() const;

private:

	/// Polar chart this widget is supposed to contain
	/// Row ptr, apparently the ownership of this item is
	/// given to the chart view
	VppPolarChart* pPolarChart_;

	/// Chart view
	boost::shared_ptr<VppPolarChartView> pChartWiew_;



};

#endif
