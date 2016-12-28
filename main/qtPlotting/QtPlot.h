#ifndef QT_PLOT_H
#define QT_PLOT_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

// Add support for (log) axes definition
//#include <QtCharts/QValueAxis>
//#include <QtCharts/QLogValueAxis>

QT_CHARTS_USE_NAMESPACE

class QtPlot {

	public:

		/// Ctor
		QtPlot(int argc, char** argv);

		/// Dtor
		~QtPlot();

	private:

};

#endif
