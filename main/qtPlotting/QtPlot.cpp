#include "QtPlot.h"

// Ctor
QtPlot::QtPlot(int argc, char** argv) {

	// Declare the args for the constructor of the Qapplication
  QApplication a(argc, argv);

  QLineSeries *series = new QLineSeries();

  // First way to add points to the plot, using the append method
  series->append(0, 6);
  series->append(2, 4);
  series->append(3, 8);
  series->append(7, 4);
  series->append(10, 5);

  // Second way to add points to the plot, using the operator <<
  *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

  series->setName("myLine");

  QChart *chart = new QChart();
  chart->legend()->show();
  chart->addSeries(series);
  chart->setTitle("Simple line chart example");

  // Create default axes...
  chart->createDefaultAxes();

//  // ...or, set the y-axis to logarithmic
//  QValueAxis *axisX = new QValueAxis;
//  axisX->setTitleText("Data point");
//  axisX->setTickCount(6);
//  axisX->setLabelFormat("%i");
//  chart->addAxis(axisX, Qt::AlignBottom);
//  series->attachAxis(axisX);
//
//  QLogValueAxis *axisY = new QLogValueAxis;
//  axisY->setLabelFormat("%g");
//  axisY->setTitleText("Values");
//  axisY->setBase(8);
//  chart->addAxis(axisY, Qt::AlignLeft);
//  series->attachAxis(axisY);

  // --

  QChartView *chartView = new QChartView(chart);

  // Make the rendered lines look nicer
  chartView->setRenderHint(QPainter::Antialiasing);

  QMainWindow window;
  window.setCentralWidget(chartView);
  window.resize(400, 300);
  window.show();

  a.exec();

}

// Dtor
QtPlot::~QtPlot() {

}
