#ifndef MULTIPLEPLOTCHARTVIEW_H
#define MULTIPLEPLOTCHARTVIEW_H

#include <QtCharts/QChartView>

QT_CHARTS_USE_NAMESPACE

/// Class that overrides the QChartView in order to define
/// the signals required to hide the plots of a multiplePlotWidget
/// on double click
class MultiplePlotChartComponent : public QChartView {

	Q_OBJECT

public:

	/// Ctor
	explicit MultiplePlotChartComponent(QWidget *parent = Q_NULLPTR);

	/// Ctor taking a chart
	explicit MultiplePlotChartComponent(QChart *chart, QWidget *parent = Q_NULLPTR);

	/// Dtor
	~MultiplePlotChartComponent();

Q_SIGNALS:

	void requestFullScreen(const MultiplePlotChartComponent*);

	protected :

	/// Override the parent class method called on double click
	virtual void mouseDoubleClickEvent(QMouseEvent*);

	/// Overrides the key actions
	virtual void keyPressEvent(QKeyEvent *event);

	private:


};


#endif
