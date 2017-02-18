#ifndef VPPCHARTVIEW_H
#define VPPCHARTVIEW_H

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QRubberBand>

QT_CHARTS_USE_NAMESPACE

class VppChartView : public QChartView {

	Q_OBJECT

public :

	/// Ctor
    explicit VppChartView(QChart *chart, QWidget *parent = Q_NULLPTR);

    /// Dtor
    ~VppChartView();

protected:

    /// Overrides the key actions
    virtual void keyPressEvent(QKeyEvent *event);


};

class VppPolarChartView : public VppChartView {

	Q_OBJECT

public :

	/// Ctor
    explicit VppPolarChartView(QChart *chart, QWidget *parent = Q_NULLPTR);

    /// Dtor
    ~VppPolarChartView();

protected:

    void keyPressEvent(QKeyEvent *event);


};


#endif
