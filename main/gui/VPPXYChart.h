#ifndef XYCHART_H
#define XYCHART_H

#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include "iostream"

QT_CHARTS_USE_NAMESPACE

class VPPXYChart : public QChart {

	Q_OBJECT

public :

	/// Explicit Constructor
	explicit VPPXYChart(QGraphicsItem* parent= Q_NULLPTR, Qt::WindowFlags wFlags= Qt::WindowFlags());

	/// Dtor
	~VPPXYChart();

public slots:

	/// Add a default test series
	void addSeries();

	/// Remove a curve from the plot
	void removeSeries();

    /// Connect the signal issued when a marker is clicked to the slot handleMarkerClicked
    void connectMarkers();

	/// Disconnect the signal issued when a marker is clicked from the slot handleMarkerClicked
	void disconnectMarkers();

	/// What to do when a legend marker is clicked?
	void handleMarkerClicked();

private:

	/// Container that stores the data to visualize in the chart
	QList<QLineSeries*> series_;

};


#endif
