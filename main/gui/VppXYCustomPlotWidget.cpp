#include "VppXYCustomPlotWidget.h"

VppXYCustomPlotWidget::VppXYCustomPlotWidget(
		QString title, QString xAxisLabel, QString yAxisLabel,
		QWidget* parent/*=Q_NULLPTR*/) :
		QCustomPlot(parent) {

	// Set the title of this widget. This won't be shown in
	// the context of a multiplePlotWidget though.
	setObjectName(title);
	setWindowTitle(title);

  // add the text label at the top:
  QCPItemText *textLabel = new QCPItemText(this);
  textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
  textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
  textLabel->position->setCoords(0.5, 0); // place position at center/top of axis rect
  textLabel->setText(title);
  textLabel->setFont(QFont(font().family(), 10)); // make font a bit larger
  textLabel->setPen(QPen(Qt::black)); // show black border around text

	// Set the axis labels
	xAxis->setLabel(xAxisLabel);
	yAxis->setLabel(yAxisLabel);

	// Allow for dragging and zooming the plot and selecting the curves
  setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

}

// Dtor
VppXYCustomPlotWidget::~VppXYCustomPlotWidget() {
}

// Add some data to the plot
void VppXYCustomPlotWidget::addData(QVector<double>& x, QVector<double>& y,  QString dataLabel/*=""*/) {

	// Add one graph to the plot widget -- not sure if this should not go to the constructor..??
	addGraph();

	// Add the data to the very last graph, that we get with graph()
  graph()->setData(x, y);

  // Set the name of these data.
  graph()->setName(dataLabel);
}

// Override the parent class method called on double click
void VppXYCustomPlotWidget::mouseDoubleClickEvent(QMouseEvent* pMouseEvent) {

	// Launch a signal that will be caught by the MultiplePlotWidget
	requestFullScreen(this);

	// This method decorates the parent method
	QCustomPlot::mouseDoubleClickEvent(pMouseEvent);
}

