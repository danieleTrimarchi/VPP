#include "VppCustomPlotWidgetBase.h"
#include <iostream>

VppCustomPlotWidgetBase::VppCustomPlotWidgetBase(
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

	// Show the legend
	legend->setVisible(true);
	QFont legendFont = font();
	legendFont.setPointSize(7);
	legend->setFont(legendFont);
	legend->setSelectedFont(legendFont);
	legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items
	legend->setIconSize(15,5);


	// Set the axis labels and the font
	legendFont.setPointSize(9);
	xAxis_->setLabel(xAxisLabel);
	yAxis_->setLabel(yAxisLabel);
	xAxis_->setLabelFont(legendFont);
	yAxis_->setLabelFont(legendFont);

	xAxis_->setTickLabelFont(legendFont);
	yAxis_->setTickLabelFont(legendFont);

	// Allow for dragging and zooming the plot and selecting the curves
	setInteractions(
			QCP::iRangeDrag |
			QCP::iRangeZoom |
			QCP::iSelectPlottables |
			QCP::iSelectLegend |
			QCP::iSelectAxes
	);

	// Connect slot that ties some axis selections together (especially opposite axes):
	connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

	// connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
	connect(this, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
	connect(this, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

}

// Dtor
VppCustomPlotWidgetBase::~VppCustomPlotWidgetBase() {
}

// Add some data to the plot
void VppCustomPlotWidgetBase::addData(QVector<double>& x, QVector<double>& y,  QString dataLabel/*=""*/) {

	// Add one graph to the plot widget -- not sure if this should not go to the constructor..??
	addGraph();

	// Add the data to the very last graph, that we get with graph()
	graph()->setData(x, y);

	// Set the name of these data.
	graph()->setName(dataLabel);

	// Set a random line-style, and pen
	//graph()->setLineStyle((QCPGraph::LineStyle)(rand()%5+1)); commented to keep only continuous lines
	if (rand()%100 > 50)
		graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(rand()%14+1)));
	QPen graphPen;
	graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
	//graphPen.setWidthF(rand()/(double)RAND_MAX*2+1); commented to keep lines thin
	graph()->setPen(graphPen);

}

/// Add some quivers to the plots
void VppCustomPlotWidgetBase::addQuivers(QVector<double>& x, QVector<double>& y, QVector<double>& dx, QVector<double>& dy ) {

	// Add the arrows now
	for(size_t i=0; i<x.size();i++){

			// Instantiate the quiver and set its style
			QCPItemLine* arrow = new QCPItemLine(this);
			arrow->setHead(QCPLineEnding::esLineArrow);

			// Set the coordinates of the quiver
			arrow->start->setCoords(x[i], y[i]);
			arrow->end->setCoords(x[i]+dx[i], y[i]+dy[i]);
	}
}

// Override the parent class method called on double click
void VppCustomPlotWidgetBase::mouseDoubleClickEvent(QMouseEvent* pMouseEvent) {

	// Launch a signal that will be caught by the MultiplePlotWidget
	requestFullScreen(this);

	// This method decorates the parent method
	QCustomPlot::mouseDoubleClickEvent(pMouseEvent);
}

// Overrides the key actions
void VppCustomPlotWidgetBase::keyPressEvent(QKeyEvent *event) {

	switch (event->key()) {
		//  case Qt::Key_Plus:
		//      chart()->zoomIn();
		//      break;
		//  case Qt::Key_Minus:
		//      chart()->zoomOut();
		//      break;
		//  case Qt::Key_Left:
		//      chart()->scroll(-1.0, 0);
		//      break;
		//  case Qt::Key_Right:
		//      chart()->scroll(1.0, 0);
		//      break;
		case Qt::Key_Up:
			changeGraphSelection(Qt::Key_Up);
			break;
		case Qt::Key_Down:
			changeGraphSelection(Qt::Key_Down);
			break;
			//    case Qt::Key_Space:
			//        switchChartType();
			//        break;
		default:
			QWidget::keyPressEvent(event);
			break;
	}
}

void VppCustomPlotWidgetBase::changeGraphSelection(int key){

	// Loop on the graphs and search which one is selected
	for (int i=0; i<graphCount(); ++i) {

		// Get a ptr to the i-th graph curve
		QCPGraph* pGraph = graph(i);
		// Get a ptr to the legend item related to this graph curve
		QCPPlottableLegendItem* item = legend->itemWithPlottable(pGraph);

		// If the curve OR the legend item have been selected, highlight them both
		if (item->selected() || pGraph->selected()) {

			// Deselect the current legend item
			item->setSelected(false);
			pGraph->setSelection(QCPDataSelection());

			// todo dtrimarchi :
			// 2_ make sure the value of k loops and does not go out of bounds!
			int k=0;
			if(key==Qt::Key_Up)
				i==0 ? k=graphCount()-1 : k=i-1;

			else if(key==Qt::Key_Down)
				i==graphCount()-1 ? k=0 : k=i+1;

			// Get the
			QCPGraph* pGraphToSelect = graph(k);
			QCPPlottableLegendItem* itemToSelect = legend->itemWithPlottable(pGraphToSelect);

			itemToSelect->setSelected(true);
			pGraphToSelect->setSelection(QCPDataSelection(pGraphToSelect->data()->dataRange()));

			// Refresh the plot and visualize the new selection
			replot();
            
			// Break, otherwise we keep looping and selecting items
			break;

		}
	}
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
void VppCustomPlotWidgetBase::selectionChanged() {

	// make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
	if (xAxis_->selectedParts().testFlag(QCPAxis::spAxis) || xAxis_->selectedParts().testFlag(QCPAxis::spTickLabels) ||
			xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)) {
		xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
		xAxis_->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
	}

	// make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
	if (yAxis_->selectedParts().testFlag(QCPAxis::spAxis) || yAxis_->selectedParts().testFlag(QCPAxis::spTickLabels) ||
			yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)) {
		yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
		yAxis_->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
	}

	// Synchronize selection of graphs with selection of corresponding legend items:
	for (int i=0; i<plottableCount(); ++i) {

		// Get a ptr to the i-th plottable pLegendItem
		QCPAbstractPlottable* pPlottable = plottable(i);

		// Get a ptr to the legend item related to this graph curve
		QCPPlottableLegendItem* pLegendItem = legend->itemWithPlottable(pPlottable);

		// If the curve OR the legend item have been selected, highlight them both
		if (pLegendItem->selected() || pPlottable->selected()) {
			pLegendItem->setSelected(true);
			pPlottable->setSelection( pPlottable->selection() );
		}
	}
}

// If an axis is selected, only allow the direction of that axis to be dragged
// If no axis is selected, both directions may be dragged
void VppCustomPlotWidgetBase::mousePress() {

	if (xAxis_->selectedParts().testFlag(QCPAxis::spAxis))
		axisRect()->setRangeDrag(xAxis_->orientation());
	else if (yAxis_->selectedParts().testFlag(QCPAxis::spAxis))
		axisRect()->setRangeDrag(yAxis_->orientation());
	else
		axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

// if an axis is selected, only allow the direction of that axis to be zoomed
// If no axis is selected, both directions may be zoomed
void VppCustomPlotWidgetBase::mouseWheel() {

	if (xAxis_->selectedParts().testFlag(QCPAxis::spAxis))
		axisRect()->setRangeZoom(xAxis_->orientation());
	else if (yAxis_->selectedParts().testFlag(QCPAxis::spAxis))
		axisRect()->setRangeZoom(yAxis_->orientation());
	else
		axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

