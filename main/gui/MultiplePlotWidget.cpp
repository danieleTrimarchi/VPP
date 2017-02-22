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

	// Instantiate a ptr to temporarily store the adress of a chart to be assigned
	// to the grid layout, but also to be assigned to the chartList_. We iterate on
	// the chart list to define the actions used to expand one chart on double click.
	MultiplePlotChartComponent* pChart;

	pChart = new MultiplePlotChartComponent( new VPPXYChart );
	pGridLayout_->addWidget(pChart, 0, 0);
	chartList_ << pChart;

	pChart = new MultiplePlotChartComponent( new VppPolarChart );
	pGridLayout_->addWidget(pChart, 0, 1);
	chartList_ << pChart;

	pChart = new MultiplePlotChartComponent( new VppPolarChart );
	pGridLayout_->addWidget(pChart, 1, 0);
	chartList_ << pChart;

	pChart = new MultiplePlotChartComponent( new VPPXYChart );
	pGridLayout_->addWidget(pChart, 1, 1);
	chartList_ << pChart;

	setLayout(pGridLayout_);

	// Connect the widget to the function that handles the widget visibility in the layout
	foreach (MultiplePlotChartComponent* view, chartList_) {
		QObject::disconnect(view, SIGNAL(requestFullScreen(const MultiplePlotChartComponent*)), this, SLOT(toggleFullScreen(const MultiplePlotChartComponent*)));
		QObject::connect(view, SIGNAL(requestFullScreen(const MultiplePlotChartComponent*)), this, SLOT(toggleFullScreen(const MultiplePlotChartComponent*)));
	}

    // Set a minimum size for this widget
    QScreen* pScreen= QGuiApplication::primaryScreen();
	setMinimumSize(QSize(pScreen->size().width()/ 3, pScreen->size().height() / 3));

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



