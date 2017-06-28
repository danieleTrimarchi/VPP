#include "VppXYCustomPlotWidget.h"
#include "VPPException.h"
#include <iostream>

VppXYCustomPlotWidget::VppXYCustomPlotWidget(
		QString title, QString xAxisLabel, QString yAxisLabel,
		QWidget* parent/*=Q_NULLPTR*/) :
		VppCustomPlotWidgetBase(title,xAxisLabel,yAxisLabel,parent) {

	// Allow for dragging and zooming the plot and selecting the curves
	setInteractions(
			QCP::iRangeDrag |
			QCP::iRangeZoom |
			QCP::iSelectPlottables |
			QCP::iSelectLegend |
			QCP::iSelectAxes |
			QCP::iMultiSelect
	);

	setMultiSelectModifier(Qt::AltModifier);

}

// Dtor
VppXYCustomPlotWidget::~VppXYCustomPlotWidget() {
}

// Select a curve - in this case a QCPGraphs
void VppXYCustomPlotWidget::select(QCPAbstractPlottable* pGraphToSelect) {

	QCPGraph* pGraph = qobject_cast<QCPGraph*>(pGraphToSelect);
	if(!pGraph)
		throw VPPException(HERE, "Could not cast to QCPGraph!");

	pGraph->setSelection(QCPDataSelection(pGraph->data()->dataRange()));

}

// Hide a selected curve - in this case a QCPGraphs
void VppXYCustomPlotWidget::toggleSelected() {

	QList<QCPAbstractPlottable*> selectedPlottableList= selectedPlottables();

	for(QList<QCPAbstractPlottable*>::iterator it=selectedPlottableList.begin();
			it!=selectedPlottableList.end(); ++it){

		QCPAbstractPlottable* pSelectedPlottable = *it;

		QCPGraph* pGraph = qobject_cast<QCPGraph*>(pSelectedPlottable);
		if(!pGraph)
			throw VPPException(HERE, "Could not cast this plottable to QCPGraph!");

		// Get a handle to the legend and modify it
		QCPPlottableLegendItem* legendItemToGrayOut = legend->itemWithPlottable(pGraph);

		// Declare a color
		QColor color;

		// The curve is visible : hide it
		if(pGraph->visible()){

			// Hide the curve
			pGraph->setVisible(false);

			// Write the legend in red
			color.setRgb(255,0,0);
			legendItemToGrayOut->setTextColor(color);

		} else {
			// The curve is hidden : show it
			pGraph->setVisible(true);

			// Restore the color of the legend to black
			color.setRgb(0,0,0);
			legendItemToGrayOut->setTextColor(color);

		}
	}

	replot();

}

// Show the coordinates of a point - connected to mouseMoveEvent.
// Implemented of the advice given in: https://stackoverflow.com/questions/
// 18140446/display-the-plot-values-on-mouse-over-detect-scatter-points
void VppXYCustomPlotWidget::showPointToolTip(QMouseEvent* event) {

	float x = this->xAxis_->pixelToCoord(event->pos().x());
  float y = this->yAxis_->pixelToCoord(event->pos().y());

  setToolTip(QString("%1 , %2").arg(x).arg(y));

}
