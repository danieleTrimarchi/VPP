#include "PolarPlotWidget.h"
#include "math.h"
#include <QDebug>
#include <QScreen>
#include <QtGui>

PolarPlotWidget::PolarPlotWidget(QMainWindow* parent/*=Q_NULLPTR*/, Qt::WindowFlags flags/*=0*/) :
VppTabDockWidget(parent, flags) {

	setObjectName("PolarPlotView");
	setWindowTitle("PolarPlotView");

	// Instantiate the polar plot
	pPolarChart_= new VppPolarChart();
	pPolarChart_->plotPolars();

	// Instantiate a view for this chart. The view is to be visualized in the widget
	pChartWiew_.reset( new VppPolarChartView(pPolarChart_) );

	// Make the rendered lines look nicer
	pChartWiew_->setRenderHint(QPainter::Antialiasing);

	// Associate the polar plot view to this dockable widget
	setWidget(pChartWiew_.get());

	// Set a minimum size for this widget
	QScreen* pScreen= QGuiApplication::primaryScreen();
	setMinimumSize(QSize(pScreen->size().width()/ 3, pScreen->size().height() / 3));

}

// Virtual dtor
PolarPlotWidget::~PolarPlotWidget() {

}

// Returns the chart view
VppPolarChartView* PolarPlotWidget::getChartView() const {
	return pChartWiew_.get();
}

// Returns the polar chart
VppPolarChart* PolarPlotWidget::getPolarChart() const {
	return pPolarChart_;
}

