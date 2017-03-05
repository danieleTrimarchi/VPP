#ifndef VPP_CUSTOMPLOT_WIDGET_H
#define VPP_CUSTOMPLOT_WIDGET_H

#include <QMainWindow>
#include "boost/shared_ptr.hpp"
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

//	/// Returns the chart view
//	QChartView* getChartView() const;


private:

    /// Underlying chart. Row ptr, as apparently the ownership of
		/// this item is given to the view
    //VPPXYChart* pChart_;

	/// Chart view
    //boost::shared_ptr<VppChartView> pChartView_;

};

#endif // MAINWIDGET_H
