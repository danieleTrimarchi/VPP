#include "VppCustomPlotWidget.h"
#include <QApplication>
#include <QScreen>
#include <qboxlayout.h>
#include "qcustomplot.h"

VppCustomPlotWidget::VppCustomPlotWidget(QMainWindow* parent/*=Q_NULLPTR*/, Qt::WindowFlags flags/*=0*/) :
	VppTabDockWidget(parent, flags) {

    setObjectName("VPPCustomPlotView");
    QString demoName;

    setupSimpleDemo(demoName);

    setWindowTitle("QCustomPlot: " + demoName);

    // Set a minimum size for this widget
    QScreen* pScreen= QGuiApplication::primaryScreen();
    setMinimumSize(QSize(pScreen->size().width()/ 3, pScreen->size().height() / 3));

}

// Dtor
VppCustomPlotWidget::~VppCustomPlotWidget() {
}

void VppCustomPlotWidget::setupQuadraticDemo(QString& demoName) {

  demoName = "Quadratic Demo";

  // Instantiate a QCustomPlot
  QCustomPlot * pCustomPlot = new QCustomPlot(this);
  setWidget(pCustomPlot);

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

}

void VppCustomPlotWidget::setupSimpleDemo(QString& demoName) {

  demoName = "Simple Demo";

  // Instantiate a QCustomPlot
  QCustomPlot * pCustomPlot = new QCustomPlot(this);
  setWidget(pCustomPlot);

  // add two new graphs and set their look:
  pCustomPlot->addGraph();
  pCustomPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
  pCustomPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
  pCustomPlot->addGraph();
  pCustomPlot->graph(1)->setPen(QPen(Qt::red)); // line color red for second graph
  // generate some points of data (y0 for first, y1 for second graph):
  QVector<double> x(251), y0(251), y1(251);
  for (int i=0; i<251; ++i)
  {
    x[i] = i;
    y0[i] = qExp(-i/150.0)*qCos(i/10.0); // exponentially decaying cosine
    y1[i] = qExp(-i/150.0);              // exponential envelope
  }
  // configure right and top axis to show ticks but no labels:
  // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
  pCustomPlot->xAxis2->setVisible(true);
  pCustomPlot->xAxis2->setTickLabels(false);
  pCustomPlot->yAxis2->setVisible(true);
  pCustomPlot->yAxis2->setTickLabels(false);
  // make left and bottom axes always transfer their ranges to right and top axes:
  connect(pCustomPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), pCustomPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(pCustomPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), pCustomPlot->yAxis2, SLOT(setRange(QCPRange)));
  // pass data points to graphs:
  pCustomPlot->graph(0)->setData(x, y0);
  pCustomPlot->graph(1)->setData(x, y1);
  // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
  pCustomPlot->graph(0)->rescaleAxes();
  // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
  pCustomPlot->graph(1)->rescaleAxes(true);
  // Note: we could have also just called customPlot->rescaleAxes(); instead
  // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
  pCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);

}

