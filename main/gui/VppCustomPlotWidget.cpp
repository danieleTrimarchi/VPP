#include "VppCustomPlotWidget.h"
#include <QApplication>
#include <QScreen>
#include <qboxlayout.h>
#include "qcustomplot.h"

VppCustomPlotWidget::VppCustomPlotWidget(QMainWindow* parent/*=Q_NULLPTR*/, Qt::WindowFlags flags/*=0*/) :
	VppTabDockWidget(parent, flags) {

    setObjectName("VPPCustomPlotView");
    QString demoName = "Quadratic Demo";
    setWindowTitle("QCustomPlot: " + demoName);

    // Instantiate a QCustomPlot
    QCustomPlot * pCustomPlot = new QCustomPlot(this);

    // -- QCustomPlot setup --
    pCustomPlot->setObjectName(QStringLiteral("pCustomPlot"));

    // generate some data:
    QVector<double> x(101), y(101); // initialize with entries 0..100
    for (int i=0; i<101; ++i)
    {
      x[i] = i/50.0 - 1; // x goes from -1 to 1
      y[i] = x[i]*x[i];  // let's plot a quadratic function
    }
    // create graph and assign data to it:
    pCustomPlot->addGraph();
    pCustomPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    pCustomPlot->xAxis->setLabel("x");
    pCustomPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    pCustomPlot->xAxis->setRange(-1, 1);
    pCustomPlot->yAxis->setRange(0, 1);
    // --

    setWidget(pCustomPlot);

    // Set a minimum size for this widget
    QScreen* pScreen= QGuiApplication::primaryScreen();
    setMinimumSize(QSize(pScreen->size().width()/ 3, pScreen->size().height() / 3));

}

// Dtor
VppCustomPlotWidget::~VppCustomPlotWidget() {
}

// Returns the chart view
//QChartView* VPPCustomPlotWidget::getChartView() const {
//	return pChartView_.get();
//}
