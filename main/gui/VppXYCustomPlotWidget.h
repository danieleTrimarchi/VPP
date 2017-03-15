#ifndef VPP_XY_CUSTOMPLOTWIDGET_H
#define VPP_XY_CUSTOMPLOTWIDGET_H

#include "qcustomplot.h"

/// Class defining a XY plot. It contains a VPPXYChart, which
/// is where the data to plot are actually set
class VppXYCustomPlotWidget : public QCustomPlot {

		Q_OBJECT

public:

	/// Explicit Constructor
	explicit VppXYCustomPlotWidget(QString title, QString xAxisLabel, QString yAxisLabel,
																	QWidget* parent= Q_NULLPTR);

	/// Dtor
	~VppXYCustomPlotWidget();

	/// Add some data to the plot
	void addData(QVector<double>& x, QVector<double>& y, QString dataLabel="");

Q_SIGNALS:

	void requestFullScreen(const VppXYCustomPlotWidget*);

protected :

	/// Override the parent class method called on double click
	virtual void mouseDoubleClickEvent(QMouseEvent*);

  /// Overrides the key actions
  virtual void keyPressEvent(QKeyEvent *event);

private slots:

	/// Normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
	/// the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
	/// and the axis base line together. However, the axis label shall be selectable individually.
	///
	/// The selection state of the left and right axes shall be synchronized as well as the state of the
	/// bottom and top axes.
	///
	/// Further, we want to synchronize the selection of the graphs with the selection state of the respective
	/// legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
	/// or on its legend item.
	void selectionChanged();

	/// If an axis is selected, only allow the direction of that axis to be dragged
	/// If no axis is selected, both directions may be dragged
	void mousePress();

	/// if an axis is selected, only allow the direction of that axis to be zoomed
	/// If no axis is selected, both directions may be zoomed
	void mouseWheel();

	/// Change selection using arrow keys up or down. Called by keyPressEvent
	void changeGraphSelection(int key);


};

#endif // VPP_XY_CUSTOMPLOTWIDGET_H
