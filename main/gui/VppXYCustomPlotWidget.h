#ifndef VPP_XY_CUSTOMPLOTWIDGET_H
#define VPP_XY_CUSTOMPLOTWIDGET_H

#include "VppCustomPlotWidgetBase.h"

/// Class defining a XY plot. It contains a VPPXYChart, which
/// is where the data to plot are actually set
class VppXYCustomPlotWidget : public VppCustomPlotWidgetBase {

		Q_OBJECT

	public:

		/// Explicit Constructor
		explicit VppXYCustomPlotWidget(QString title, QString xAxisLabel, QString yAxisLabel,
				QWidget* parent= Q_NULLPTR);

		/// Dtor
		~VppXYCustomPlotWidget();

	protected:

		/// Select a curve - in this case a QCPGraphs
		virtual void select(QCPAbstractPlottable *);

		protected slots:

		/// Hide/Show a selected curve - in this case a QCPGraphs
		virtual void toggleSelected();

		/// Show all curves in the plot
	  virtual void showAllCurves();

	  /// Hide all curves in the plot
	  virtual void hideAllCurves();

		/// Show the coordinates of a point - connected to mouseMoveEvent.
		/// Implemented of the advice given in: https://stackoverflow.com/questions/
		/// 18140446/display-the-plot-values-on-mouse-over-detect-scatter-points
		/// Pure virtual because the format of this method must change according to
		/// the type of plot - polar plot must convert the xy coordinates to polar!
		virtual void showPointToolTip(QMouseEvent*);

};

#endif // VPP_XY_CUSTOMPLOTWIDGET_H
