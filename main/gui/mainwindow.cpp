#include "MainWindow.h"

#include <stdio.h>
#include <iostream>

#include <QDockWidget>

#include <QAction>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QFile>
#include <QDataStream>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QSignalMapper>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QtCore>
#include "VariableTreeModel.h"
#include <QScreen>
#include "VPPItemFactory.h"
#include "VppCustomPlotWidget.h"
#include "VPPDialogs.h"
#include "VPPJacobian.h"
#include "VPPGradient.h"
#include "VPPSolverFactoryBase.h"
#include "Version.h"
#include "VppPolarCustomPlotWidget.h"

// Stream used to redirect cout to the log window
// This object is explicitely deleted in the destructor
// of MainWindow
boost::shared_ptr<QDebugStream> pQstream;

Q_DECLARE_METATYPE(VppTabDockWidget::DockWidgetFeatures)

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags):
QMainWindow(parent, flags),
pXYPlotWidget_(0),
pLogWidget_(0),
pSailCoeffPlotWidget_(0),
p_d_SailCoeffPlotWidget_(0),
p_d2_SailCoeffPlotWidget_(0),
pForceMomentsPlotWidget_(0),
pVariablesWidget_(0),
p3dPlotWidget_(0),
pJacobianPlotWidget_(0),
windowLabel_("V++") {

	// Set the name and the title of the app
	setObjectName(windowLabel_);
	setWindowTitle(windowLabel_);

	// The log widget is the central widget for this application
	pLogWidget_.reset(new QTextEdit(this));
	setCentralWidget(pLogWidget_.get());
	pLogWidget_->setReadOnly(true);

	// Tie cout to the log widget
	//QDebugStream qout(std::cout, pLogWidget_.get());
	pQstream.reset(new QDebugStream(std::cout, pLogWidget_.get()));

	std::cout<<"======================="<<std::endl;
	std::cout<<"===  V++ PROGRAM  ======="<<std::endl;
	std::cout<<"======================="<<std::endl;

	// --

	// Add the widget menu, to be populated by each widget
	pWidgetMenu_.reset(menuBar()->addMenu(tr("&Widgets")));

	// Add the variable tree widget
	pVariablesWidget_.reset( new VariablesDockWidget() );

	// Add the variable view to the right of the app window
	addDockWidget(Qt::RightDockWidgetArea, pVariablesWidget_.get());

	// Add the menubar item for the variable widget
	pWidgetMenu_->addAction(pVariablesWidget_->getMenuToggleViewAction());

	// --

	// Set the toolbars. In this case NO horizontal toolbars.
	setupToolBar();

	// Message that will appear in the status (lower) bar
	statusBar()->showMessage(tr("Status Bar"));

	// Set the menu bar: File, edit...
	setupMenuBar();

	// Make sure the solver factory is empty
	pSolverFactory_.reset();
}

// Virtual destructor
MainWindow::~MainWindow() {

	// Make sure the cout stream redirection class is deleted
	pQstream.reset();
}

