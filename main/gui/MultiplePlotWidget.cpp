#include "MultiplePlotWidget.h"
#include <QtCharts/QLegendMarker>
#include <QtCharts/QXYLegendMarker>
#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QScreen>
#include <QtGui>
#include "VppChartView.h"
#include "VppPolarChart.h"
#include "VPPXYChart.h"
#include "VppCustomPlotWidgetBase.h"

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
void MultiplePlotWidget::addChart(VppChartView* chartView, size_t px, size_t py) {

	pGridLayout_->addWidget(chartView, px, py);
	chartList_ << boost::shared_ptr<VppChartView>(chartView);

	// Connect the charts in the multiple plot in order to being able to
	// magnify one chart on double click
	connectFullScreenSignals();

}

// Add a chart in a given position
void MultiplePlotWidget::addChart(VppCustomPlotWidgetBase* chart, size_t px, size_t py) {

	// Add the widget to the multiple plot
	pGridLayout_->addWidget(chart, px, py);
	customPlotList_ << boost::shared_ptr<VppCustomPlotWidgetBase>(chart);

	// Connect the charts in the multiple plot in order to being able to
	// magnify one chart on double click
	connectFullScreenSignals();

}

// How many columns is this multiplePlotWidget made of?
int MultiplePlotWidget::columnCount() const {
	return pGridLayout_->columnCount();
}

// How many rows is this multiplePlotWidget made of?
int MultiplePlotWidget::rowCount() const {
	return pGridLayout_->rowCount();
}

// Connect all the charts in the multiplot to the toggleFullScreen
// signal that is used to expand one chart to full screen
void MultiplePlotWidget::connectFullScreenSignals() {

	// Connect the widget to the function that handles the widget visibility in the layout
	foreach (boost::shared_ptr<VppChartView> pChart, chartList_) {
		QObject::disconnect(pChart.get(), SIGNAL(requestFullScreen(const VppChartView*)), this, SLOT(toggleFullScreen(const VppChartView*)));
		QObject::connect(pChart.get(), SIGNAL(requestFullScreen(const VppChartView*)), this, SLOT(toggleFullScreen(const VppChartView*)));
	}

	// Connect the widget to the function that handles the widget visibility in the layout
	foreach (boost::shared_ptr<VppCustomPlotWidgetBase> pChart, customPlotList_) {
		QObject::disconnect(pChart.get(), SIGNAL(requestFullScreen(const VppCustomPlotWidgetBase*)), this, SLOT(toggleFullScreen(const VppCustomPlotWidgetBase*)));
		QObject::connect(pChart.get(), SIGNAL(requestFullScreen(const VppCustomPlotWidgetBase*)), this, SLOT(toggleFullScreen(const VppCustomPlotWidgetBase*)));
	}
}


// What to do when one of the plots is clicked?
void MultiplePlotWidget::toggleFullScreen(const VppChartView* pClickedView) {

	// Loop on the list of chart view and reverse the visibility of all,
	// except the one that was clicked
	foreach (boost::shared_ptr<VppChartView> pView, chartList_) {

		if(pView.get()!=pClickedView)
			pView->setVisible( !pView->isVisible() );
	}
}

// What to do when one of the plots is clicked?
void MultiplePlotWidget::toggleFullScreen(const VppCustomPlotWidgetBase* pClickedView) {

	// Loop on the list of chart view and reverse the visibility of all,
	// except the one that was clicked
	foreach (boost::shared_ptr<VppCustomPlotWidgetBase> pChart, customPlotList_) {

		if(pChart.get()!=pClickedView)
			pChart->setVisible( !pChart->isVisible() );
	}

}

// Dtor
MultiplePlotWidget::~MultiplePlotWidget() {
}