void VppCustomPlotWidget::setupSincScatterDemo(QString& demoName) {

  demoName = "Sinc Scatter Demo";

  // Instantiate a QCustomPlot
  QCustomPlot * pCustomPlot = new QCustomPlot(this);
  setWidget(pCustomPlot);

  pCustomPlot->legend->setVisible(true);
  pCustomPlot->legend->setFont(QFont("Helvetica",9));
  // set locale to english, so we get english decimal separator:
  pCustomPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
  // add confidence band graphs:
  pCustomPlot->addGraph();
  QPen pen;
  pen.setStyle(Qt::DotLine);
  pen.setWidth(1);
  pen.setColor(QColor(180,180,180));
  pCustomPlot->graph(0)->setName("Confidence Band 68%");
  pCustomPlot->graph(0)->setPen(pen);
  pCustomPlot->graph(0)->setBrush(QBrush(QColor(255,50,30,20)));
  pCustomPlot->addGraph();
  pCustomPlot->legend->removeItem(pCustomPlot->legend->itemCount()-1); // don't show two confidence band graphs in legend
  pCustomPlot->graph(1)->setPen(pen);
  pCustomPlot->graph(0)->setChannelFillGraph(pCustomPlot->graph(1));
  // add theory curve graph:
  pCustomPlot->addGraph();
  pen.setStyle(Qt::DashLine);
  pen.setWidth(2);
  pen.setColor(Qt::red);
  pCustomPlot->graph(2)->setPen(pen);
  pCustomPlot->graph(2)->setName("Theory Curve");
  // add data point graph:
  pCustomPlot->addGraph();
  pCustomPlot->graph(3)->setPen(QPen(Qt::blue));
  pCustomPlot->graph(3)->setLineStyle(QCPGraph::lsNone);
  pCustomPlot->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 4));
  // add error bars:
  QCPErrorBars *errorBars = new QCPErrorBars(pCustomPlot->xAxis, pCustomPlot->yAxis);
  errorBars->removeFromLegend();
  errorBars->setAntialiased(false);
  errorBars->setDataPlottable(pCustomPlot->graph(3));
  errorBars->setPen(QPen(QColor(180,180,180)));
  pCustomPlot->graph(3)->setName("Measurement");

  // generate ideal sinc curve data and some randomly perturbed data for scatter plot:
  QVector<double> x0(250), y0(250);
  QVector<double> yConfUpper(250), yConfLower(250);
  for (int i=0; i<250; ++i)
  {
    x0[i] = (i/249.0-0.5)*30+0.01; // by adding a small offset we make sure not do divide by zero in next code line
    y0[i] = qSin(x0[i])/x0[i]; // sinc function
    yConfUpper[i] = y0[i]+0.15;
    yConfLower[i] = y0[i]-0.15;
    x0[i] *= 1000;
  }
  QVector<double> x1(50), y1(50), y1err(50);
  for (int i=0; i<50; ++i)
  {
    // generate a gaussian distributed random number:
    double tmp1 = rand()/(double)RAND_MAX;
    double tmp2 = rand()/(double)RAND_MAX;
    double r = qSqrt(-2*qLn(tmp1))*qCos(2*M_PI*tmp2); // box-muller transform for gaussian distribution
    // set y1 to value of y0 plus a random gaussian pertubation:
    x1[i] = (i/50.0-0.5)*30+0.25;
    y1[i] = qSin(x1[i])/x1[i]+r*0.15;
    x1[i] *= 1000;
    y1err[i] = 0.15;
  }
  // pass data to graphs and let QCustomPlot determine the axes ranges so the whole thing is visible:
  pCustomPlot->graph(0)->setData(x0, yConfUpper);
  pCustomPlot->graph(1)->setData(x0, yConfLower);
  pCustomPlot->graph(2)->setData(x0, y0);
  pCustomPlot->graph(3)->setData(x1, y1);
  errorBars->setData(y1err);
  pCustomPlot->graph(2)->rescaleAxes();
  pCustomPlot->graph(3)->rescaleAxes(true);
  // setup look of bottom tick labels:
  pCustomPlot->xAxis->setTickLabelRotation(30);
  pCustomPlot->xAxis->ticker()->setTickCount(9);
  pCustomPlot->xAxis->setNumberFormat("ebc");
  pCustomPlot->xAxis->setNumberPrecision(1);
  pCustomPlot->xAxis->moveRange(-10);
  // make top right axes clones of bottom left axes. Looks prettier:
  pCustomPlot->axisRect()->setupFullAxesBox();

}