void MainWindow::setupMenuBar() {

	pVppActionMenu_.reset( menuBar()->addMenu(tr("&VPP Actions")) );

	// Create 'Import boat description' action and associate an icon
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Import boat description", QIcon(":/icons/importBoatData.png")),
					tr("&Import boat description..."), this)
	) );
	QAction* importBoatAction= actionVector_.back().get();
	importBoatAction->setStatusTip(tr("Import boat description"));
	connect(importBoatAction, &QAction::triggered, this, &MainWindow::import);
	pVppActionMenu_->addAction(importBoatAction);
	pToolBar_->addAction(importBoatAction);

	// Create 'run' action and associate an icon
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Run", QIcon(":/icons/run.png")),
					tr("&Run"), this)
	) );
	QAction* runAction = actionVector_.back().get();
	runAction->setStatusTip(tr("Run the VPP analysis"));
	connect(runAction, &QAction::triggered, this, &MainWindow::run);
	pVppActionMenu_->addAction(runAction);
	pToolBar_->addAction(runAction);

	// Create 'tabular' action and associate an icon
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Result table", QIcon(":/icons/tabularResults.png")),
					tr("&Result table"), this)
	) );
	QAction* tabResAction = actionVector_.back().get();
	tabResAction->setStatusTip(tr("Show the result table"));
	connect(tabResAction, &QAction::triggered, this, &MainWindow::tableResults);
	pVppActionMenu_->addAction(tabResAction);
	pToolBar_->addAction(tabResAction);

	pVppActionMenu_->addSeparator();

	// Create a 'Save Results'action and associate an icon
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("save Results", QIcon(":/icons/saveResults.png")),
					tr("&Save Results"), this)
	) );
	QAction* saveResultsAction = actionVector_.back().get();
	saveResultsAction->setStatusTip(tr("Save results"));
	connect(saveResultsAction, &QAction::triggered, this, &MainWindow::saveResults);
	pVppActionMenu_->addAction(saveResultsAction);

	// Create a 'Save Results'action and associate an icon
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Import Previous Results", QIcon(":/icons/importResults.png")),
					tr("&Import Previous Results"), this)
	) );
	QAction* importResultsAction = actionVector_.back().get();
	importResultsAction->setStatusTip(tr("Import Previous Results"));
	connect(importResultsAction, &QAction::triggered, this, &MainWindow::importResults);
	pVppActionMenu_->addAction(importResultsAction);

	// ---

	// Create an action and associate an icon
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot polars", QIcon(":/icons/plotPolars.png")),
					tr("&Polars"), this)
	) );
	QAction* plotPolarsAction = actionVector_.back().get();
	plotPolarsAction->setStatusTip(tr("Plot polars"));
	connect(plotPolarsAction, &QAction::triggered, this, &MainWindow::plotPolars);
	pToolBar_->addAction(plotPolarsAction);

	// Add a menu in the toolbar. This is used to group plot coeffs, and their derivatives
	pSailCoeffsMenu_.reset( new QMenu("Plot Sail Related Quantities", this) );
	pSailCoeffsMenu_->setIcon( QIcon::fromTheme("Plot Sail Coeffs", QIcon(":/icons/sailCoeffs.png")) );
	pToolBar_->addAction(pSailCoeffsMenu_->menuAction());

	// Plot sail coeffs
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Sail Coeffs", QIcon(":/icons/sailCoeffs.png")),
					tr("&Sail Coeffs"), this)
	) );
	QAction* plotSailCoeffsAction = actionVector_.back().get();
	plotSailCoeffsAction->setStatusTip(tr("Plot Sail Coeffs"));
	pSailCoeffsMenu_->addAction(plotSailCoeffsAction);
	connect(plotSailCoeffsAction, &QAction::triggered, this, &MainWindow::plotSailCoeffs);

	// Plot sail coeffs derivatives
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Sail Coeffs Derivatives", QIcon(":/icons/d_sailCoeffs.png")),
					tr("&d(Sail Coeffs)"), this)
	) );
	QAction* plot_d_SailCoeffsAction= actionVector_.back().get();
	plot_d_SailCoeffsAction->setStatusTip(tr("Plot d(Sail Coeffs)"));
	pSailCoeffsMenu_->addAction(plot_d_SailCoeffsAction);
	connect(plot_d_SailCoeffsAction, &QAction::triggered, this, &MainWindow::plot_d_SailCoeffs);

	// Plot sail coeffs second derivatives
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Sail Coeffs Second Derivatives", QIcon(":/icons/d2_sailCoeffs.png")),
					tr("&d2(Sail Coeffs)"), this)
	) );
	QAction* plot_d2_SailCoeffsAction = actionVector_.back().get();
	plot_d2_SailCoeffsAction->setStatusTip(tr("Plot d2(Sail Coeffs)"));
	pSailCoeffsMenu_->addAction(plot_d2_SailCoeffsAction);
	connect(plot_d2_SailCoeffsAction, &QAction::triggered, this, &MainWindow::plot_d2_SailCoeffs);

	// Plot sail force and moments
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Sail Force/Moments", QIcon(":/icons/sailForceMoment.png")),
					tr("&Plot Sail Force/Moments"), this)
	) );
	QAction* plotSailForceMomentAction = actionVector_.back().get();
	plotSailForceMomentAction->setStatusTip(tr("Plot Sail Force/Moments"));
	pSailCoeffsMenu_->addAction(plotSailForceMomentAction);
	connect(plotSailForceMomentAction, &QAction::triggered, this, &MainWindow::plotSailForceMoments);

	// --

	// ---------------------------------------
	// --- WARNING : for some reason this cannot be defined as member shared ptr, the program crashes..?? ---
	// ---------------------------------------
	QMenu* pResistanceMenu = new QMenu("Plot Resistance", this);
	pResistanceMenu->setIcon( QIcon::fromTheme("Plot Resistance", QIcon(":/icons/resistanceComponent.png")) );
	pToolBar_->addAction(pResistanceMenu->menuAction());

	// Plot total Resistance
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Total Resistance", QIcon(":/icons/totResistance.png")),
					tr("&Total Resistance"), this)
	) );
	QAction* plotTotResAction = actionVector_.back().get();
	plotTotResAction->setStatusTip(tr("Plot Total Resistance"));
	pResistanceMenu->addAction(plotTotResAction);
	connect(plotTotResAction, &QAction::triggered, this, &MainWindow::plotTotalResistance);

	// Plot Frictional Resistance
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Frictional Resistance", QIcon(":/icons/frictionalResistance.png")),
					tr("&Frictional Resistance"), this)
	) );
	QAction* plotFrictResAction = actionVector_.back().get();
	plotFrictResAction->setStatusTip(tr("Plot Frictional Resistance"));
	pResistanceMenu->addAction(plotFrictResAction);
	connect(plotFrictResAction, &QAction::triggered, this, &MainWindow::plotFrictionalResistance);

	// Plot Delta Frictional Resistance due to Heel
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Delta Frictional Resistance due to Heel", QIcon(":/icons/DeltaFrictRes_Heel.png")),
					tr("&Delta Frict. Res. Heel"), this)
	) );
	QAction* plotDeltaFrictResHeelAction = actionVector_.back().get();
	plotDeltaFrictResHeelAction->setStatusTip(tr("Plot Delta Frictional Resistance due to Heel"));
	pResistanceMenu->addAction(plotDeltaFrictResHeelAction);
	connect(plotDeltaFrictResHeelAction, &QAction::triggered, this, &MainWindow::plotDelta_FrictionalResistance_Heel);

	// Plot Induced Resistance
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Induced Resistance", QIcon(":/icons/inducedResistance.png")),
					tr("&Induced Resistance"), this)
	) );
	QAction* plotIndResAction = actionVector_.back().get();
	plotIndResAction->setStatusTip(tr("Plot Induced Resistance"));
	pResistanceMenu->addAction(plotIndResAction);
	connect(plotIndResAction, &QAction::triggered, this, &MainWindow::plotInducedResistance);

	// Plot Residuary Resistance
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Residuary Resistance", QIcon(":/icons/residuaryResistance.png")),
					tr("&Residuary Resistance"), this)
	) );
	QAction* plotResiduaryResAction = actionVector_.back().get();
	plotResiduaryResAction->setStatusTip(tr("Plot Residuary Resistance"));
	pResistanceMenu->addAction(plotResiduaryResAction);
	connect(plotResiduaryResAction, &QAction::triggered, this, &MainWindow::plotResiduaryResistance);

	// Plot Negative Resistance
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Negative Resistance", QIcon(":/icons/negativeResistance.png")),
					tr("&Negative Resistance"), this)
	) );
	QAction* plotNegativeResAction = actionVector_.back().get();
	plotNegativeResAction->setStatusTip(tr("Plot Negative Resistance"));
	pResistanceMenu->addAction(plotNegativeResAction);
	connect(plotNegativeResAction, &QAction::triggered, this, &MainWindow::plotNegativeResistance);

	// --

	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Optimization Space", QIcon(":/icons/plot3d.png")),
					tr("&Plot Optimization Space"), this)
	) );
	QAction* plot3dAction = actionVector_.back().get();
	plot3dAction->setStatusTip(tr("Plot Optimization Space"));
	connect(plot3dAction, &QAction::triggered, this, &MainWindow::plotOptimizationSpace);
	pToolBar_->addAction(plot3dAction);

	// --

	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Gradient", QIcon(":/icons/plotGradient.png")),
					tr("&Plot Graident"), this)
	) );

	QAction* plotGradientAction = actionVector_.back().get();
	plotGradientAction->setStatusTip(tr("Plot Gradient"));
	connect(plotGradientAction, &QAction::triggered, this, &MainWindow::plotGradient);
	pToolBar_->addAction(plotGradientAction);

	// --

	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Jacobian", QIcon(":/icons/plotJacobian.png")),
					tr("&Plot Jacobian"), this)
	) );

	QAction* plotJacobianAction = actionVector_.back().get();
	plotJacobianAction->setStatusTip(tr("Plot Jacobian"));
	connect(plotJacobianAction, &QAction::triggered, this, &MainWindow::plotJacobian);
	pToolBar_->addAction(plotJacobianAction);

	// --

	pToolBar_->addAction(saveResultsAction);
	pToolBar_->addAction(importResultsAction);

	// ---

	pPreferencesMenu_.reset( menuBar()->addMenu(tr("&VPP Settings")) );
	pPreferencesMenu_->addAction(tr("&Select formulations"), this, &MainWindow::plotSailCoeffs);

	pHelpMenu_.reset( menuBar()->addMenu(tr("&Help")) );
	QAction *aboutAct = pHelpMenu_->addAction(tr("&About"), this, &MainWindow::about);
	aboutAct->setStatusTip(tr("Show the application's About box"));

}


