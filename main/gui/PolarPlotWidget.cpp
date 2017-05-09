#include "PolarPlotWidget.h"
#include "math.h"
#include <QDebug>
#include <QScreen>
#include <QtGui>

PolarPlotWidget::PolarPlotWidget(QString title, QMainWindow* parent/*=Q_NULLPTR*/, Qt::WindowFlags flags/*=0*/) :
VppTabDockWidget(parent, flags) {

	setObjectName(title);
	setWindowTitle(title);

	// Instantiate the polar plot
	pPolarChart_.reset( new VppPolarChart );

	// Instantiate a view for this chart. The view is to be visualized in the widget
	pChartWiew_.reset( new VppPolarChartView(pPolarChart_.get()) );

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
VppPolarChartView* PolarPlotWidget::getView() const {
	return pChartWiew_.get();
}

// Returns the polar chart
VppPolarChart* PolarPlotWidget::chart() const {
	return pPolarChart_.get();
}