void VppCustomPlotWidget::setupScatterStyleDemo(QString& demoName)
{
  demoName = "Scatter Style Demo";

  // Instantiate a QCustomPlot
  QCustomPlot * pCustomPlot = new QCustomPlot(this);
  setWidget(pCustomPlot);

  pCustomPlot->legend->setVisible(true);
  pCustomPlot->legend->setFont(QFont("Helvetica", 9));
  pCustomPlot->legend->setRowSpacing(-3);
  QVector<QCPScatterStyle::ScatterShape> shapes;
  shapes << QCPScatterStyle::ssCross;
  shapes << QCPScatterStyle::ssPlus;
  shapes << QCPScatterStyle::ssCircle;
  shapes << QCPScatterStyle::ssDisc;
  shapes << QCPScatterStyle::ssSquare;
  shapes << QCPScatterStyle::ssDiamond;
  shapes << QCPScatterStyle::ssStar;
  shapes << QCPScatterStyle::ssTriangle;
  shapes << QCPScatterStyle::ssTriangleInverted;
  shapes << QCPScatterStyle::ssCrossSquare;
  shapes << QCPScatterStyle::ssPlusSquare;
  shapes << QCPScatterStyle::ssCrossCircle;
  shapes << QCPScatterStyle::ssPlusCircle;
  shapes << QCPScatterStyle::ssPeace;
  shapes << QCPScatterStyle::ssCustom;

  QPen pen;
  // add graphs with different scatter styles:
  for (int i=0; i<shapes.size(); ++i)
  {
  	pCustomPlot->addGraph();
    pen.setColor(QColor(qSin(i*0.3)*100+100, qSin(i*0.6+0.7)*100+100, qSin(i*0.4+0.6)*100+100));
    // generate data:
    QVector<double> x(10), y(10);
    for (int k=0; k<10; ++k)
    {
      x[k] = k/10.0 * 4*3.14 + 0.01;
      y[k] = 7*qSin(x[k])/x[k] + (shapes.size()-i)*5;
    }
    pCustomPlot->graph()->setData(x, y);
    pCustomPlot->graph()->rescaleAxes(true);
    pCustomPlot->graph()->setPen(pen);
    pCustomPlot->graph()->setName(QCPScatterStyle::staticMetaObject.enumerator(QCPScatterStyle::staticMetaObject.indexOfEnumerator("ScatterShape")).valueToKey(shapes.at(i)));
    pCustomPlot->graph()->setLineStyle(QCPGraph::lsLine);
    // set scatter style:
    if (shapes.at(i) != QCPScatterStyle::ssCustom)
    {
    	pCustomPlot->graph()->setScatterStyle(QCPScatterStyle(shapes.at(i), 10));
    }
    else
    {
      QPainterPath customScatterPath;
      for (int i=0; i<3; ++i)
        customScatterPath.cubicTo(qCos(2*M_PI*i/3.0)*9, qSin(2*M_PI*i/3.0)*9, qCos(2*M_PI*(i+0.9)/3.0)*9, qSin(2*M_PI*(i+0.9)/3.0)*9, 0, 0);
      pCustomPlot->graph()->setScatterStyle(QCPScatterStyle(customScatterPath, QPen(Qt::black, 0), QColor(40, 70, 255, 50), 10));
    }
  }
  // set blank axis lines:
  pCustomPlot->rescaleAxes();
  pCustomPlot->xAxis->setTicks(false);
  pCustomPlot->yAxis->setTicks(false);
  pCustomPlot->xAxis->setTickLabels(false);
  pCustomPlot->yAxis->setTickLabels(false);
  // make top right axes clones of bottom left axes:
  pCustomPlot->axisRect()->setupFullAxesBox();
}

void VppCustomPlotWidget::setupScatterPixmapDemo(QString& demoName)
{
  demoName = "Scatter Pixmap Demo";

  // Instantiate a QCustomPlot
  QCustomPlot * pCustomPlot = new QCustomPlot(this);
  setWidget(pCustomPlot);

  pCustomPlot->axisRect()->setBackground(QPixmap("./resources/solarpanels.jpg"));
  pCustomPlot->addGraph();
  pCustomPlot->graph()->setLineStyle(QCPGraph::lsLine);
  QPen pen;
  pen.setColor(QColor(255, 200, 20, 200));
  pen.setStyle(Qt::DashLine);
  pen.setWidthF(2.5);
  pCustomPlot->graph()->setPen(pen);
  pCustomPlot->graph()->setBrush(QBrush(QColor(255,200,20,70)));
  pCustomPlot->graph()->setScatterStyle(QCPScatterStyle(QPixmap("./resources/sun.png")));
  // set graph name, will show up in legend next to icon:
  pCustomPlot->graph()->setName("Data from Photovoltaic\nenergy barometer 2011");
  // set data:
  QVector<double> year, value;
  year  << 2005 << 2006 << 2007 << 2008  << 2009  << 2010 << 2011;
  value << 2.17 << 3.42 << 4.94 << 10.38 << 15.86 << 29.33 << 52.1;
  pCustomPlot->graph()->setData(year, value);

  // set title of plot:
  pCustomPlot->plotLayout()->insertRow(0);
  pCustomPlot->plotLayout()->addElement(0, 0, new QCPTextElement(pCustomPlot, "Regenerative Energies"));
  // axis configurations:
  pCustomPlot->xAxis->setLabel("Year");
  pCustomPlot->yAxis->setLabel("Installed Gigawatts of\nphotovoltaic in the European Union");
  pCustomPlot->xAxis2->setVisible(true);
  pCustomPlot->yAxis2->setVisible(true);
  pCustomPlot->xAxis2->setTickLabels(false);
  pCustomPlot->yAxis2->setTickLabels(false);
  pCustomPlot->xAxis2->setTicks(false);
  pCustomPlot->yAxis2->setTicks(false);
  pCustomPlot->xAxis2->setSubTicks(false);
  pCustomPlot->yAxis2->setSubTicks(false);
  pCustomPlot->xAxis->setRange(2004.5, 2011.5);
  pCustomPlot->yAxis->setRange(0, 52);
  // setup legend:
  pCustomPlot->legend->setFont(QFont(font().family(), 7));
  pCustomPlot->legend->setIconSize(50, 20);
  pCustomPlot->legend->setVisible(true);
  pCustomPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop);
}