// Add the widget to the tabbed ones
void MainWindow::tabDockWidget(VppTabDockWidget* pWidget) {

	if(tabbedWidgets_.size()){
		// Add the widget as a tab on the right of the others
		tabifyDockWidget(tabbedWidgets_.back(), pWidget);

		// Make sure the widget is on top
		// Make sure this sucker is the shown tab
		pWidget->show();
		pWidget->raise();

	}

	tabbedWidgets_.push_back(pWidget);

	// Connect the signal itemDeleted, in order to update the tabbedWidgets_ vector on deletion
	QObject::disconnect(pWidget, SIGNAL(itemDeleted(const VppTabDockWidget*)), this, SLOT(updateTabbedWidgetsVector(const VppTabDockWidget*)));
	QObject::connect(pWidget, SIGNAL(itemDeleted(const VppTabDockWidget*)), this, SLOT(updateTabbedWidgetsVector(const VppTabDockWidget*)));


}

// Slot called on deletion of a plotWidget. It assures
// the sync of the tabbedWidgets_ vector
void MainWindow::updateTabbedWidgetsVector(const VppTabDockWidget* deleteWidget) {

	if(!deleteWidget)
		return;

	std::cout<<"Sync tabbedWidget Vector on plot deletion..."<<std::endl;

	for(std::vector<VppTabDockWidget*>::iterator it=tabbedWidgets_.begin(); it!=tabbedWidgets_.end(); it++){
		if(*it==deleteWidget){
			tabbedWidgets_.erase(it);
			return;
		}
	}
}


// Make sure the vector is in sync
void MainWindow::removeWidgetFromVector(VppTabDockWidget* pWidget) {

	for(std::vector<VppTabDockWidget*>::iterator it=tabbedWidgets_.begin(); it!=tabbedWidgets_.end(); it++){

		// Remove the ptr to a destroyed widget
		if(*it == pWidget)
			tabbedWidgets_.erase(it);
	}
}

