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

// Get the max Y value of the series
qreal VppPolarChartSeries::getMaxYVal() const {

	qreal retVal=-1E20;
	for(size_t i=0; i<count(); i++){
		if(at(i).y()>retVal)
			retVal=at(i).y();
	}

	return retVal;
}
