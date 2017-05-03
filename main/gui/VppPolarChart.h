#ifndef VPPPOLARPLOT_H
#define VPPPOLARPLOT_H

#include <QtCharts/QChart>
#include <QtCharts/QPolarChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QLegend>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QXYLegendMarker>
#include "boost/shared_ptr.hpp"

QT_CHARTS_USE_NAMESPACE

class QGestureEvent;

class VppPolarChart : public QPolarChart {

	Q_OBJECT

public:

	/// Ctor
	VppPolarChart();

	/// Dtor
	~VppPolarChart();

	/// Add curves to the plot
	void plotPolars();

public slots:

	/// Allow for switching on-off the curves on click
	void connectMarkers();

	void disconnectMarkers();

	/// What to do on legend marker click?
	void handleMarkerClicked();

protected:

	bool sceneEvent(QEvent *event);

	private:

	bool gestureEvent(QGestureEvent *event);

	// Polar chart axes
	boost::shared_ptr<QValueAxis> pAngularAxis_, pRadialAxis_;

	const size_t fontSize_;
};

#endif
