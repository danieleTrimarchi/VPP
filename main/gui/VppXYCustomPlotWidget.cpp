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

	// Allow for dragging and zooming the plot
  setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

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
void VppXYCustomPlotWidget::setBounds(double min, double max) {

	// Set the min and max of the x-axis as requested
	xAxis->setRange(min,max);

	// For the y-vals, let's search the right values

	// Set min and max values
	double minY= 1e+20, maxY= -1E+20;

	// Loop on the graphs
	for(int iGraph=0; iGraph<graphCount(); iGraph++){

		// get the i-th graph
	  QVector<QCPGraphData>::iterator it;
		for(	it=graph(iGraph)->data()->begin(); it != graph(iGraph)->data()->end(); it++ ){
	  	if (it->value > maxY )
	  		maxY = it->value;
	  	if (it->value < minY )
	  		minY = it->value;
		}
	}

	//xAxis->setRange(minX,maxX);
  // Increment the bounds of 10%
  double delta= 0.05 * fabs( maxY-minY );
	yAxis->setRange(minY-delta,maxY+delta);

}

// Override the parent class method called on double click
void VppXYCustomPlotWidget::mouseDoubleClickEvent(QMouseEvent* pMouseEvent) {

	// Launch a signal that will be caught by the MultiplePlotWidget
	requestFullScreen(this);

	// This method decorates the parent method
	QCustomPlot::mouseDoubleClickEvent(pMouseEvent);
}