void MainWindow::import() {

	try {

		QString caption;
		QString dir;

		// Launch a file selector
		QString fileName = QFileDialog::getOpenFileName(this,caption,dir,
				tr("VPP Input File(*.vppIn);; All Files (*.*)"));

		if (!fileName.isEmpty()) {

			std::cout<<string("Opening the vpp input file... ") << fileName.toStdString() <<std::endl;

			// Instantiate a variableFileParser (and clear any previous one)
			pVariableFileParser_.reset( new VariableFileParser(fileName.toStdString()) );

			// Parse the variables file
			pVariableFileParser_->parse();

			// Instantiate the sailset
			pSails_.reset( SailSet::SailSetFactory( *pVariableFileParser_ ) );

			// Instantiate the items
			pVppItems_.reset( new VPPItemFactory(pVariableFileParser_.get(),pSails_) );

			// Populate the variable item tree accordingly
			pVariableFileParser_->populate( pVariablesWidget_->getTreeModel() );

			// SailSet also contains several variables. Append them to the bottom
			pSails_->populate( pVariablesWidget_->getTreeModel() );

			// Expand the items in the variable tree view, in order to see all the variables
			pVariablesWidget_->getView()->expandAll();

		}

	}	catch(...) {}
}

void MainWindow::run() {

	try {
		// If the boat description has not been imported we do not have the
		// vppItems nor the coefficients to be plotted!
		// If the boat description has not been imported we do not have the
		// vppItems nor the coefficients to be plotted!
		if(!hasBoatDescription())
			return;

		// todo dtrimarchi : this should be selected by a switch in the UI!
		// Instantiate a solver by default. This can be an optimizer (with opt vars)
		// or a simple solver that will keep fixed the values of the optimization vars
		//		// SolverFactory solverFactory(pVppItems);
		pSolverFactory_.reset( new Optim::NLOptSolverFactory(pVppItems_) );
		//		// SAOASolverFactory solverFactory(pVppItems);
		//		// IppOptSolverFactory solverFactory(pVppItems);

		std::cout<<"Running the VPP analysis... "<<std::endl;

		for(size_t itwv=0; itwv<5; itwv++){
			char msg[256];
			sprintf(msg,"Analyzing wind velocity %zu", itwv);
			pLogWidget_->append(msg);
		}

		// Loop on the wind ANGLES and VELOCITIES
		for(int aTW=0; aTW<pVariableFileParser_->get("N_ALPHA_TW"); aTW++)
			for(int vTW=0; vTW<pVariableFileParser_->get("N_TWV"); vTW++){

				pLogWidget_->append("vTW=" + QString(vTW) + "  -  aTW=" + QString(aTW) );

				try{

					// Run the optimizer for the current wind speed/angle
					pSolverFactory_->run(vTW,aTW);
				}
				catch(...){
					//do nothing and keep going
				}
			}

		// outer try-catch block
	}	catch(...) {}
}

void MainWindow::saveResults() {

	try {
		// Results must be available!
		if(!pSolverFactory_ ||
				!pSolverFactory_->get()->getResults() ) {
			QMessageBox msgBox;
			msgBox.setText("Please run the analysis or import results first");
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();
			return;
		}

		// todo dtrimarchi: improve the filtering to not grey out the
		// *.vpp files! See what we do in MainWIndow::importResults where
		// things work properly. Write a generic class for file selection?
		QFileDialog dialog(this);
		dialog.setWindowModality(Qt::WindowModal);
		dialog.setAcceptMode(QFileDialog::AcceptSave);
		dialog.setNameFilter(tr("VPP Result File(*.vpp)"));
		dialog.setDefaultSuffix(".vpp");
		if (dialog.exec() != QDialog::Accepted)
			return;

		// Get the file selected by the user
		const QString fileName(dialog.selectedFiles().first());
		QFile file(fileName);

		// Check the file is writable and that is is a text file
		if (!file.open(QFile::WriteOnly | QFile::Text)) {
			QMessageBox::warning(this, tr("Application"),
					tr("Cannot write file %1:\n%2.")
					.arg(QDir::toNativeSeparators(fileName),
							file.errorString()));
			return;
		}

		pSolverFactory_->get()->saveResults(fileName.toStdString());

	// outer try-catch block
	}	catch(...) {}

}

// Import VPP results from file
void MainWindow::importResults() {

	try {
		QString caption;
		QString dir;

		QString fileName = QFileDialog::getOpenFileName(this,caption,dir,
				tr("VPP Result File(*.vpp);; All Files (*.*)"));

		if (!fileName.isEmpty())
			std::cout<<"attempting to import results from : "<<fileName.toStdString()<<std::endl;

		// Instantiate an empty variableFileParser (and clear any previous one)
		// Do not parse as the variables will be read directly from the result file
		pVariableFileParser_.reset( new VariableFileParser( fileName.toStdString() ) );

		// The variableFileParser can read its part in the result file
		pVariableFileParser_->parse();

		// Instantiate the sailset
		pSails_.reset( SailSet::SailSetFactory( *pVariableFileParser_ ) );

		// Instantiate the items
		pVppItems_.reset( new VPPItemFactory(pVariableFileParser_.get(),pSails_) );

		// Populate the variable item tree accordingly
		pVariableFileParser_->populate( pVariablesWidget_->getTreeModel() );

		// SailSet also contains several variables. Append them to the bottom
		pSails_->populate( pVariablesWidget_->getTreeModel() );

		// Expand the items in the variable tree view, in order to see all the variables
		pVariablesWidget_->getView()->expandAll();

		// Instantiate a new solverFactory without vppItems_
		pSolverFactory_.reset( new Optim::NLOptSolverFactory(pVppItems_) );

		pSolverFactory_->get()->importResults(fileName.toStdString());

		// outer try-catch block
	}	catch(...) {}
}