void VppCustomPlotWidget::setupTextureBrushDemo(QString& demoName)
{
  demoName = "Texture Brush Demo";

  // Instantiate a QCustomPlot
  QCustomPlot * pCustomPlot = new QCustomPlot(this);
  setWidget(pCustomPlot);

  // add two graphs with a textured fill:
  pCustomPlot->addGraph();
  QPen redDotPen;
  redDotPen.setStyle(Qt::DotLine);
  redDotPen.setColor(QColor(170, 100, 100, 180));
  redDotPen.setWidthF(2);
  pCustomPlot->graph(0)->setPen(redDotPen);
  pCustomPlot->graph(0)->setBrush(QBrush(QPixmap("./resources/balboa.jpg"))); // fill with texture of specified image

  pCustomPlot->addGraph();
  pCustomPlot->graph(1)->setPen(QPen(Qt::red));

  // activate channel fill for graph 0 towards graph 1:
  pCustomPlot->graph(0)->setChannelFillGraph(pCustomPlot->graph(1));

  // generate data:
  QVector<double> x(250);
  QVector<double> y0(250), y1(250);
  for (int i=0; i<250; ++i)
  {
    // just playing with numbers, not much to learn here
    x[i] = 3*i/250.0;
    y0[i] = 1+qExp(-x[i]*x[i]*0.8)*(x[i]*x[i]+x[i]);
    y1[i] = 1-qExp(-x[i]*x[i]*0.4)*(x[i]*x[i])*0.1;
  }

  // pass data points to graphs:
  pCustomPlot->graph(0)->setData(x, y0);
  pCustomPlot->graph(1)->setData(x, y1);
  // activate top and right axes, which are invisible by default:
  pCustomPlot->xAxis2->setVisible(true);
  pCustomPlot->yAxis2->setVisible(true);
  // make tick labels invisible on top and right axis:
  pCustomPlot->xAxis2->setTickLabels(false);
  pCustomPlot->yAxis2->setTickLabels(false);
  // set ranges:
  pCustomPlot->xAxis->setRange(0, 2.5);
  pCustomPlot->yAxis->setRange(0.9, 1.6);
  // assign top/right axes same properties as bottom/left:
  pCustomPlot->axisRect()->setupFullAxesBox();
}

