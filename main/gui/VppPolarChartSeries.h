#ifndef VPP_POLAR_CHART_SERIES
#define VPP_POLAR_CHART_SERIES

#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE

class VppPolarChartSeries : public QSplineSeries {

		Q_OBJECT

	public:

		/// Explicit Ctor
		explicit VppPolarChartSeries(QString name, QValueAxis* xAxis, QValueAxis* yAxis, QObject *parent = Q_NULLPTR);

		/// Dtor
		~VppPolarChartSeries();

	private:


};
#endif