// Add a table widget with the results
void MainWindow::tableResults() {

	try{

		// Do nothing if there are no results to show
		if(!pSolverFactory_){
			QMessageBox msgBox;
			msgBox.setText("Please run the analysis or import results first");
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();
			return;
		}

		std::cout<<"Showing the results in tabular form..."<<std::endl;

		pTableWidget_.reset( new VppTableDockWidget(pSolverFactory_->get(),this) );

		addDockWidget(Qt::TopDockWidgetArea, pTableWidget_.get());

		// Tab the widget if other widgets have already been instantiated
		// In the same area. Todo dtrimarchi : this is way too fragile
		// It requires widgets instantiated on the topDockWidgetArea and
		// I need to add the deleted signal to the slot removeWidgetFromVector
		tabDockWidget(pTableWidget_.get());

		// outer try-catch block
	} catch(...) {}
}

// Make sure a boat description has been imported. Otherwise
// warns the user with an error-like widget
bool MainWindow::hasBoatDescription() {

	// If the boat description has not been imported we do not have the
	// vppItems nor the coefficients to be plotted!
	if(!pVppItems_){
		QMessageBox msgBox;
		msgBox.setText("Please import a boat description first!");
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
		return false;
	}
	return true;
}

// Make sure a solver is available. Otherwise
// warns the user with an error-like widget
bool MainWindow::hasSolver() {

	// If the boat description has not been imported we do not have the
	// vppItems nor the coefficients to be plotted!
	if(!pSolverFactory_){
		QMessageBox msgBox;
		msgBox.setText("Please run the analysis or import some analysis results!");
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
		return false;
	}
	return true;
}

// Plot the velocity polars
void MainWindow::plotPolars() {

	// Instantiate a graphic plotting window in the central widget
	pPolarPlotWidget_.reset( new MultiplePlotWidget(this, "Polars") );

	VppPolarCustomPlotWidget* pChart = new VppPolarCustomPlotWidget("Polars","-","-", pPolarPlotWidget_.get());
	pPolarPlotWidget_->addChart(pChart,0,0);

	// Add the polar plot view to the left of the app window
	addDockWidget(Qt::TopDockWidgetArea, pPolarPlotWidget_.get());

	// Tab the widget if other widgets have already been instantiated
	// In the same area. Todo dtrimarchi : this is way too fragile
	// It requires widgets instantiated on the topDockWidgetArea and
	// I need to add the deleted signal to the slot removeWidgetFromVector
	tabDockWidget(pPolarPlotWidget_.get());

	return;

	try{

		// If the boat description has not been imported we do not have the
		// vppItems nor the coefficients to be plotted!
		// If the boat description has not been imported we do not have the
		// vppItems nor the coefficients to be plotted!
		if(!hasBoatDescription())
			return;

		if(!hasSolver())
			return;

		std::cout<<"Plotting Polars..."<<std::endl;

		// Instantiate a graphic plotting window in the central widget
		pPolarPlotWidget_.reset( new MultiplePlotWidget(this, "Polars") );

		// Hand the multiplePlotwidget to the solver factory that stores the results and knows how to plot them
		pSolverFactory_->get()->plotPolars( pPolarPlotWidget_.get() );

		// Add the polar plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pPolarPlotWidget_.get());

		// Tab the widget if other widgets have already been instantiated
		// In the same area. Todo dtrimarchi : this is way too fragile
		// It requires widgets instantiated on the topDockWidgetArea and
		// I need to add the deleted signal to the slot removeWidgetFromVector
		tabDockWidget(pPolarPlotWidget_.get());

		// outer try-catch block
	}	catch(...) {}
}


// Plot the velocity polars
void MainWindow::plotSailCoeffs() {

	try{
		// If the boat description has not been imported we do not have the
		// vppItems nor the coefficients to be plotted!
		// If the boat description has not been imported we do not have the
		// vppItems nor the coefficients to be plotted!
		if(!hasBoatDescription())
			return;

		std::cout<<"Plotting Sail coeffs..."<<std::endl;

		// Instantiate an empty multiple plot widget
		pSailCoeffPlotWidget_.reset( new MultiplePlotWidget(this,"Sail Coeffs") );

		// Hand the multiple plot to the plot method of the sailCoeffs that knows how to plot
		pVppItems_->getSailCoefficientItem()->plotInterpolatedCoefficients( pSailCoeffPlotWidget_.get() );

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pSailCoeffPlotWidget_.get() );

		tabDockWidget(pSailCoeffPlotWidget_.get());

		// outer try-catch block
	} catch(...) {}
}