void VppCustomPlotWidget::setupMultiAxisDemo(QString& demoName)
{
  // Instantiate a QCustomPlot
  QCustomPlot * pCustomPlot = new QCustomPlot(this);
  setWidget(pCustomPlot);

  pCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
  demoName = "Multi Axis Demo";

  pCustomPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
  pCustomPlot->legend->setVisible(true);
  QFont legendFont = font();  // start out with MainWindow's font..
  legendFont.setPointSize(9); // and make a bit smaller for legend
  pCustomPlot->legend->setFont(legendFont);
  pCustomPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
  // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
  pCustomPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

  // setup for graph 0: key axis left, value axis bottom
  // will contain left maxwell-like function
  pCustomPlot->addGraph(pCustomPlot->yAxis, pCustomPlot->xAxis);
  pCustomPlot->graph(0)->setPen(QPen(QColor(255, 100, 0)));
  pCustomPlot->graph(0)->setBrush(QBrush(QPixmap("./balboa.jpg"))); // fill with texture of specified image
  pCustomPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
  pCustomPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
  pCustomPlot->graph(0)->setName("Left maxwell function");

  // setup for graph 1: key axis bottom, value axis left (those are the default axes)
  // will contain bottom maxwell-like function with error bars
  pCustomPlot->addGraph();
  pCustomPlot->graph(1)->setPen(QPen(Qt::red));
  pCustomPlot->graph(1)->setBrush(QBrush(QPixmap("./balboa.jpg"))); // same fill as we used for graph 0
  pCustomPlot->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
  pCustomPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, Qt::white, 7));
  pCustomPlot->graph(1)->setName("Bottom maxwell function");
  QCPErrorBars *errorBars = new QCPErrorBars(pCustomPlot->xAxis, pCustomPlot->yAxis);
  errorBars->removeFromLegend();
  errorBars->setDataPlottable(pCustomPlot->graph(1));

  // setup for graph 2: key axis top, value axis right
  // will contain high frequency sine with low frequency beating:
  pCustomPlot->addGraph(pCustomPlot->xAxis2, pCustomPlot->yAxis2);
  pCustomPlot->graph(2)->setPen(QPen(Qt::blue));
  pCustomPlot->graph(2)->setName("High frequency sine");

  // setup for graph 3: same axes as graph 2
  // will contain low frequency beating envelope of graph 2
  pCustomPlot->addGraph(pCustomPlot->xAxis2, pCustomPlot->yAxis2);
  QPen blueDotPen;
  blueDotPen.setColor(QColor(30, 40, 255, 150));
  blueDotPen.setStyle(Qt::DotLine);
  blueDotPen.setWidthF(4);
  pCustomPlot->graph(3)->setPen(blueDotPen);
  pCustomPlot->graph(3)->setName("Sine envelope");

  // setup for graph 4: key axis right, value axis top
  // will contain parabolically distributed data points with some random perturbance
  pCustomPlot->addGraph(pCustomPlot->yAxis2, pCustomPlot->xAxis2);
  pCustomPlot->graph(4)->setPen(QColor(50, 50, 50, 255));
  pCustomPlot->graph(4)->setLineStyle(QCPGraph::lsNone);
  pCustomPlot->graph(4)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
  pCustomPlot->graph(4)->setName("Some random data around\na quadratic function");

  // generate data, just playing with numbers, not much to learn here:
  QVector<double> x0(25), y0(25);
  QVector<double> x1(15), y1(15), y1err(15);
  QVector<double> x2(250), y2(250);
  QVector<double> x3(250), y3(250);
  QVector<double> x4(250), y4(250);
  for (int i=0; i<25; ++i) // data for graph 0
  {
    x0[i] = 3*i/25.0;
    y0[i] = qExp(-x0[i]*x0[i]*0.8)*(x0[i]*x0[i]+x0[i]);
  }
  for (int i=0; i<15; ++i) // data for graph 1
  {
    x1[i] = 3*i/15.0;;
    y1[i] = qExp(-x1[i]*x1[i])*(x1[i]*x1[i])*2.6;
    y1err[i] = y1[i]*0.25;
  }
  for (int i=0; i<250; ++i) // data for graphs 2, 3 and 4
  {
    x2[i] = i/250.0*3*M_PI;
    x3[i] = x2[i];
    x4[i] = i/250.0*100-50;
    y2[i] = qSin(x2[i]*12)*qCos(x2[i])*10;
    y3[i] = qCos(x3[i])*10;
    y4[i] = 0.01*x4[i]*x4[i] + 1.5*(rand()/(double)RAND_MAX-0.5) + 1.5*M_PI;
  }

  // pass data points to graphs:
  pCustomPlot->graph(0)->setData(x0, y0);
  pCustomPlot->graph(1)->setData(x1, y1);
  errorBars->setData(y1err);
  pCustomPlot->graph(2)->setData(x2, y2);
  pCustomPlot->graph(3)->setData(x3, y3);
  pCustomPlot->graph(4)->setData(x4, y4);
  // activate top and right axes, which are invisible by default:
  pCustomPlot->xAxis2->setVisible(true);
  pCustomPlot->yAxis2->setVisible(true);
  // set ranges appropriate to show data:
  pCustomPlot->xAxis->setRange(0, 2.7);
  pCustomPlot->yAxis->setRange(0, 2.6);
  pCustomPlot->xAxis2->setRange(0, 3.0*M_PI);
  pCustomPlot->yAxis2->setRange(-70, 35);
  // set pi ticks on top axis:
  pCustomPlot->xAxis2->setTicker(QSharedPointer<QCPAxisTickerPi>(new QCPAxisTickerPi));
  // add title layout element:
  pCustomPlot->plotLayout()->insertRow(0);
  pCustomPlot->plotLayout()->addElement(0, 0, new QCPTextElement(pCustomPlot, "Way too many graphs in one plot"));
  // set labels:
  pCustomPlot->xAxis->setLabel("Bottom axis with outward ticks");
  pCustomPlot->yAxis->setLabel("Left axis label");
  pCustomPlot->xAxis2->setLabel("Top axis label");
  pCustomPlot->yAxis2->setLabel("Right axis label");
  // make ticks on bottom axis go outward:
  pCustomPlot->xAxis->setTickLength(0, 5);
  pCustomPlot->xAxis->setSubTickLength(0, 3);
  // make ticks on right axis go inward and outward:
  pCustomPlot->yAxis2->setTickLength(3, 3);
  pCustomPlot->yAxis2->setSubTickLength(1, 1);
}

