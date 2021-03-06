#ifndef VPP_CUSTOMPLOTWIDGET_H
#define VPP_CUSTOMPLOTWIDGET_H

#include "qcustomplot.h"

/// Class defining a XY plot. It contains a VPPXYChart, which
/// is where the data to plot are actually set
class VppCustomPlotWidgetBase : public QCustomPlot {

		Q_OBJECT

public:

		enum lineStyle {
			randomStyle,
			showPoints
		};

	/// Explicit Constructor
	explicit VppCustomPlotWidgetBase(QString title, QString xAxisLabel, QString yAxisLabel,
																	QWidget* parent= Q_NULLPTR);

	/// Dtor
	~VppCustomPlotWidgetBase();

	/// Add some data to the plot
	virtual void addData(QVector<double>& x, QVector<double>& y, QString dataLabel="", int style= lineStyle::randomStyle);

	/// Add some quivers to the plots
	void addQuivers(QVector<double>& x, QVector<double>& y, QVector<double>& dx, QVector<double>& dy );

Q_SIGNALS:

	void requestFullScreen(const VppCustomPlotWidgetBase*);

protected :

	/// Override the parent class method called on double click
	virtual void mouseDoubleClickEvent(QMouseEvent*);

  /// Overrides the key actions
  virtual void keyPressEvent(QKeyEvent*);

  /// Select a curve - either a QCPGraphs (XY plots) or
	// QCPCurves (polar plots)
  virtual void select(QCPAbstractPlottable*) =0;

  /// Arrange the legend on one or two columns
  void toggleMultiColumnsLegend();

protected slots:

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

	/// Show the coordinates of a point - connected to mouseMoveEvent.
	/// Implemented of the advice given in: https://stackoverflow.com/questions/
	/// 18140446/display-the-plot-values-on-mouse-over-detect-scatter-points
	/// Pure virtual because the format of this method must change according to
	/// the type of plot - polar plot must convert the xy coordinates to polar!
	virtual void showPointToolTip(QMouseEvent*) =0;

	/// Manage what happens when the user right-clicks.
	/// From : qcustomplot interaction example
  void contextMenuRequest(QPoint pos);

  /// Hide/show a curve that has been selected
  virtual void toggleSelected() =0;

  /// Show all curves in the plot
  virtual void showAllCurves();

  /// Hide all curves in the plot
  virtual void hideAllCurves();

  /// Move the legend
  void moveLegend();

};

#endif // VPP_XY_CUSTOMPLOTWIDGET_H