// Plot the sail coefficients derivatives
void MainWindow::plot_d_SailCoeffs() {

	try{

		// If the boat description has not been imported we do not have the
		// vppItems nor the coefficients to be plotted!
		if(!hasBoatDescription())
			return;

		std::cout<<"Plotting Sail coeffs Derivatives..."<<std::endl;

		// Instantiate an empty multiple plot widget
		p_d_SailCoeffPlotWidget_.reset( new MultiplePlotWidget(this,"d(Sail Coeffs)") );

		// Hand the multiple plot to the plot method of the sailCoeffs that knows how to plot
		pVppItems_->getSailCoefficientItem()->plot_D_InterpolatedCoefficients( p_d_SailCoeffPlotWidget_.get() );

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, p_d_SailCoeffPlotWidget_.get() );

		tabDockWidget(p_d_SailCoeffPlotWidget_.get());

		// outer try-catch block
	}	catch(...) {}
}

// Plot the sail coefficients second derivatives
void MainWindow::plot_d2_SailCoeffs() {

	try{
		// If the boat description has not been imported we do not have the
		// vppItems nor the coefficients to be plotted!
		// If the boat description has not been imported we do not have the
		// vppItems nor the coefficients to be plotted!
		if(!hasBoatDescription())
			return;

		std::cout<<"Plotting Second Derivatives of the Sail coeffs..."<<std::endl;

		// Instantiate an empty multiple plot widget
		p_d2_SailCoeffPlotWidget_.reset( new MultiplePlotWidget(this,"d2(Sail Coeffs)") );

		// Hand the multiple plot to the plot method of the sailCoeffs that knows how to plot
		pVppItems_->getSailCoefficientItem()->plot_D2_InterpolatedCoefficients( p_d2_SailCoeffPlotWidget_.get() );

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, p_d2_SailCoeffPlotWidget_.get() );

		tabDockWidget(p_d2_SailCoeffPlotWidget_.get());
		// outer try-catch block
	}	catch(...) {}
}

// Plot sail forces and moments
void MainWindow::plotSailForceMoments() {

	try{
		if(!hasBoatDescription())
			return;

		std::cout<<"Plotting Sail Forces/Moments..."<<std::endl;

		// Instantiate an multiple plot widget for the force and moments plot
		pForceMomentsPlotWidget_.reset( new MultiplePlotWidget(this,"Sail Force/Moments") );

		// Ask the forces/moments to plot themself
		pVppItems_->getAeroForcesItem()->plot(pForceMomentsPlotWidget_.get());

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pForceMomentsPlotWidget_.get() );

		// Tab the widget with the others
		tabDockWidget(pForceMomentsPlotWidget_.get());

		// outer try-catch block
	} catch(...) {}
}

// Plot the total resistance
void MainWindow::plotTotalResistance() {

	try {

		if(!hasBoatDescription())
			return;

		// For which TWV, TWA shall we plot the aero forces/moments?
		WindIndicesDialog wd(pVppItems_->getWind());
		if (wd.exec() == QDialog::Rejected)
			return;

		OptimVarsStateVectorDialog sd;
		if (sd.exec() == QDialog::Rejected)
			return;

		// Instantiate an empty multiple plot widget
		pTotResistancePlotWidget_.reset( new MultiplePlotWidget(this,"Total Resistance") );

		// Ask the forces/moments to plot themself
		pTotResistancePlotWidget_->addChart(
				pVppItems_->plotTotalResistance(&wd,&sd)[0],
				0,0);

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pTotResistancePlotWidget_.get() );

		// Tab the widget with the others
		tabDockWidget(pTotResistancePlotWidget_.get());

		// outer try-catch block
	}	catch(...) {}

}

/// Plot the Frictional resistance of the hull, the keel and the rudder
void MainWindow::plotFrictionalResistance() {

	try{

		if(!hasBoatDescription())
			return;

		std::cout<<"Plotting Frictional Resistance..."<<std::endl;

		// Instantiate an empty multiple plot widget
		pFricitionalResistancePlotWidget_.reset( new MultiplePlotWidget(this,"Frictional Resistance") );

		// Add the plots generated by the items into the multiPlotWidget
		pFricitionalResistancePlotWidget_->addChart(
				pVppItems_->getFrictionalResistanceItem()->plot()[0],
				0,0);

		pFricitionalResistancePlotWidget_->addChart(
				pVppItems_->getViscousResistanceKeelItem()->plot()[0],
				0,1);

		pFricitionalResistancePlotWidget_->addChart(
				pVppItems_->getViscousResistanceRudderItem()->plot()[0],
				0,2);

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pFricitionalResistancePlotWidget_.get() );

		// Tab the widget with the others
		tabDockWidget(pFricitionalResistancePlotWidget_.get());

		// outer try-catch block
	}	catch(...) {}
}