void VppCustomPlotWidget::setupParametricCurveDemo(QString& demoName)
{
  demoName = "Parametric Curves Demo";

  // Instantiate a QCustomPlot
  QCustomPlot * pCustomPlot = new QCustomPlot(this);
  setWidget(pCustomPlot);

  // create empty curve objects:
  QCPCurve *fermatSpiral1 = new QCPCurve(pCustomPlot->xAxis, pCustomPlot->yAxis);
  QCPCurve *fermatSpiral2 = new QCPCurve(pCustomPlot->xAxis, pCustomPlot->yAxis);
  QCPCurve *deltoidRadial = new QCPCurve(pCustomPlot->xAxis, pCustomPlot->yAxis);
  // generate the curve data points:
  const int pointCount = 500;
  QVector<QCPCurveData> dataSpiral1(pointCount), dataSpiral2(pointCount), dataDeltoid(pointCount);
  for (int i=0; i<pointCount; ++i)
  {
    double phi = i/(double)(pointCount-1)*8*M_PI;
    double theta = i/(double)(pointCount-1)*2*M_PI;
    dataSpiral1[i] = QCPCurveData(i, qSqrt(phi)*qCos(phi), qSqrt(phi)*qSin(phi));
    dataSpiral2[i] = QCPCurveData(i, -dataSpiral1[i].key, -dataSpiral1[i].value);
    dataDeltoid[i] = QCPCurveData(i, 2*qCos(2*theta)+qCos(1*theta)+2*qSin(theta), 2*qSin(2*theta)-qSin(1*theta));
  }
  // pass the data to the curves; we know t (i in loop above) is ascending, so set alreadySorted=true (saves an extra internal sort):
  fermatSpiral1->data()->set(dataSpiral1, true);
  fermatSpiral2->data()->set(dataSpiral2, true);
  deltoidRadial->data()->set(dataDeltoid, true);
  // color the curves:
  fermatSpiral1->setPen(QPen(Qt::blue));
  fermatSpiral1->setBrush(QBrush(QColor(0, 0, 255, 20)));
  fermatSpiral2->setPen(QPen(QColor(255, 120, 0)));
  fermatSpiral2->setBrush(QBrush(QColor(255, 120, 0, 30)));
  QRadialGradient radialGrad(QPointF(310, 180), 200);
  radialGrad.setColorAt(0, QColor(170, 20, 240, 100));
  radialGrad.setColorAt(0.5, QColor(20, 10, 255, 40));
  radialGrad.setColorAt(1,QColor(120, 20, 240, 10));
  deltoidRadial->setPen(QPen(QColor(170, 20, 240)));
  deltoidRadial->setBrush(QBrush(radialGrad));
  // set some basic customPlot config:
  pCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
  pCustomPlot->axisRect()->setupFullAxesBox();
  pCustomPlot->rescaleAxes();
}

void VppCustomPlotWidget::setupSimpleItemDemo(QString& demoName)
{
  demoName = "Simple Item Demo";

  // Instantiate a QCustomPlot
  QCustomPlot * pCustomPlot = new QCustomPlot(this);
  setWidget(pCustomPlot);

  pCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

  // add the text label at the top:
  QCPItemText *textLabel = new QCPItemText(pCustomPlot);
  textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
  textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
  textLabel->position->setCoords(0.5, 0); // place position at center/top of axis rect
  textLabel->setText("Text Item Demo");
  textLabel->setFont(QFont(font().family(), 16)); // make font a bit larger
  textLabel->setPen(QPen(Qt::black)); // show black border around text

  // add the arrow:
  QCPItemLine *arrow = new QCPItemLine(pCustomPlot);
  //arrow->start->setParentAnchor(textLabel->bottom);
  arrow->start->setCoords(3, 1.);
	arrow->end->setCoords(4, 1.6); // point to (4, 1.6) in x-y-plot coordinates
  arrow->setHead(QCPLineEnding::esSpikeArrow);

  QCPItemLine *arrow2 = new QCPItemLine(pCustomPlot);
  arrow2->start->setCoords(5, 1.);
	arrow2->end->setCoords(6, 1.6); // point to (4, 1.6) in x-y-plot coordinates
  arrow2->setHead(QCPLineEnding::esSpikeArrow);

}
