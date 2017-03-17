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
	// ==== WARNING !!! ===================================================================
	// Try to define this guy as a class member and see the app crashing on construction...
	// This ONLY happens for the optimized build. Will building profile in xCode help?
	QMenu* pSailCoeffsMenu = new QMenu("Plot Sail Related Quantities", this);
	pSailCoeffsMenu->setIcon( QIcon::fromTheme("Plot Sail Coeffs", QIcon(":/icons/sailCoeffs.png")) );
	pToolBar_->addAction(pSailCoeffsMenu->menuAction());

	// Plot sail coeffs
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Sail Coeffs", QIcon(":/icons/sailCoeffs.png")),
					tr("&Sail Coeffs"), this)
			) );
	QAction* plotSailCoeffsAction = actionVector_.back().get();
	plotSailCoeffsAction->setStatusTip(tr("Plot Sail Coeffs"));
	pSailCoeffsMenu->addAction(plotSailCoeffsAction);
	connect(plotSailCoeffsAction, &QAction::triggered, this, &MainWindow::plotSailCoeffs);

	// Plot sail coeffs derivatives
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Sail Coeffs Derivatives", QIcon(":/icons/d_sailCoeffs.png")),
					tr("&d(Sail Coeffs)"), this)
	) );
	QAction* plot_d_SailCoeffsAction= actionVector_.back().get();
	plot_d_SailCoeffsAction->setStatusTip(tr("Plot d(Sail Coeffs)"));
	pSailCoeffsMenu->addAction(plot_d_SailCoeffsAction);
	connect(plot_d_SailCoeffsAction, &QAction::triggered, this, &MainWindow::plot_d_SailCoeffs);

	// Plot sail coeffs second derivatives
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Sail Coeffs Second Derivatives", QIcon(":/icons/d2_sailCoeffs.png")),
					tr("&d2(Sail Coeffs)"), this)
			) );
	QAction* plot_d2_SailCoeffsAction = actionVector_.back().get();
	plot_d2_SailCoeffsAction->setStatusTip(tr("Plot d2(Sail Coeffs)"));
	pSailCoeffsMenu->addAction(plot_d2_SailCoeffsAction);
	connect(plot_d2_SailCoeffsAction, &QAction::triggered, this, &MainWindow::plot_d2_SailCoeffs);

	// Plot sail force and moments
	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot Sail Force/Moments", QIcon(":/icons/sailForceMoment.png")),
					tr("&Plot Sail Force/Moments"), this)
			) );
	QAction* plotSailForceMomentAction = actionVector_.back().get();
	plotSailForceMomentAction->setStatusTip(tr("Plot Sail Force/Moments"));
	pSailCoeffsMenu->addAction(plotSailForceMomentAction);
	connect(plotSailForceMomentAction, &QAction::triggered, this, &MainWindow::plotSailForceMoments);

	// --

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


	// --

	actionVector_.push_back( boost::shared_ptr<QAction>(
			new QAction(
					QIcon::fromTheme("Plot 3d", QIcon(":/icons/plot3d.png")),
					tr("&3d Plot"), this)
	) );
	QAction* plot3dAction = actionVector_.back().get();
	plot3dAction->setStatusTip(tr("Plot 3d"));
	connect(plot3dAction, &QAction::triggered, this, &MainWindow::threedPlot);
	pToolBar_->addAction(plot3dAction);

	// ---

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

	// Instantiate an empty multiple plot widget
	pTotResistancePlotWidget_.reset( new MultiplePlotWidget(this,"Total Resistance") );

	// Ask the forces/moments to plot themself
	pVppItems_->plotTotalResistance(pTotResistancePlotWidget_.get());

	// Add the xy plot view to the left of the app window
	addDockWidget(Qt::TopDockWidgetArea, pTotResistancePlotWidget_.get() );

	// Tab the widget with the others
	tabDockWidget(pTotResistancePlotWidget_.get());

}