// Plot the delta Frictional resistance due to the heel of the hull
void MainWindow::plotDelta_FrictionalResistance_Heel() {

	try{
		if(!hasBoatDescription())
			return;

		std::cout<<"Plotting Delta Frictional Resistance due to Heel..."<<std::endl;

		// For which TWV, TWA shall we plot the residuary resistance?
		WindIndicesDialog dg(pVppItems_->getWind());
		if (dg.exec() == QDialog::Rejected)
			return;

		// Instantiate an empty multiple plot widget
		p_dFrictRes_HeelPlotWidget_.reset( new MultiplePlotWidget(this,"Delta Frictional Resistance due to Heel") );

		// Plot the change of wetted area due to heel - see DSYHS99 p 116
		p_dFrictRes_HeelPlotWidget_->addChart(
				pVppItems_->getDelta_FrictionalResistance_HeelItem()->plot_deltaWettedArea_heel()[0],
				0,0);

		// Add the plot for the Delta Frictional Resistance due to Heel
		p_dFrictRes_HeelPlotWidget_->addChart(
				pVppItems_->getDelta_FrictionalResistance_HeelItem()->plot(&dg)[0],
				1,0);

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, p_dFrictRes_HeelPlotWidget_.get() );

		// Tab the widget with the others
		tabDockWidget(p_dFrictRes_HeelPlotWidget_.get());

		// outer try-catch block
	} catch(...) {}
}

// Plot the Induced resistance
void MainWindow::plotInducedResistance() {

	try{
		if(!hasBoatDescription())
			return;

		// For which TWV, TWA shall we plot the residuary resistance?
		WindIndicesDialog wd(pVppItems_->getWind());
		if (wd.exec() == QDialog::Rejected)
			return;

		OptimVarsStateVectorDialog vd;
		if( vd.exec() == QDialog::Rejected )
			return;

		// Instantiate an empty multiple plot widget
		pInducedResistancePlotWidget_.reset( new MultiplePlotWidget(this,"Induced Resistance") );

		std::vector<VppXYCustomPlotWidget*> resCharts= pVppItems_->getInducedResistanceItem()->plot(&wd,&vd);

		// Ask the frictional resistance item to plot itself
		pInducedResistancePlotWidget_->addChart(resCharts[0],0,0);

		// Ask the frictional resistance item to plot itself
		pInducedResistancePlotWidget_->addChart(resCharts[1],0,1);

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pInducedResistancePlotWidget_.get() );

		// Tab the widget with the others
		tabDockWidget(pInducedResistancePlotWidget_.get());

		// outer try-catch block
	}	catch(...) {}
}

// Plot the Residuary resistance
void MainWindow::plotResiduaryResistance() {

	try{
		if(!hasBoatDescription())
			return;

		std::cout<<"Plotting Residuary Resistance..."<<std::endl;

		// For which TWV, TWA shall we plot the residuary resistance?
		WindIndicesDialog dg(pVppItems_->getWind());
		if (dg.exec() == QDialog::Rejected)
			return;

		OptimVarsStateVectorDialog vd;
		if( vd.exec() == QDialog::Rejected )
			return;

		// Instantiate an empty multiple plot widget
		pResiduaryResistancePlotWidget_.reset( new MultiplePlotWidget(this,"Residuary Resistance") );

		// Ask the Residuary resistance items of hull and keel to plot itself
		pResiduaryResistancePlotWidget_->addChart(
				pVppItems_->getResiduaryResistanceItem()->plot(&dg, &vd)[0],
				0,0);

		pResiduaryResistancePlotWidget_->addChart(
				pVppItems_->getResiduaryResistanceKeelItem()->plot(&dg, &vd)[0],
				0,1);

		pResiduaryResistancePlotWidget_->addChart(
				pVppItems_->getDelta_ResiduaryResistance_HeelItem()->plot(&dg, &vd)[0],
				1,0);

		pResiduaryResistancePlotWidget_->addChart(
				pVppItems_->getDelta_ResiduaryResistanceKeel_HeelItem()->plot(&dg, &vd)[0],
				1,1);

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pResiduaryResistancePlotWidget_.get() );

		// Tab the widget with the others
		tabDockWidget(pResiduaryResistancePlotWidget_.get());

		// outer try-catch block
	}	catch(...) {}

}

// Plot the Negative resistance
void MainWindow::plotNegativeResistance() {

	try{
		if(!hasBoatDescription())
			return;

		std::cout<<"Plotting Negative Resistance..."<<std::endl;

		// Instantiate an empty multiple plot widget
		pNegativeResistancePlotWidget_.reset( new MultiplePlotWidget(this,"Negative Resistance") );

		// Ask the frictional resistance item to plot itself
		pNegativeResistancePlotWidget_->addChart(
				pVppItems_->getNegativeResistanceItem()->plot()[0],
				0,0);

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pNegativeResistancePlotWidget_.get() );

		// Tab the widget with the others
		tabDockWidget(pNegativeResistancePlotWidget_.get());

		// outer try-catch block
	}	catch(...) {}
}

// Temp method used to test QCustomPlot in the current env
// This method shall be removed at some point todo dtrimarchi
void MainWindow::testQCustomPlot() {

	try{
		// Init the widget that will be containing this plot
		pXYPlotWidget_.reset( new VppCustomPlotWidget(this) );

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pXYPlotWidget_.get() );

		// Tabify the dockwidget
		tabDockWidget(pXYPlotWidget_.get());

		// outer try-catch block
	}	catch(...) {}
}

