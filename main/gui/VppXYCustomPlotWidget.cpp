#include "VppXYCustomPlotWidget.h"
#include "VPPException.h"

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
			QCP::iSelectAxes
	);

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

// Show the coordinates of a point - connected to mouseMoveEvent.
// Implemented of the advice given in: https://stackoverflow.com/questions/
// 18140446/display-the-plot-values-on-mouse-over-detect-scatter-points
void VppXYCustomPlotWidget::showPointToolTip(QMouseEvent* event) {

	float x = this->xAxis_->pixelToCoord(event->pos().x());
  float y = this->yAxis_->pixelToCoord(event->pos().y());

  setToolTip(QString("%1 , %2").arg(x).arg(y));

}

