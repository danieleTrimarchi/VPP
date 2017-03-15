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
  setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes );

  // Connect slot that ties some axis selections together (especially opposite axes):
  connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

  // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
  connect(this, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
  connect(this, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

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

// Normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
// the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
// and the axis base line together. However, the axis label shall be selectable individually.
//
// The selection state of the left and right axes shall be synchronized as well as the state of the
// bottom and top axes.
//
// Further, we want to synchronize the selection of the graphs with the selection state of the respective
// legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
// or on its legend item.
void VppXYCustomPlotWidget::selectionChanged() {

  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (xAxis->selectedParts().testFlag(QCPAxis::spAxis) || xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
     xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)) {
    xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (yAxis->selectedParts().testFlag(QCPAxis::spAxis) || yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)) {
    yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
}

// If an axis is selected, only allow the direction of that axis to be dragged
// If no axis is selected, both directions may be dragged
void VppXYCustomPlotWidget::mousePress() {

  if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    axisRect()->setRangeDrag(xAxis->orientation());
  else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    axisRect()->setRangeDrag(yAxis->orientation());
  else
    axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

// if an axis is selected, only allow the direction of that axis to be zoomed
// If no axis is selected, both directions may be zoomed
void VppXYCustomPlotWidget::mouseWheel() {

  if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    axisRect()->setRangeZoom(xAxis->orientation());
  else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    axisRect()->setRangeZoom(yAxis->orientation());
  else
    axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

