#include "VppPolarCustomPlotWidget.h"
#include "VppException.h"
#include "mathUtils.h"

VppPolarCustomPlotWidget::VppPolarCustomPlotWidget(
		QString title,
		QWidget* parent/*=Q_NULLPTR*/) :
		VppCustomPlotWidgetBase(title,QString(""),QString(""),parent) {

	// Allow for axes passing trough the centre
	connect(this, SIGNAL(beforeReplot()), this, SLOT(centreAxes()));

}

/// Add the circles that draw the coordinate system on the canvas. One circle per
/// integer value
void VppPolarCustomPlotWidget::addCircles() {

	// Set the pen for the circles : thin dotted lines
	QPen myPen;
	myPen.setStyle(Qt::PenStyle::DashDotDotLine);
	myPen.setWidth(0);

	// Number of circles to plot for all curves
	int numCircles=0;

	// Get the higher bound for the value (in terms of mag!) of this polar plot.
	// Loop on the graphs and search which one is selected
	for (int i=0; i<plottableCount(); ++i) {

		// Get a ptr to the i-th graph curve
		QCPAbstractPlottable* pPlottable = plottable(i);

		// Get the range for this graph curve
		bool found=false;
		QCPRange curveRange= pPlottable->getValueRange(found);

		// Get the current ranges of the x and y values
		double maxX = xAxis_->range().upper;
		double maxY = curveRange.upper;

		// Get the number of curves to plot from range for this curve with a c-style cast
		int curCircles= (int)sqrt( maxX*maxX + maxY*maxY );

		if(curCircles>numCircles)
			numCircles= curCircles;
	}

	// Now set some circles
	for(size_t i=0; i<numCircles+1; i++){
		QCPItemEllipse* circle = new QCPItemEllipse(this);
		double val= i;
		circle->topLeft->setCoords(-val,val);
		circle->bottomRight->setCoords(val,-val);
		circle->setPen(myPen);
	}

	// Set the axis range, based on the bounds
	float bound= numCircles + 0.1 * numCircles;
	xAxis_->setRange(-bound,bound);
	yAxis_->setRange(-bound,bound);

}

// Dtor
VppPolarCustomPlotWidget::~VppPolarCustomPlotWidget() {
}

// Make sure the x-y axes are visualized in the center of the plot
void VppPolarCustomPlotWidget::centreAxes() {

	int pxx= yAxis_->coordToPixel(0);
	int pyy= xAxis_->coordToPixel(0);
	xAxis_->setOffset(-axisRect()->height()-axisRect()->top()+pxx);
	yAxis_->setOffset(axisRect()->left()-pyy);

}

void VppPolarCustomPlotWidget::addData(QVector<double>& x, QVector<double>& y, QString dataLabel /*=""*/) {

	// Create empty curve objects:
	QCPCurve* pDataCurve = new QCPCurve(xAxis_, yAxis_);

	// Set the name of the curve (legend)
	pDataCurve->setName(dataLabel);

	// Generate the curve data points:
	QVector<QCPCurveData> polarData(x.size());
	for (int i=0; i<x.size(); ++i)
	  polarData[i] = QCPCurveData(i, x[i], y[i] );

	// Pass the data to the curves; we know t (i in loop above) is ascending,
	// so set alreadySorted=true (saves an extra internal sort):
	pDataCurve->data()->set(polarData, true);

	// Set the curve label
	pDataCurve->setName(dataLabel);

	// Color the curves:
	if (rand()%100 > 50)
		pDataCurve->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(rand()%14+1)));
	QPen graphPen;
	graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
	pDataCurve->setPen(graphPen);

	// Set some basic customPlot config:
	axisRect()->setupFullAxesBox();

	// Do not rescale axes yet, this will be done right at the end
	//rescaleAxes();

}

// Select a curve - in this case a QCPGraphs
void VppPolarCustomPlotWidget::select(QCPAbstractPlottable* pCurveToSelect) {

	QCPCurve* pCurve = qobject_cast<QCPCurve*>(pCurveToSelect);
	if(!pCurve)
		throw VPPException(HERE, "Could not cast to QCPGraph!");

	pCurve->setSelection(QCPDataSelection(pCurve->data()->dataRange()));

}

// Show the coordinates of a point - connected to mouseMoveEvent.
// Implemented of the advice given in: https://stackoverflow.com/questions/
// 18140446/display-the-plot-values-on-mouse-over-detect-scatter-points
void VppPolarCustomPlotWidget::showPointToolTip(QMouseEvent* event) {

	double x = this->xAxis_->pixelToCoord(event->pos().x());
  double y = this->yAxis_->pixelToCoord(event->pos().y());

  double angle = 90 - mathUtils::toDeg( atan2(y,x) );
  double mag = std::sqrt( x*x + y*y);

  setToolTip(QString("|%1|, %2º").arg(mag).arg(angle));

}
