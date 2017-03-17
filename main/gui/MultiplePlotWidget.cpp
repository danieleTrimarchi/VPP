#include "MultiplePlotWidget.h"
#include <QtCharts/QLegendMarker>
#include <QtCharts/QXYLegendMarker>
#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QScreen>
#include <QtGui>
#include "MultiplePlotChartComponent.h"
#include "VppPolarChart.h"
#include "VPPXYChart.h"
#include "VppXYCustomPlotWidget.h"

MultiplePlotWidget::MultiplePlotWidget(QMainWindow* parent/*=Q_NULLPTR*/, QString title /*=0*/) :
VppTabDockWidget(parent) {

	setObjectName(title);
	setWindowTitle(title);

	// Create an empty widget that will hold the multiplot
	QWidget* widget = new QWidget;

	// Set this widget to the dockable widget
	setWidget(widget);

	// create layout as a child of the widget
	pGridLayout_= new QGridLayout(widget);

	// Attempt maximizing the plotting area
	pGridLayout_->setContentsMargins(0,0,0,0);
	pGridLayout_->setHorizontalSpacing(0);
	pGridLayout_->setVerticalSpacing(0);

	setLayout(pGridLayout_);

	// Set a minimum size for this widget
	QScreen* pScreen= QGuiApplication::primaryScreen();
	setMinimumSize(QSize(pScreen->size().width()/ 2, pScreen->size().height() / 2));

}

// Add a chart in a given position
void MultiplePlotWidget::addChart(QChart& chart, size_t px, size_t py) {

	MultiplePlotChartComponent* pChart = new MultiplePlotChartComponent( &chart );
	pGridLayout_->addWidget(pChart, px, py);
	chartList_ << boost::shared_ptr<MultiplePlotChartComponent>(pChart);

	// Connect the charts in the multiple plot in order to being able to
	// magnify one chart on double click
	connectFullScreenSignals();

}

// Add a chart in a given position
void MultiplePlotWidget::addChart(VppXYCustomPlotWidget* chart, size_t px, size_t py) {

	// Add the widget to the multiple plot
	pGridLayout_->addWidget(chart, px, py);
	customPlotList_ << boost::shared_ptr<VppXYCustomPlotWidget>(chart);

	// Connect the charts in the multiple plot in order to being able to
	// magnify one chart on double click
	connectFullScreenSignals();

}

// Connect all the charts in the multiplot to the toggleFullScreen
// signal that is used to expand one chart to full screen
void MultiplePlotWidget::connectFullScreenSignals() {

	// Connect the widget to the function that handles the widget visibility in the layout
	foreach (boost::shared_ptr<MultiplePlotChartComponent> pChart, chartList_) {
		QObject::disconnect(pChart.get(), SIGNAL(requestFullScreen(const MultiplePlotChartComponent*)), this, SLOT(toggleFullScreen(const MultiplePlotChartComponent*)));
		QObject::connect(pChart.get(), SIGNAL(requestFullScreen(const MultiplePlotChartComponent*)), this, SLOT(toggleFullScreen(const MultiplePlotChartComponent*)));
	}

	// Connect the widget to the function that handles the widget visibility in the layout
	foreach (boost::shared_ptr<VppXYCustomPlotWidget> pChart, customPlotList_) {
		QObject::disconnect(pChart.get(), SIGNAL(requestFullScreen(const VppXYCustomPlotWidget*)), this, SLOT(toggleFullScreen(const VppXYCustomPlotWidget*)));
		QObject::connect(pChart.get(), SIGNAL(requestFullScreen(const VppXYCustomPlotWidget*)), this, SLOT(toggleFullScreen(const VppXYCustomPlotWidget*)));
	}
}


// What to do when one of the plots is clicked?
void MultiplePlotWidget::toggleFullScreen(const MultiplePlotChartComponent* pClickedView) {

	// Loop on the list of chart view and reverse the visibility of all,
	// except the one that was clicked
	foreach (boost::shared_ptr<MultiplePlotChartComponent> pView, chartList_) {

		if(pView.get()!=pClickedView)
			pView->setVisible( !pView->isVisible() );
	}
}

// What to do when one of the plots is clicked?
void MultiplePlotWidget::toggleFullScreen(const VppXYCustomPlotWidget* pClickedView) {

	// Loop on the list of chart view and reverse the visibility of all,
	// except the one that was clicked
	foreach (boost::shared_ptr<VppXYCustomPlotWidget> pChart, customPlotList_) {

		if(pChart.get()!=pClickedView)
			pChart->setVisible( !pChart->isVisible() );
	}

}

// Dtor
MultiplePlotWidget::~MultiplePlotWidget() {
}



