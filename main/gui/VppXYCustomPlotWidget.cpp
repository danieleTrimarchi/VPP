#include "VppXYCustomPlotWidget.h"
#include "VPPException.h"

VppXYCustomPlotWidget::VppXYCustomPlotWidget(
		QString title, QString xAxisLabel, QString yAxisLabel,
		QWidget* parent/*=Q_NULLPTR*/) :
		VppCustomPlotWidgetBase(title,xAxisLabel,yAxisLabel,parent) {
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

