#include "VppPolarChartSeries.h"

// Explicit Ctor
VppPolarChartSeries::VppPolarChartSeries(
		QString name,
		QObject* parent /*= Q_NULLPTR*/) :
	QSplineSeries(parent){

	setName(name);
	setPointsVisible(true);
	//setPointLabelsVisible(true);

}

// Dtor
VppPolarChartSeries::~VppPolarChartSeries() {

}