// Plot the Frictional resistance
void MainWindow::plotFrictionalResistance() {

	if(!hasBoatDescription())
		return;

	pLogWidget_->append("Plotting Frictional Resistance...");

	// Instantiate an empty multiple plot widget
	pFricitionalResistancePlotWidget_.reset( new MultiplePlotWidget(this,"Frictional Resistance") );

	// Ask the frictional resistance item to plot itself
	pVppItems_->getFrictionalResistanceItem()->plot(pFricitionalResistancePlotWidget_.get());

	// Add the xy plot view to the left of the app window
	addDockWidget(Qt::TopDockWidgetArea, pFricitionalResistancePlotWidget_.get() );

	// Tab the widget with the others
	tabDockWidget(pFricitionalResistancePlotWidget_.get());

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
void MainWindow::threedPlot() {

	pLogWidget_->append("Adding a 3d plot window...");

	// This widget is to be assigned to a dockable widget
	p3dPlotWidget_.reset(new ThreeDPlotWidget(this) );

	// Add the 3d plot view to the left of the app window
	addDockWidget(Qt::TopDockWidgetArea, p3dPlotWidget_.get());

	// Tab the widget if other widgets have already been instantiated
	// In the same area. Todo dtrimarchi : this is way too fragile
	// It requires widgets instantiated on the topDockWidgetArea and
	// I need to add the deleted signal to the slot removeWidgetFromVector
	tabDockWidget(p3dPlotWidget_.get());

}

void MainWindow::about() {
	// todo dtrimarchi
	// Printout info about the build
	pLogWidget_->append("About VPP...");

}


void MainWindow::actionTriggered(QAction *action) {
	qDebug("action '%s' triggered", action->text().toLocal8Bit().data());
}

void MainWindow::setupToolBar()
{
#ifdef Q_OS_OSX
	setUnifiedTitleAndToolBarOnMac(true);
#endif

	// Instantiate the toolbar
	pToolBar_= addToolBar(tr("File"));

	// Set the size of the icons in the toolbar
	QSize iconSize(20,20);
	pToolBar_->setIconSize(iconSize);

}


//void MainWindow::setDockOptions()
//{
//    DockOptions opts;
//    QList<QAction*> actions = mainWindowMenu->actions();
//
//    if (actions.at(0)->isChecked())
//        opts |= AnimatedDocks;
//    if (actions.at(1)->isChecked())
//        opts |= AllowNestedDocks;
//    if (actions.at(2)->isChecked())
//        opts |= AllowTabbedDocks;
//    if (actions.at(3)->isChecked())
//        opts |= ForceTabbedDocks;
//    if (actions.at(4)->isChecked())
//        opts |= VerticalTabs;
//    if (actions.at(5)->isChecked())
//        opts |= GroupedDragging;
//
//    QMainWindow::setDockOptions(opts);
//}

//void MainWindow::saveLayout()
//{
//    QString fileName
//        = QFileDialog::getSaveFileName(this, tr("Save layout"));
//    if (fileName.isEmpty())
//        return;
//    QFile file(fileName);
//    if (!file.open(QFile::WriteOnly)) {
//        QString msg = tr("Failed to open %1\n%2")
//                        .arg(QDir::toNativeSeparators(fileName), file.errorString());
//        QMessageBox::warning(this, tr("Error"), msg);
//        return;
//    }
//
//    QByteArray geo_data = saveGeometry();
//    QByteArray layout_data = saveState();
//
//    bool ok = file.putChar((uchar)geo_data.size());
//    if (ok)
//        ok = file.write(geo_data) == geo_data.size();
//    if (ok)
//        ok = file.write(layout_data) == layout_data.size();
//
//    if (!ok) {
//        QString msg = tr("Error writing to %1\n%2")
//                        .arg(QDir::toNativeSeparators(fileName), file.errorString());
//        QMessageBox::warning(this, tr("Error"), msg);
//        return;
//    }
//}

//void MainWindow::loadLayout()
//{
//    QString fileName
//        = QFileDialog::getOpenFileName(this, tr("Load layout"));
//    if (fileName.isEmpty())
//        return;
//    QFile file(fileName);
//    if (!file.open(QFile::ReadOnly)) {
//        QString msg = tr("Failed to open %1\n%2")
//                        .arg(QDir::toNativeSeparators(fileName), file.errorString());
//        QMessageBox::warning(this, tr("Error"), msg);
//        return;
//    }
//
//    uchar geo_size;
//    QByteArray geo_data;
//    QByteArray layout_data;
//
//    bool ok = file.getChar((char*)&geo_size);
//    if (ok) {
//        geo_data = file.read(geo_size);
//        ok = geo_data.size() == geo_size;
//    }
//    if (ok) {
//        layout_data = file.readAll();
//        ok = layout_data.size() > 0;
//    }
//
//    if (ok)
//        ok = restoreGeometry(geo_data);
//    if (ok)
//        ok = restoreState(layout_data);
//
//    if (!ok) {
//        QString msg = tr("Error reading %1").arg(QDir::toNativeSeparators(fileName));
//        QMessageBox::warning(this, tr("Error"), msg);
//        return;
//    }
//}

//class DockWidgetAreaCornerFunctor {
//public:
//    explicit DockWidgetAreaCornerFunctor(QMainWindow *mw, Qt::Corner c, Qt::DockWidgetArea a)
//        : m_mainWindow(mw), m_area(a), m_corner(c) {}
//
//    void operator()() const { m_mainWindow->setCorner(m_corner, m_area); }
//
//private:
//    QMainWindow *m_mainWindow;
//    Qt::DockWidgetArea m_area;
//    Qt::Corner m_corner;
//};

//static QAction *addCornerAction(const QString &text, QMainWindow *mw, QMenu *menu, QActionGroup *group,
//                                Qt::Corner c, Qt::DockWidgetArea a)
//{
//    QAction *result = menu->addAction(text, mw, DockWidgetAreaCornerFunctor(mw, c, a));
//    result->setCheckable(true);
//    group->addAction(result);
//    return result;
//}

//void MainWindow::setupDockWidgets(const CustomSizeHintMap &customSizeHints)
//{
//    qRegisterMetaType<QDockWidget::DockWidgetFeatures>();
//
//    QMenu *cornerMenu = dockWidgetMenu->addMenu(tr("Top left corner"));
//    QActionGroup *group = new QActionGroup(this);
//    group->setExclusive(true);
//    QAction *cornerAction = addCornerAction(tr("Top dock area"), this, cornerMenu, group, Qt::TopLeftCorner, Qt::TopDockWidgetArea);
//    cornerAction->setChecked(true);
//    addCornerAction(tr("Left dock area"), this, cornerMenu, group, Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
//
//    cornerMenu = dockWidgetMenu->addMenu(tr("Top right corner"));
//    group = new QActionGroup(this);
//    group->setExclusive(true);
//    cornerAction = addCornerAction(tr("Top dock area"), this, cornerMenu, group, Qt::TopRightCorner, Qt::TopDockWidgetArea);
//    cornerAction->setChecked(true);
//    addCornerAction(tr("Right dock area"), this, cornerMenu, group, Qt::TopRightCorner, Qt::RightDockWidgetArea);
//
//    cornerMenu = dockWidgetMenu->addMenu(tr("Bottom left corner"));
//    group = new QActionGroup(this);
//    group->setExclusive(true);
//    cornerAction = addCornerAction(tr("Bottom dock area"), this, cornerMenu, group, Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
//    cornerAction->setChecked(true);
//    addCornerAction(tr("Left dock area"), this, cornerMenu, group, Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
//
//    cornerMenu = dockWidgetMenu->addMenu(tr("Bottom right corner"));
//    group = new QActionGroup(this);
//    group->setExclusive(true);
//    cornerAction = addCornerAction(tr("Bottom dock area"), this, cornerMenu, group, Qt::BottomRightCorner, Qt::BottomDockWidgetArea);
//    cornerAction->setChecked(true);
//    addCornerAction(tr("Right dock area"), this, cornerMenu, group, Qt::BottomRightCorner, Qt::RightDockWidgetArea);
//
//    dockWidgetMenu->addSeparator();
//
//    static const struct Set {
//        const char * name;
//        uint flags;
//        Qt::DockWidgetArea area;
//    } sets [] = {
//#ifndef Q_OS_MAC
//        { "Black", 0, Qt::LeftDockWidgetArea },
//#else
//        { "Black", Qt::Drawer, Qt::LeftDockWidgetArea },
//#endif
//        { "White", 0, Qt::RightDockWidgetArea },
//        { "Red", 0, Qt::TopDockWidgetArea },
//        { "Green", 0, Qt::TopDockWidgetArea },
//        { "Blue", 0, Qt::BottomDockWidgetArea },
//        { "Yellow", 0, Qt::BottomDockWidgetArea }
//    };
//    const int setCount = sizeof(sets) / sizeof(Set);
//
//    const QIcon qtIcon(QPixmap(":/res/qt.png"));
//    for (int i = 0; i < setCount; ++i) {
//        ColorSwatch *swatch = new ColorSwatch(tr(sets[i].name), this, Qt::WindowFlags(sets[i].flags));
//        if (i % 2)
//            swatch->setWindowIcon(qtIcon);
//        if (qstrcmp(sets[i].name, "Blue") == 0) {
//            BlueTitleBar *titlebar = new BlueTitleBar(swatch);
//            swatch->setTitleBarWidget(titlebar);
//            connect(swatch, &QDockWidget::topLevelChanged, titlebar, &BlueTitleBar::updateMask);
//            connect(swatch, &QDockWidget::featuresChanged, titlebar, &BlueTitleBar::updateMask, Qt::QueuedConnection);
//        }
//
//        QString name = QString::fromLatin1(sets[i].name);
//        if (customSizeHints.contains(name))
//            swatch->setCustomSizeHint(customSizeHints.value(name));
//
//        addDockWidget(sets[i].area, swatch);
//        dockWidgetMenu->addMenu(swatch->colorSwatchMenu());
//    }
//
//    destroyDockWidgetMenu = new QMenu(tr("Destroy dock widget"), this);
//    destroyDockWidgetMenu->setEnabled(false);
//    connect(destroyDockWidgetMenu, &QMenu::triggered, this, &MainWindow::destroyDockWidget);
//
//    dockWidgetMenu->addSeparator();
//    dockWidgetMenu->addAction(tr("Add dock widget..."), this, &MainWindow::createDockWidget);
//    dockWidgetMenu->addMenu(destroyDockWidgetMenu);
//}

//void MainWindow::switchLayoutDirection()
//{
//    if (layoutDirection() == Qt::LeftToRight)
//        QApplication::setLayoutDirection(Qt::RightToLeft);
//    else
//        QApplication::setLayoutDirection(Qt::LeftToRight);
//}

//class CreateDockWidgetDialog : public QDialog
//{
//public:
//    explicit CreateDockWidgetDialog(QWidget *parent = Q_NULLPTR);
//
//    QString enteredObjectName() const { return m_objectName->text(); }
//    Qt::DockWidgetArea location() const;
//
//private:
//    QLineEdit *m_objectName;
//    QComboBox *m_location;
//};

//CreateDockWidgetDialog::CreateDockWidgetDialog(QWidget *parent)
//    : QDialog(parent)
//    , m_objectName(new QLineEdit(this))
//    , m_location(new QComboBox(this))
//{
//    setWindowTitle(tr("Add Dock Widget"));
//    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
//    QGridLayout *layout = new QGridLayout(this);
//
//    layout->addWidget(new QLabel(tr("Object name:")), 0, 0);
//    layout->addWidget(m_objectName, 0, 1);
//
//    layout->addWidget(new QLabel(tr("Location:")), 1, 0);
//    m_location->setEditable(false);
//    m_location->addItem(tr("Top"));
//    m_location->addItem(tr("Left"));
//    m_location->addItem(tr("Right"));
//    m_location->addItem(tr("Bottom"));
//    m_location->addItem(tr("Restore"));
//    layout->addWidget(m_location, 1, 1);
//
//    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
//    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
//    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
//    layout->addWidget(buttonBox, 2, 0, 1, 2);
//}

//Qt::DockWidgetArea CreateDockWidgetDialog::location() const
//{
//    switch (m_location->currentIndex()) {
//        case 0: return Qt::TopDockWidgetArea;
//        case 1: return Qt::LeftDockWidgetArea;
//        case 2: return Qt::RightDockWidgetArea;
//        case 3: return Qt::BottomDockWidgetArea;
//        default:
//            break;
//    }
//    return Qt::NoDockWidgetArea;
//}

//void MainWindow::createDockWidget()
//{
//    CreateDockWidgetDialog dialog(this);
//    if (dialog.exec() == QDialog::Rejected)
//        return;
//
//    QDockWidget *dw = new QDockWidget;
//    const QString name = dialog.enteredObjectName();
//    dw->setObjectName(name);
//    dw->setWindowTitle(name);
//    dw->setWidget(new QTextEdit);
//
//    Qt::DockWidgetArea area = dialog.location();
//    switch (area) {
//        case Qt::LeftDockWidgetArea:
//        case Qt::RightDockWidgetArea:
//        case Qt::TopDockWidgetArea:
//        case Qt::BottomDockWidgetArea:
//            addDockWidget(area, dw);
//            break;
//        default:
//            if (!restoreDockWidget(dw)) {
//                QMessageBox::warning(this, QString(), tr("Failed to restore dock widget"));
//                delete dw;
//                return;
//            }
//            break;
//    }
//
//    extraDockWidgets.append(dw);
//    destroyDockWidgetMenu->setEnabled(true);
//    destroyDockWidgetMenu->addAction(new QAction(name, this));
//}

//void MainWindow::destroyDockWidget(QAction *action)
//{
//    int index = destroyDockWidgetMenu->actions().indexOf(action);
//    delete extraDockWidgets.takeAt(index);
//    destroyDockWidgetMenu->removeAction(action);
//    action->deleteLater();
//
//    if (destroyDockWidgetMenu->isEmpty())
//        destroyDockWidgetMenu->setEnabled(false);
//}
