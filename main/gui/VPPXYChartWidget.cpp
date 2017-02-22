#include <QtCharts/QChartView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtCore/QDebug>
#include <QtWidgets/QFormLayout>
#include <QtCharts/QXYLegendMarker>
#include "iostream"
#include <QScreen>
#include <QtGui>

#include "VPPXYChartWidget.h"

QT_CHARTS_USE_NAMESPACE

VPPXYChartWidget::VPPXYChartWidget(QMainWindow* parent/*=Q_NULLPTR*/, Qt::WindowFlags flags/*=0*/) :
	VppTabDockWidget(parent, flags) {

    setObjectName("XYPlotView");
    setWindowTitle("XYPlotView");

    // Instantiate the XY plot
    pChart_= new VPPXYChart;

    // Instantiate a view for this chart. The view is to be visualized in the widget
    pChartView_.reset(new VppChartView(pChart_));

    // Improve the view of this plot using antialiasing
    pChartView_->setRenderHint(QPainter::Antialiasing);

    // Set the chart view to be visualized in this dockable widget
    setWidget(pChartView_.get());

    // Set a minimum size for this widget
    QScreen* pScreen= QGuiApplication::primaryScreen();
    setMinimumSize(QSize(pScreen->size().width()/ 3, pScreen->size().height() / 3));

}

// Dtor
VPPXYChartWidget::~VPPXYChartWidget() {
}

// Returns the chart view
QChartView* VPPXYChartWidget::getChartView() const {
	return pChartView_.get();
}
