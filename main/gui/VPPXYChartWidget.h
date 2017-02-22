#ifndef XYPLOTWIDGET_H
#define XYPLOTWIDGET_H

#include <QtCharts/QChartGlobal>
#include <QtCharts/QChartView>
#include <QtWidgets/QGraphicsWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QMainWindow>
#include "boost/shared_ptr.hpp"
#include "VppChartView.h"
#include "VppTabDockWidget.h"
#include "VPPXYChart.h"

QT_CHARTS_USE_NAMESPACE

/* Class defining a XY plot. It contains a VPPXYChart, which
 * is where the data to plot are actually set   */
class VPPXYChartWidget : public VppTabDockWidget
{
    Q_OBJECT

public:

	/// Explicit Constructor
	explicit VPPXYChartWidget(QMainWindow* parent= Q_NULLPTR, Qt::WindowFlags flags =0);

	/// Dtor
	~VPPXYChartWidget();

	/// Returns the chart view
	QChartView* getChartView() const;


private:

    /// Underlying chart. Row ptr, as apparently the ownership of
		/// this item is given to the view
    VPPXYChart* pChart_;

	/// Chart view
    boost::shared_ptr<VppChartView> pChartView_;

};

#endif // MAINWIDGET_H
