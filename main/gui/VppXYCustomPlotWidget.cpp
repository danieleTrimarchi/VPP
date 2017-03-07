#include "VppXYCustomPlotWidget.h"

VppXYCustomPlotWidget::VppXYCustomPlotWidget(
		QString title, QString xAxisLabel, QString yAxisLabel,
		QWidget* parent/*=Q_NULLPTR*/) :
		QCustomPlot(parent) {

	// Set the title of this widget
	setObjectName(title);
	setWindowTitle(title);

	// Set the axis labels
	xAxis->setLabel(xAxisLabel);
	yAxis->setLabel(yAxisLabel);

	// Set a minimum size for this widget
	QScreen* pScreen= QGuiApplication::primaryScreen();
	setMinimumSize(QSize(pScreen->size().width()/ 3, pScreen->size().height() / 3));

}

// Dtor
VppXYCustomPlotWidget::~VppXYCustomPlotWidget() {
}

// Add some data to the plot
void VppXYCustomPlotWidget::addData(QVector<double>& x, QVector<double>& y) {

	// Add one graph to the plot widget -- not sure if this should not go to the constructor..??
	addGraph();

	// Add the data to the very last graph, that we get with graph()
  graph()->setData(x, y);

}

// Set the bounds for this plot
void VppXYCustomPlotWidget::setBounds(double minX, double maxX, double minY, double maxY ) {

	xAxis->setRange(minX,maxX);
  yAxis->setRange(minY,maxY);

}

// Override the parent class method called on double click
void VppXYCustomPlotWidget::mouseDoubleClickEvent(QMouseEvent* pMouseEvent) {

	// Launch a signal that will be caught by the MultiplePlotWidget
	requestFullScreen(this);

	// This method decorates the parent method
	QCustomPlot::mouseDoubleClickEvent(pMouseEvent);
}

