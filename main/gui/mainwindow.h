#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QToolBar>
#include "LogWindow.h"
#include "VariablesDockWidget.h"
#include "LogDockWidget.h"
#include "MultiplePlotWidget.h"
#include "ThreeDPlotWidget.h"
#include "VppTableDockWidget.h"
#include "VariableFileParser.h"
#include "SailSet.h"
#include "DebugStream.h"
#include "VPPSolverFactoryBase.h"
#include "VppCustomPlotWidget.h"

class ToolBar;
class VPPItemFactory;

QT_FORWARD_DECLARE_CLASS(QMenu)

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

	/// Get the toolbar with some shortcuts to actions
	QToolBar* getToolBar();

	/// Create a toolbar with some actions
	void setupToolBar();

	/// Updates the variable tree getting values from the
	/// settingsWindow tree tab
	void udpateVariableTree();


public slots:

	/// Open the settings dialog
	void openSettings();

	/// Run the VPP analysis
	void run();

	/// Save the VPP results to file
	void saveResults();

	/// Import the VPP results from file to the current session
	void importResults();

	/// Add a table widget with the results
	void tableResults();

	/// Plot the velocity polars
	void plotPolars();

	/// Plot XY results
	void plotXY();

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

	/// Plot the VIscous resistance of the hull, the keel and the rudder
	void plotViscousResistance();

	/// Plot the delta Viscous resistance due to the heel of the hull
	void plotDelta_ViscousResistance_Heel();

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

	/// Plot the Jacobian of the solution, defined as:
	/// J = | dF/du dF/dPhi |
	///	   | dM/du dM/dPhi |
	/// so that 	|dF| = J 	|du  |
	///					|dM|			  |dPhi|
	void plotJacobian();

	/// print out the software info
	void about();

	void actionTriggered(QAction *action);

	/// Slot called on deletion of a plotWidget. It assures
	/// the sync of the tabbedWidgets_ vector
	void updateTabbedWidgetsVector(const VppTabDockWidget*);

private:

	/// Import either settings either results from xml file
	QString importData(string);

	/// Set the menu bar
	void setupMenuBar();

	/// Add the widget to the tabbed ones
	void tabDockWidget(VppTabDockWidget*);

	// Remove a widget from the buffer vector. This should be called from the
	// as a slot from the delete of the QDockWidget right before the destruction
	void removeTabWidgetFromVector(VppTabDockWidget* pWidget);

	/// Make sure a boat description has been imported. Otherwise
	/// warns the user with an error-like widget
	bool hasBoatDescription();

	/// Given the settings, instantiates (refreshes) all of the
	/// VPP items: sailItems, resistanceItems... All is required
	/// to run an analysis
	void updateVppItems();

	/// Make sure a solver is available. Otherwise
	/// warns the user with an error-like widget
	bool hasSolver();

	/// Run a VPPDefaultFileBrowser to select a sail coefficient file
	/// OR leave the default sail coeffs in place
	void importSailCoeffs();

	/// Log widget - This must be declared at the very beginning as we want to
	/// make sure it is the last item to be removed on destruction
	std::shared_ptr<QTextEdit> pLogWidget_;

	/// Menus available to the menu bar
	std::shared_ptr<QMenu> pPreferencesMenu_, pWidgetMenu_, pHelpMenu_;

	/// Toolbar with some shortcuts to actions
	QToolBar* pToolBar_;

	/// Multi-plot widget used to plot sail coeffs
	MultiplePlotWidget	*pSailCoeffPlotWidget_,
											*p_d_SailCoeffPlotWidget_,
											*p_d2_SailCoeffPlotWidget_,
											*pForceMomentsPlotWidget_,
											*pTotResistancePlotWidget_,
											*pViscousResistancePlotWidget_,
											*pInducedResistancePlotWidget_,
											*pResiduaryResistancePlotWidget_,
											*p_dFrictRes_HeelPlotWidget_,
											*pNegativeResistancePlotWidget_,
											*pJacobianPlotWidget_,
											*pGradientPlotWidget_,
											*pPolarPlotWidget_,
											*pXYPlotWidget_;

	/// Widget that contains the tabular view of the results
	std::shared_ptr<VppTableDockWidget> pTableWidget_;

	std::shared_ptr<VppCustomPlotWidget> pCustomPlotWidget_;

	/// Three dimensional plot widget
	std::shared_ptr<ThreeDPlotWidget> p3dPlotWidget_;

	/// Variable Widget
	std::shared_ptr<VariablesDockWidget> pVariablesWidget_;

	std::vector<VppTabDockWidget*> tabbedWidgets_;

	/// Singleton
	/// File browser used to choose sail coeffs file. This becomes member to
	/// report the previous file chosen - in the case of multiple calls
	static std::shared_ptr<VPPDefaultFileBrowser> pSailCoeffFileBrowser_;

	/// Main window label
	const QString windowLabel_;

	/// Variable file parser, used to import a boat description and store the variables
	std::shared_ptr<VariableFileParser> pVariableFileParser_;

	/// Sail configuration, based on the variables that have been read in
	std::shared_ptr<SailSet> pSails_;

	/// Declare a container for all the items that
	/// constitute the VPP components (Wind, Resistance, RightingMoment...)
	std::shared_ptr<VPPItemFactory> pVppItems_;

	/// Ptr to the solver factory - we want to keep this alive to
	/// recover a handle to the results after  run
	std::shared_ptr<Optim::VPPSolverFactoryBase> pSolverFactory_;

};

#endif // MAINWINDOW_H