// Add a 3dPlot widget
void MainWindow::plotOptimizationSpace() {

	try{

		if(!hasBoatDescription())
			return;

		std::cout<<"Plotting the optimization space..."<<std::endl;

		// For which TWV, TWA shall we plot the aero forces/moments?
		WindIndicesDialog wd(pVppItems_->getWind());
		if (wd.exec() == QDialog::Rejected)
			return;

		OptimVarsStateVectorDialog sd;
		if (sd.exec() == QDialog::Rejected)
			return;

		// This widget is to be assigned to a dockable widget
		p3dPlotWidget_.reset(new ThreeDPlotWidget(this) );

		p3dPlotWidget_->addChart(
				pVppItems_->plotOptimizationSpace(wd,sd)
		);

		// Add the 3d plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, p3dPlotWidget_.get());

		// Tab the widget if other widgets have already been instantiated
		// In the same area. Todo dtrimarchi : this is way too fragile
		// It requires widgets instantiated on the topDockWidgetArea and
		// I need to add the deleted signal to the slot removeWidgetFromVector
		tabDockWidget(p3dPlotWidget_.get());

		// outer try-catch block
	} catch(...) {}
}

// Plot the gradient of the solution
// Grad(u) = | du/du du/dPhi  du/db  du/df  |
void MainWindow::plotGradient() {

	try{
		if(!hasBoatDescription())
			return;

		// For which TWV, TWA shall we plot the Jacobian?
		WindIndicesDialog wd(pVppItems_->getWind());
		if (wd.exec() == QDialog::Rejected)
			return;

		// Ask the state vector (defines a linearization point)
		FullStateVectorDialog sd;
		if (sd.exec() == QDialog::Rejected)
			return;

		std::cout<<"Plotting Gradient..."<<std::endl;

		VectorXd x = sd.getStateVector();

		// Instantiate a Gradient
		VPPGradient G(x,pVppItems_.get());

		pGradientPlotWidget_.reset(new MultiplePlotWidget(this,"VPP Gradient"));

		// Get all of the plots the Gradient is up to draw
		std::vector<VppXYCustomPlotWidget*> gPlotVector( G.plot(wd) );

		// Send the plots to the widget and arrange them in 2xn ordering
		size_t dx=0, dy=0;
		for(size_t i=0; i<gPlotVector.size(); i++){
			pGradientPlotWidget_->addChart( gPlotVector[i], dx++, dy );
			if(dx==2){
				dx=0;
				dy++;
			}
		}

		// Add the plot view to the top of the app window
		addDockWidget(Qt::TopDockWidgetArea, pGradientPlotWidget_.get());

		// Tab the widget if other widgets have already been instantiated
		// In the same area.
		tabDockWidget(pGradientPlotWidget_.get());

		// outer try-catch block
	}	catch(...) {}
}

// Plot the Jacobian of the solution, defined as:
// J = | dF/du dF/dPhi |
//	   | dM/du dM/dPhi |
// so that 	|dF| = J 	|du  |
//					|dM|			|dPhi|
void MainWindow::plotJacobian() {

	try{
		if(!hasBoatDescription())
			return;

		// For which TWV, TWA shall we plot the Jacobian?
		WindIndicesDialog wd(pVppItems_->getWind());
		if (wd.exec() == QDialog::Rejected)
			return;

		// Ask the state vector (defines a linearization point)
		FullStateVectorDialog sd;
		if (sd.exec() == QDialog::Rejected)
			return;

		// Define the size of the sub-pb. Here we have 2 state
		//  vars (subPbSize) and 2 optim vars
		size_t subPbsize=2;

		VectorXd x = sd.getStateVector();

		// Instantiate a Jacobian
		VPPJacobian J(x, pVppItems_.get(), subPbsize);

		// Instantiate a widget container for this plot
		pJacobianPlotWidget_.reset(new MultiplePlotWidget(this,"VPP Jacobian"));

		// Get all of the plots the Jacobian is up to draw
		std::vector<VppXYCustomPlotWidget*> jPlotVector( J.plot(wd) );

		// Send the plots to the widget and arrange them in 2xn ordering
		size_t dx=0, dy=0;
		for(size_t i=0; i<jPlotVector.size(); i++){
			pJacobianPlotWidget_->addChart( jPlotVector[i], dx++, dy );
			if(dx==2){
				dx=0;
				dy++;
			}
		}

		// Add the plot view to the top of the app window
		addDockWidget(Qt::TopDockWidgetArea, pJacobianPlotWidget_.get());

		// Tab the widget if other widgets have already been instantiated
		// In the same area.
		tabDockWidget(pJacobianPlotWidget_.get());

		// outer try-catch block
	}	catch(...) {}
}

void MainWindow::about() {

	QString text( "Branch: " );
	text += currentBranch;
	text += "\n";
	text += "Revision number: ";
	text += currentRevNumber;
	text += "\n";
	text += "Commit hash: ";
	text += currentHash;
	text += "\n";
	text += "Build on: ";
	text += buildDate;

	QMessageBox::about(this, tr("About VPP"),text);

}


void MainWindow::actionTriggered(QAction *action) {
	qDebug("action '%s' triggered", action->text().toLocal8Bit().data());
}

void MainWindow::setupToolBar() {

#ifdef Q_OS_OSX
	setUnifiedTitleAndToolBarOnMac(true);
#endif

	// Instantiate the toolbar
	pToolBar_= addToolBar(tr("File"));

	// Set the size of the icons in the toolbar
	QSize iconSize(20,20);
	pToolBar_->setIconSize(iconSize);

}
