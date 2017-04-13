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
	pLogWidget_->append("=======================");
	pLogWidget_->append("===  V++ PROGRAM  =======");
	pLogWidget_->append("=======================");

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

}

// Virtual destructor
MainWindow::~MainWindow() {

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

	pVppActionMenu_->addSeparator();

	pVppActionMenu_->addAction(tr("&Quit"), this, &QWidget::close);

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
	pPreferencesMenu_->addSeparator();
	pPreferencesMenu_->addAction(tr("&VPP version"), this, &MainWindow::about);

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

	pLogWidget_->append("Sync tabbedWidget Vector on plot deletion...");

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

// Append a string to the log
void MainWindow::appendToLog(const QString &text) {

	pLogWidget_->append(text);
}

void MainWindow::import() {

	try {

		QString caption;
		QString dir;

		// Launch a file selector
		QString fileName = QFileDialog::getOpenFileName(this,caption,dir,
				tr("VPP Input File(*.vppIn);; All Files (*.*)"));

		if (!fileName.isEmpty()) {

			pLogWidget_->append(QString("Opening the vpp input file... ") + fileName );

			// Instantiate a variableFileParser (and clear any previous one)
			pVariableFileParser_.reset( new VariableFileParser(fileName.toStdString()) );

			// Parse the variables file
			pVariableFileParser_->parse();

			// Instantiate the sailset
			pSails_.reset( SailSet::SailSetFactory( *pVariableFileParser_ ) );

			// Instantiate the items
			pVppItems_.reset( new VPPItemFactory(pVariableFileParser_.get(),pSails_) );

			// Populate the variable item tree accordingly
			pVariableFileParser_->populate( pVariablesWidget_->getModel() );

			// SailSet also contains several variables. Append them to the bottom
			pSails_->populate( pVariablesWidget_->getModel() );

			// Expand the items in the variable tree view, in order to see all the variables
			pVariablesWidget_->getView()->expandAll();

		}

	} catch(std::exception& e) {
		pLogWidget_->append("\n-----------------------------------------");
		pLogWidget_->append(" Exception caught in Main:  ");
		pLogWidget_->append( e.what() );
		pLogWidget_->append("\n-----------------------------------------");
	}	catch(...) {
		pLogWidget_->append("Unknown Exception occurred\n");
	}


}

void MainWindow::run() {
	// todo dtrimarchi
	// Verify that the variable tree is not empty
	// run the VPP analysis
	pLogWidget_->append("Running the VPP analysis... ");

	for(size_t itwv=0; itwv<5; itwv++){
		char msg[256];
		sprintf(msg,"Analyzing wind velocity %zu", itwv);
		pLogWidget_->append(msg);
	}

}

bool MainWindow::saveResults() {

	pLogWidget_->append("Saving the analyis results... ");
	QFileDialog dialog(this);
	dialog.setWindowModality(Qt::WindowModal);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setNameFilter(tr("VPP Result File(*.vpp)"));
	dialog.setDefaultSuffix(".vpp");
	if (dialog.exec() != QDialog::Accepted)
		return false;
	return saveFile(dialog.selectedFiles().first());
}

bool MainWindow::saveFile(const QString &fileName) {

	pLogWidget_->append("Saving the VPP results to file");

	// if save was successiful, return true, otherwise false
	//    QFile file(fileName);
	//    if (!file.open(QFile::WriteOnly | QFile::Text)) {
	//        QMessageBox::warning(this, tr("Application"),
	//                             tr("Cannot write file %1:\n%2.")
	//                             .arg(QDir::toNativeSeparators(fileName),
	//                                  file.errorString()));
	//        return false;
	//    }

	return true;
}

// Import VPP results from file
void MainWindow::importResults() {

	QString caption;
	QString dir;

	QString fileName = QFileDialog::getOpenFileName(this,caption,dir,
			tr("VPP Result File(*.vpp);; All Files (*.*)"));

	if (!fileName.isEmpty())
		//loadFile(fileName);
		pLogWidget_->append("Importing the analysis results...");

}

// Add a table widget with the results
void MainWindow::tableResults() {

	pLogWidget_->append("Showing the results in tabular form...");

	pTableWidget_.reset( new VppTableDockWidget(this) );
	addDockWidget(Qt::TopDockWidgetArea, pTableWidget_.get());

	// Tab the widget if other widgets have already been instantiated
	// In the same area. Todo dtrimarchi : this is way too fragile
	// It requires widgets instantiated on the topDockWidgetArea and
	// I need to add the deleted signal to the slot removeWidgetFromVector
	tabDockWidget(pTableWidget_.get());

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


// Plot the velocity polars
void MainWindow::plotPolars() {

	pLogWidget_->append("Plotting Polars...");

	// Instantiate a graphic plotting window in the central widget
	pPolarPlotWidget_.reset( new PolarPlotWidget(this) );
	addDockWidget(Qt::TopDockWidgetArea, pPolarPlotWidget_.get());

	// Tab the widget if other widgets have already been instantiated
	// In the same area. Todo dtrimarchi : this is way too fragile
	// It requires widgets instantiated on the topDockWidgetArea and
	// I need to add the deleted signal to the slot removeWidgetFromVector
	tabDockWidget(pPolarPlotWidget_.get());
}

// Plot the velocity polars
void MainWindow::plotSailCoeffs() {

	// If the boat description has not been imported we do not have the
	// vppItems nor the coefficients to be plotted!
	// If the boat description has not been imported we do not have the
	// vppItems nor the coefficients to be plotted!
	if(!hasBoatDescription())
		return;

	pLogWidget_->append("Plotting Sail coeffs...");

	// Instantiate an empty multiple plot widget
	pSailCoeffPlotWidget_.reset( new MultiplePlotWidget(this,"Sail Coeffs") );

	// Hand the multiple plot to the plot method of the sailCoeffs that knows how to plot
	pVppItems_->getSailCoefficientItem()->plotInterpolatedCoefficients( pSailCoeffPlotWidget_.get() );

	// Add the xy plot view to the left of the app window
	addDockWidget(Qt::TopDockWidgetArea, pSailCoeffPlotWidget_.get() );

	tabDockWidget(pSailCoeffPlotWidget_.get());

}

// Plot the sail coefficients derivatives
void MainWindow::plot_d_SailCoeffs() {

	// If the boat description has not been imported we do not have the
	// vppItems nor the coefficients to be plotted!
	if(!hasBoatDescription())
		return;

	pLogWidget_->append("Plotting Sail coeffs Derivatives...");

	// Instantiate an empty multiple plot widget
	p_d_SailCoeffPlotWidget_.reset( new MultiplePlotWidget(this,"d(Sail Coeffs)") );

	// Hand the multiple plot to the plot method of the sailCoeffs that knows how to plot
	pVppItems_->getSailCoefficientItem()->plot_D_InterpolatedCoefficients( p_d_SailCoeffPlotWidget_.get() );

	// Add the xy plot view to the left of the app window
	addDockWidget(Qt::TopDockWidgetArea, p_d_SailCoeffPlotWidget_.get() );

	tabDockWidget(p_d_SailCoeffPlotWidget_.get());
}

// Plot the sail coefficients second derivatives
void MainWindow::plot_d2_SailCoeffs() {

	// If the boat description has not been imported we do not have the
	// vppItems nor the coefficients to be plotted!
	// If the boat description has not been imported we do not have the
	// vppItems nor the coefficients to be plotted!
	if(!hasBoatDescription())
		return;

	pLogWidget_->append("Plotting Second Derivatives of the Sail coeffs...");

	// Instantiate an empty multiple plot widget
	p_d2_SailCoeffPlotWidget_.reset( new MultiplePlotWidget(this,"d2(Sail Coeffs)") );

	// Hand the multiple plot to the plot method of the sailCoeffs that knows how to plot
	pVppItems_->getSailCoefficientItem()->plot_D2_InterpolatedCoefficients( p_d2_SailCoeffPlotWidget_.get() );

	// Add the xy plot view to the left of the app window
	addDockWidget(Qt::TopDockWidgetArea, p_d2_SailCoeffPlotWidget_.get() );

	tabDockWidget(p_d2_SailCoeffPlotWidget_.get());
}

// Plot sail forces and moments
void MainWindow::plotSailForceMoments() {

	if(!hasBoatDescription())
		return;

	pLogWidget_->append("Plotting Sail Forces/Moments...");

	// Instantiate an multiple plot widget for the force and moments plot
	pForceMomentsPlotWidget_.reset( new MultiplePlotWidget(this,"Sail Force/Moments") );

	// Ask the forces/moments to plot themself
	pVppItems_->getAeroForcesItem()->plot(pForceMomentsPlotWidget_.get());

	// Add the xy plot view to the left of the app window
	addDockWidget(Qt::TopDockWidgetArea, pForceMomentsPlotWidget_.get() );

	// Tab the widget with the others
	tabDockWidget(pForceMomentsPlotWidget_.get());

}

// Plot the total resistance
void MainWindow::plotTotalResistance() {

	if(!hasBoatDescription())
		return;

	pLogWidget_->append("Plotting Total Resistance...");

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

}

/// Plot the Frictional resistance of the hull, the keel and the rudder
void MainWindow::plotFrictionalResistance() {

	if(!hasBoatDescription())
		return;

	pLogWidget_->append("Plotting Frictional Resistance...");

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

}

// Plot the delta Frictional resistance due to the heel of the hull
void MainWindow::plotDelta_FrictionalResistance_Heel() {

	if(!hasBoatDescription())
		return;

	pLogWidget_->append("Plotting Delta Frictional Resistance due to Heel...");

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

}

// Plot the Induced resistance
void MainWindow::plotInducedResistance() {

	if(!hasBoatDescription())
		return;

	// For which TWV, TWA shall we plot the residuary resistance?
	WindIndicesDialog wd(pVppItems_->getWind());
	if (wd.exec() == QDialog::Rejected)
		return;

	OptimVarsStateVectorDialog vd;
	if( vd.exec() == QDialog::Rejected )
		return;

	pLogWidget_->append("Plotting Induced Resistance...");

	// Instantiate an empty multiple plot widget
	pInducedResistancePlotWidget_.reset( new MultiplePlotWidget(this,"Induced Resistance") );

	// Ask the frictional resistance item to plot itself
	pInducedResistancePlotWidget_->addChart(
			pVppItems_->getInducedResistanceItem()->plot(&wd,&vd)[0],
			0,0);

	// Ask the frictional resistance item to plot itself
	pInducedResistancePlotWidget_->addChart(
			pVppItems_->getInducedResistanceItem()->plot(&wd,&vd)[1],
			0,1);

	// Add the xy plot view to the left of the app window
	addDockWidget(Qt::TopDockWidgetArea, pInducedResistancePlotWidget_.get() );

	// Tab the widget with the others
	tabDockWidget(pInducedResistancePlotWidget_.get());

}

// Plot the Residuary resistance
void MainWindow::plotResiduaryResistance() {

	if(!hasBoatDescription())
		return;

	pLogWidget_->append("Plotting Residuary Resistance...");

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

}



// Plot the Negative resistance
void MainWindow::plotNegativeResistance() {

	if(!hasBoatDescription())
		return;

	pLogWidget_->append("Plotting Negative Resistance...");

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

}

// Temp method used to test QCustomPlot in the current env
// This method shall be removed at some point todo dtrimarchi
void MainWindow::testQCustomPlot() {

	// Init the widget that will be containing this plot
	pXYPlotWidget_.reset( new VppCustomPlotWidget(this) );

	// Add the xy plot view to the left of the app window
	addDockWidget(Qt::TopDockWidgetArea, pXYPlotWidget_.get() );

	// Tabify the dockwidget
	tabDockWidget(pXYPlotWidget_.get());

}

// Add a 3dPlot widget
void MainWindow::plotOptimizationSpace() {

	if(!hasBoatDescription())
		return;

	pLogWidget_->append("Plotting the optimization space...");

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

}

// Plot the gradient of the solution
// Grad(u) = | du/du du/dPhi  du/db  du/df  |
void MainWindow::plotGradient() {

}

// Plot the Jacobian of the solution
// J = | dF/du dF/dPhi |	|du	 |
//	    | dM/du dM/dPhi |	|dPhi|
void MainWindow::plotJacobian() {

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

	pLogWidget_->append("Plotting Jacobian...");

	// Define the size of the sub-pb. Here we have 2 state
	//  vars (subPbSize) and 2 optim vars
	size_t subPbsize=2;

	VectorXd x = sd.getStateVector();

	// Instantiate a Jacobian
	VPPJacobian J(x, pVppItems_.get(), subPbsize);

	// Old style plot - test to be removed!
	// J.testPlot(wd.getTWV(), wd.getTWA());

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

}

void MainWindow::about() {
	// todo dtrimarchi
	// Printout info about the build
	pLogWidget_->append("About VPP...");

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
