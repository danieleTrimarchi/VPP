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

MultiplePlotWidget::MultiplePlotWidget(QMainWindow* parent/*=Q_NULLPTR*/, Qt::WindowFlags flags/*=0*/) :
	VppTabDockWidget(parent, flags) {

	setObjectName("MultiplePlotView");
	setWindowTitle("MultiplePlotView");

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
  setMinimumSize(QSize(pScreen->size().width()/ 3, pScreen->size().height() / 3));

}

// Connect all the charts in the multiplot to the toggleFullScreen
// signal that is used to expand one chart to full screen
void MultiplePlotWidget::connectFullScreenSignals() {

	// Connect the widget to the function that handles the widget visibility in the layout
	foreach (MultiplePlotChartComponent* chart, chartList_) {
		QObject::disconnect(chart, SIGNAL(requestFullScreen(const MultiplePlotChartComponent*)), this, SLOT(toggleFullScreen(const MultiplePlotChartComponent*)));
		QObject::connect(chart, SIGNAL(requestFullScreen(const MultiplePlotChartComponent*)), this, SLOT(toggleFullScreen(const MultiplePlotChartComponent*)));
	}

}

// Add a chart in a given position
void MultiplePlotWidget::addChart(QChart& chart, size_t px, size_t py) {

	MultiplePlotChartComponent* pChart = new MultiplePlotChartComponent( &chart );
	pGridLayout_->addWidget(pChart, 0, 0);
	chartList_ << pChart;

	// Connect the charts in the multiple plot in order to being able to
	// magnify one chart on double click
	connectFullScreenSignals();

}


// What to do when one of the plots is clicked?
void MultiplePlotWidget::toggleFullScreen(const MultiplePlotChartComponent* pClickedView) {

	// Loop on the list of chart view and reverse the visibility of all,
	// except the one that was clicked
	foreach (MultiplePlotChartComponent* view, chartList_) {

		if(view!=pClickedView)
			view->setVisible( !view->isVisible() );
	}
}

// Dtor
MultiplePlotWidget::~MultiplePlotWidget() {
}



