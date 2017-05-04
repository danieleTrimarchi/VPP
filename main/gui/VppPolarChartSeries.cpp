#include "VppPolarChartSeries.h"

// Explicit Ctor
VppPolarChartSeries::VppPolarChartSeries(
		QString name,
		QValueAxis* xAxis,
		QValueAxis* yAxis,
		QObject* parent /*= Q_NULLPTR*/) :
	QSplineSeries(parent){

	setName(name);
	setPointsVisible(true);
	attachAxis(xAxis);
	attachAxis(yAxis);

}

// Dtor
VppPolarChartSeries::~VppPolarChartSeries() {

}
