#include "VppXYCustomPlotWidget.h"

VppXYCustomPlotWidget::VppXYCustomPlotWidget(
		QString title, QString xAxisLabel, QString yAxisLabel,
		QWidget* parent/*=Q_NULLPTR*/) :
		VppCustomPlotWidgetBase(title,xAxisLabel,yAxisLabel,parent) {
}

// Dtor
VppXYCustomPlotWidget::~VppXYCustomPlotWidget() {
}


