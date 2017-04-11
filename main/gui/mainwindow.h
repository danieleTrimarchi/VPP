#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QToolbar>
#include "LogWindow.h"
#include <QtCharts/QChartGlobal>
#include <QtCharts/QChart>
#include <QtCharts/QLogValueAxis>
#include <QtCharts/QPolarChart>
#include <QtCharts/QLegend>
#include "VariablesDockWidget.h"
#include "LogDockWidget.h"
#include "PolarPlotWidget.h"
#include "MultiplePlotWidget.h"
#include "ThreeDPlotWidget.h"
#include "VppTableDockWidget.h"
#include "VariableFileParser.h"
#include "SailSet.h"
#include "VPPXYChartWidget.h"
#include "boost/shared_ptr.hpp"

class ToolBar;
class VPPItemFactory;

QT_FORWARD_DECLARE_CLASS(QMenu)
QT_CHARTS_USE_NAMESPACE

/* Main window of the V++ application. This class direcly
 * derives from the Qt example : MainWindow
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:

	typedef QMap<QString, QSize> CustomSizeHintMap;

	/// Ctor. Explicit specifies that the constructor
	/// doesn't allow implicit conversions or copy-initialization
	explicit MainWindow(
			QWidget *parent = Q_NULLPTR,
			Qt::WindowFlags flags = 0);

	/// Virtual destructor
	virtual ~MainWindow();

	/// Append a string to the log
	void appendToLog(const QString &text);

	/// Create a toolbar with some actions
	void setupToolBar();

public slots:

	/// Import the data from variableFile.txt
	void import();

	/// Run the VPP analysis
	void run();

	/// Save the VPP results to file
	bool saveResults();

	// if save was successiful, return true, otherwise false
	bool saveFile(const QString &fileName);

	/// Import the VPP results from file to the current session
	void importResults();

	/// Add a table widget with the results
	void tableResults();

	/// Plot the velocity polars
	void plotPolars();

	/// Plot the sail coefficients
	void plotSailCoeffs();

	/// Plot the sail coefficients derivatives
	void plot_d_SailCoeffs();

	/// Plot the sail coefficients second derivatives
	void plot_d2_SailCoeffs();

	/// Plot sail forces and moments
	void plotSailForceMoments();

	/// Plot the total resistance
	void plotTotalResistance();

	/// Plot the Frictional resistance of the hull, the keel and the rudder
	void plotFrictionalResistance();

	/// Plot the delta Frictional resistance due to the heel of the hull
	void plotDelta_FrictionalResistance_Heel();

	/// Plot the Induced resistance
	void plotInducedResistance();

	/// Plot the Residuary resistance
	void plotResiduaryResistance();

	/// Plot the Negative resistance
	void plotNegativeResistance();

	/// Temp method used to test QCustomPlot in the current env
	void testQCustomPlot();

	/// Add a 3dPlot widget
	void plotOptimizationSpace();

	/// Plot the gradient of the solution
	/// Grad(u) = | du/du du/dPhi  du/db  du/df  |
	void plotGradient();

	/// Plot the Jacobian of the solution
	/// J = | dF/du dF/dPhi |	|du	 |
	///	    | dM/du dM/dPhi |	|dPhi|
	void plotJacobian();

	/// print out the software info
	void about();

	void actionTriggered(QAction *action);

	/// Slot called on deletion of a plotWidget. It assures
	/// the sync of the tabbedWidgets_ vector
	void updateTabbedWidgetsVector(const VppTabDockWidget*);

private:

	/// Set the menu bar
	void setupMenuBar();

	/// Add the widget to the tabbed ones
	void tabDockWidget(VppTabDockWidget*);

	// Remove a widget from the buffer vector. This should be called from the
	// as a slot from the delete of the QDockWidget right before the destruction
	void removeWidgetFromVector(VppTabDockWidget* pWidget);

	/// Make sure a boat description has been imported. Otherwise
	/// warns the user with an error-like widget
	bool hasBoatDescription();

	/// Log widget - This must be declared at the very beginning as we want to
	/// make sure it is the last item to be removed on destruction
	boost::shared_ptr<QTextEdit> pLogWidget_;

	/// Menus available to the menu bar
	boost::shared_ptr<QMenu> pVppActionMenu_, pPreferencesMenu_, pWidgetMenu_;

	/// Toolbar with some shortcuts to actions
	QToolBar* pToolBar_;

	/// Widget that contains the tabular view of the results
	boost::shared_ptr<VppTabDockWidget> pTableWidget_;

	/// Widget that contains the variable tree or the log view
	boost::shared_ptr<VppTabDockWidget> pPolarPlotWidget_;

	/// XY Plot widget
	boost::shared_ptr<VppTabDockWidget> pXYPlotWidget_;

	/// Multi-plot widget used to plot sail coeffs
	boost::shared_ptr<MultiplePlotWidget> pSailCoeffPlotWidget_, p_d_SailCoeffPlotWidget_,
																				p_d2_SailCoeffPlotWidget_, pForceMomentsPlotWidget_,
																				pTotResistancePlotWidget_, pFricitionalResistancePlotWidget_,
																				pInducedResistancePlotWidget_, pResiduaryResistancePlotWidget_,
																				p_dFrictRes_HeelPlotWidget_, pNegativeResistancePlotWidget_,
																				pJacobianPlotWidget_;

	/// Three dimensional plot widget
	boost::shared_ptr<ThreeDPlotWidget> p3dPlotWidget_;

	/// Variable Widget
	boost::shared_ptr<VariablesDockWidget> pVariablesWidget_;

	/// Menu with the actions to plot sail coeffs and resistance components
	boost::shared_ptr<QMenu> pSailCoeffsMenu_;

	/// Vector storing all of the actions. Used to assure that the actions are deleted
	/// on destruction, thus preventing leaks
	std::vector<boost::shared_ptr<QAction> > actionVector_;

	std::vector<VppTabDockWidget*> tabbedWidgets_;

	/// Main window label
	const QString windowLabel_;

	/// Variable file parser, used to import a boat description and store the variables
	boost::shared_ptr<VariableFileParser> pVariableFileParser_;

	/// Sail configuration, based on the variables that have been read in
	boost::shared_ptr<SailSet> pSails_;

	/// Declare a container for all the items that
	/// constitute the VPP components (Wind, Resistance, RightingMoment...)
	boost::shared_ptr<VPPItemFactory> pVppItems_;

};

#endif // MAINWINDOW_H
