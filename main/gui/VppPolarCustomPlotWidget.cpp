#include "VppPolarCustomPlotWidget.h"

VppPolarCustomPlotWidget::VppPolarCustomPlotWidget(
		QString title,
		QWidget* parent/*=Q_NULLPTR*/) :
		VppCustomPlotWidgetBase(title,QString(""),QString(""),parent),
		numCircles_(6) {

	// Set the pen for the circles : thin dotted lines
	QPen myPen;
	myPen.setStyle(Qt::PenStyle::DashDotDotLine);
	myPen.setWidth(0);

	// Now set some circles
	for(size_t i=0; i<numCircles_+1; i++){
		QCPItemEllipse* circle = new QCPItemEllipse(this);
		double val= i;
		circle->topLeft->setCoords(-val,val);
		circle->bottomRight->setCoords(val,-val);
		circle->setPen(myPen);
	}

	// Set the axis range
	float bound= numCircles_ + 0.1 * numCircles_;
	xAxis->setRange(-bound,bound);
	yAxis->setRange(-bound,bound);

	// Allow for axes passing trough the centre
	connect(this, SIGNAL(beforeReplot()), this, SLOT(centreAxes()));

}

// Dtor
VppPolarCustomPlotWidget::~VppPolarCustomPlotWidget() {
}

// Make sure the x-y axes are visualized in the center of the plot
void VppPolarCustomPlotWidget::centreAxes() {

	int pxx= yAxis->coordToPixel(0);
	int pyy= xAxis->coordToPixel(0);
	xAxis->setOffset(-axisRect()->height()-axisRect()->top()+pxx);
	yAxis->setOffset(axisRect()->left()-pyy);

}

