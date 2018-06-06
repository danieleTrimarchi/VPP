#include "MainWindow.h"

#include <stdio.h>
#include <iostream>

#include <QtWidgets/QDockWidget>
#include <QtWidgets/QLayout>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtCore/QFile>
#include <QtCore/QDataStream>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <QtCore/QSignalMapper>
#include <QtWidgets/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtCore>
#include <QtGui/QScreen>

#include "GetItemVisitor.h"
#include "VPPItemFactory.h"
#include "VppCustomPlotWidget.h"
#include "VPPDialogs.h"
#include "VPPJacobian.h"
#include "VPPGradient.h"
#include "VPPSolverFactoryBase.h"
#include "Version.h"
#include "VppPolarCustomPlotWidget.h"
#include "VPPSailCoefficientIO.h"
#include "VPPSettingsDialog.h"
#include "VariableTreeModel.h"
#include "VppToolbarAction.h"

// Stream used to redirect cout to the log window
// This object is explicitly deleted in the destructor
// of MainWindow
boost::shared_ptr<QDebugStream> pQstream;

// Init static members : sail coeffs browser used to define new sail coeffs
boost::shared_ptr<VPPDefaultFileBrowser> MainWindow::pSailCoeffFileBrowser_= 0;

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

	// Add the menu-bar item for the variable widget
	pWidgetMenu_->addAction(pVariablesWidget_->getMenuToggleViewAction());

	// --

	// Set the toolbar. In this case NO horizontal toolbar.
	setupToolBar();

	// Message that will appear in the status (lower) bar
	statusBar()->showMessage(tr("Status Bar"));

	// Set the menu bar: File, edit...
	setupMenuBar();

	// Make sure the solver factory is empty
	pSolverFactory_.reset();

	// Instantiates a VppSettingsDialog (singleton) and
	// sync the variableTree with the VppSettingsDialog
	VPPSettingsDialog* pSd = VPPSettingsDialog::getInstance(this);

	udpateVariableTree();

}

// Virtual destructor
MainWindow::~MainWindow() {

	// Make sure the cout stream redirection class is deleted
	pQstream.reset();
}

void MainWindow::setupMenuBar() {

	// Generic handle for actions
	VppToolbarAction* pAction= NULL;

	// Create 'Import Settings' action. This object being child of 'this', it will
	// be destroyed when MainWindows gets destroyed.
	pAction = new VppToolbarAction("Import Settings and previous results",":/icons/importSettings.png",this);
	connect(pAction, &QAction::triggered, this, &MainWindow::importResults);

	// Open Settings...
	pAction = new VppToolbarAction("Settings",":/icons/openSettings.png",this);
	connect(pAction, &QAction::triggered, this, &MainWindow::openSettings);

	// Run...
	pAction = new VppToolbarAction("Run",":/icons/run.png",this);
	connect(pAction, &QAction::triggered, this, &MainWindow::run);

	// Get Result Table...
	pAction = new VppToolbarAction("Result table",":/icons/tabularResults.png",this);
	connect(pAction, &QAction::triggered, this, &MainWindow::tableResults);

	// --

	// Add a menu in the toolbar. This is used to group polar and XY result plots
	VppToolbarMenu* pPlotResultsMenu(new VppToolbarMenu("Plot polars",":/icons/plotPolars.png",this));

	// Plot Polars...
	pAction = new VppToolbarAction("Plot polars",":/icons/plotPolars.png",pPlotResultsMenu);
	connect(pAction, &QAction::triggered, this, &MainWindow::plotPolars);

	// Plot XY...
	pAction = new VppToolbarAction("Plot XY",":/icons/plotXY.png",pPlotResultsMenu);
	connect(pAction, &QAction::triggered, this, &MainWindow::plotXY);

	// --

	// Add a menu in the toolbar. This is used to group plots for plot coeffs, and their derivatives
	VppToolbarMenu* pPlotSailCoeffsMenu(new VppToolbarMenu("Plot Sail Coeffs",":/icons/sailCoeffs.png",this));

	// Plot Sail Coeffs...
	pAction = new VppToolbarAction("Plot Sail Coeffs",":/icons/sailCoeffs.png",pPlotSailCoeffsMenu);
	connect(pAction, &QAction::triggered, this, &MainWindow::plotSailCoeffs);

	// Plot Sail Coeffs Derivatives...
	pAction = new VppToolbarAction("Plot Sail Coeffs Derivatives",":/icons/d_sailCoeffs.png",pPlotSailCoeffsMenu);
	connect(pAction, &QAction::triggered, this, &MainWindow::plot_d_SailCoeffs);

	// Plot Sail Coeffs Second Derivatives...
	pAction = new VppToolbarAction("Plot Sail Coeffs Second Derivatives",":/icons/d2_sailCoeffs.png",pPlotSailCoeffsMenu);
	connect(pAction, &QAction::triggered, this, &MainWindow::plot_d2_SailCoeffs);

	// Plot Sail Forces...
	pAction = new VppToolbarAction("Plot Sail Force/Moments",":/icons/sailForceMoment.png",pPlotSailCoeffsMenu);
	connect(pAction, &QAction::triggered, this, &MainWindow::plotSailForceMoments);

	// --

	// Add a menu in the toolbar. This is used to group resistance plots
	VppToolbarMenu* pPlotResistanceMenu(new VppToolbarMenu("Plot Resistance",":/icons/resistanceComponent.png",this));

	// Plot Total Resistance...
	pAction = new VppToolbarAction("Plot Total Resistance",":/icons/totResistance.png",pPlotResistanceMenu);
	connect(pAction, &QAction::triggered, this, &MainWindow::plotTotalResistance);

	// Plot Viscous Resistance...
	pAction = new VppToolbarAction("Plot Viscous Resistance",":/icons/viscousResistance.png",pPlotResistanceMenu);
	connect(pAction, &QAction::triggered, this, &MainWindow::plotViscousResistance);

	// Plot Delta Viscous Resistance due to Heel...
	pAction = new VppToolbarAction("Plot Delta Viscous Resistance due to Heel",":/icons/DeltaFrictRes_Heel.png",pPlotResistanceMenu);
	connect(pAction, &QAction::triggered, this, &MainWindow::plotDelta_ViscousResistance_Heel);

	// Plot Induced Resistance...
	pAction = new VppToolbarAction("Plot Induced Resistance",":/icons/inducedResistance.png",pPlotResistanceMenu);
	connect(pAction, &QAction::triggered, this, &MainWindow::plotInducedResistance);

	// Plot Residuary Resistance...
	pAction = new VppToolbarAction("Plot Residuary Resistance",":/icons/residuaryResistance.png",pPlotResistanceMenu);
	connect(pAction, &QAction::triggered, this, &MainWindow::plotResiduaryResistance);

	// Plot Negative Resistance...
	pAction = new VppToolbarAction("Plot Negative Resistance",":/icons/negativeResistance.png",pPlotResistanceMenu);
	connect(pAction, &QAction::triggered, this, &MainWindow::plotResiduaryResistance);

	// --

	// Plot the 3d optimization space...
	pAction = new VppToolbarAction("Plot Optimization Space",":/icons/plot3d.png",this);
	connect(pAction, &QAction::triggered, this, &MainWindow::plotOptimizationSpace);

	//	 Plot the gradient of the solution...
	pAction = new VppToolbarAction("Plot Gradient",":/icons/plotGradient.png",this);
	connect(pAction, &QAction::triggered, this, &MainWindow::plotGradient);

	// Plot the Jacobian of the solution
	pAction = new VppToolbarAction("Plot Jacobian",":/icons/plotJacobian.png",this);
	connect(pAction, &QAction::triggered, this, &MainWindow::plotJacobian);

	// --

	// Save Results...
	pAction = new VppToolbarAction("Save results",":/icons/saveResults.png",this);
	connect(pAction, &QAction::triggered, this, &MainWindow::saveResults);

	// ---

	pPreferencesMenu_.reset( menuBar()->addMenu(tr("&VPP Settings")) );
	pPreferencesMenu_->addAction(tr("&Import Sail Coefficients"), this, &MainWindow::importSailCoeffs);

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
void MainWindow::removeTabWidgetFromVector(VppTabDockWidget* pWidget) {

	for(std::vector<VppTabDockWidget*>::iterator it=tabbedWidgets_.begin(); it!=tabbedWidgets_.end(); it++){

		// Remove the ptr to a destroyed widget
		if(*it == pWidget)
			tabbedWidgets_.erase(it);
	}
}

QString MainWindow::importData(string filter) {

	QString caption;
	QString dir;

	// try getting the settings from file
	QString fileName = QFileDialog::getOpenFileName(this,caption,dir,
			tr(filter.c_str()));

	if (!fileName.isEmpty())
		std::cout<<"Importing data from : "<<fileName.toStdString()<<std::endl;

	// This is the file we are about to read
	QFile sourceFile(fileName);
	if (!sourceFile.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("VppSettings Import"),
				tr("Cannot read file %1:\n%2.")
				.arg(fileName)
				.arg(sourceFile.errorString()));
		return QString();
	}

	VPPSettingsDialog* pSd = VPPSettingsDialog::getInstance(this);
	pSd->read(sourceFile);

	// Sync the variable tree with the vppSettingsDialog
	udpateVariableTree();

	return fileName;
}

void MainWindow::openSettings() {

	try {

		// Open up a VPP settings dialog
		VPPSettingsDialog* pSd = VPPSettingsDialog::getInstance(this);

		// Sync the variable Ctree with the settings window
		udpateVariableTree();

		// show the SettingsDialog
		pSd->exec();

	}	catch(...) {}
}


// Updates the variable tree getting values from the
// settingsWindow tree tab
void MainWindow::udpateVariableTree() {

	// Open up a VPP settings dialog
	VPPSettingsDialog* pSd = VPPSettingsDialog::getInstance(this);

	// Instantiate a variableFileParser (and clear any previous one)
	// on top of the VPP Settings Dialog. The parser will get populated
	// with all the variables edited in the settings
	pVariableFileParser_.reset( new VariableFileParser(pSd) );

	// The variable file parser populates the variable item tree
	pVariableFileParser_->populate( pVariablesWidget_->getTreeModel() );

	// Expand the items in the variable tree view, in order to see all the variables
	pVariablesWidget_->getView()->expandAll();

}


void MainWindow::run() {

	try {

		// Verify if the variable values aCre within the allowed ranges
		pVariableFileParser_->check();

		// before each run we rebuild the items with the latest settings
		// entered by the user. Actually one should create new items each
		// time the settings change. But this seems relatively heavy to do
		updateVppItems();

		// todo dtrimarchi : this should be selected by a switch in the UI!
		// Instantiate a solver by default. This can be an optimizer (with opt vars)
		// or a simple solver that will keep fixed the values of the optimization vars
		//pSolverFactory_.reset( new Optim::SolverFactory(pVppItems_) );
		//pSolverFactory_.reset( new Optim::NLOptSolverFactory(pVppItems_) );
		//pSolverFactory_.reset( 	new Optim::SAOASolverFactory(pVppItems_) );
		pSolverFactory_.reset( new Optim::IppOptSolverFactory(pVppItems_) );

		std::cout<<"Running the VPP analysis... "<<std::endl;

		// Instantiate a progress dialog. Also instantiate a
		// count to update the bar progression
		QProgressDialog progressDialog(this);
		size_t maxVal=
				pVariableFileParser_->get(Var::nta_);
				pVariableFileParser_->get(Var::ntw_);
		progressDialog.setRange(0,maxVal);
		progressDialog.setCancelButtonText(tr("&Cancel"));
		progressDialog.setWindowTitle(tr("Running VPP analysis..."));

		int statusProgress=0;

		// Loop on the wind ANGLES and VELOCITIES
		for(int aTW=0; aTW<pVariableFileParser_->get(Var::nta_); aTW++) {

			// exit the outer loop if the user pressed the 'cancel' button
			if (progressDialog.wasCanceled())
				break;

			for(int vTW=0; vTW<pVariableFileParser_->get(Var::ntw_); vTW++){

				try{

					// Run the optimizer for the current wind speed/angle
					pSolverFactory_->run(vTW,aTW);

					// Refresh the UI -> update the progress bar and the log
					QCoreApplication::processEvents();

					progressDialog.setValue(statusProgress);
					progressDialog.setLabelText(tr("_ Solving case number %1 of %n...", 0, maxVal).arg(statusProgress));

					statusProgress++;

				}
				catch(...){ /* do nothing and keep going */ }
			}
		}
		// outer try-catch block
	}	catch(...) { /* do nothing */ }
}

void MainWindow::saveResults() {

	try {

		//-- Save the UI settings
		QFileDialog dialog(this);
		dialog.setWindowModality(Qt::WindowModal);
		dialog.setAcceptMode(QFileDialog::AcceptSave);
		dialog.setNameFilter(tr("VPP Result File(*.xml)"));
		dialog.setDefaultSuffix(".xml");
		if (dialog.exec() != QDialog::Accepted)
			return;

		// Get the file selected by the user
		QString fileName(dialog.selectedFiles().first());

		// Introduce a scope because the settings are saved to
		// QFile when the file is closed : at the destruction
		// of the QFile. Neglecting to do so implies that the
		// settings data are written at the very end of the method.
		// Since the settings write clear the file, this must be
		// done as first. Otherwise any other data will be destroyed
		{
			QFile file(fileName);

			if (!file.open(QFile::WriteOnly | QFile::Text)) {
				QMessageBox::warning(this, tr("Saving Vpp Settings"),
						tr("Cannot write file %1:\n%2.")
						.arg(fileName)
						.arg(file.errorString()));
				return;
			}

			// Get the settings tree and save it to xml file
			VPPSettingsDialog::getInstance()->save(file);
		}

		// If some results are available, save them
		if(pSolverFactory_)
			if(pSolverFactory_->get()->getResults() ) {
				//-- Now save the results using the old interface. There is no need
				// to store results in xml format
				pSolverFactory_->get()->saveResults(fileName.toStdString());
			}
		// outer try-catch block
	}	catch(...) {}

}

// Import VPP results from file
void MainWindow::importResults() {

	try {

		QString fileName= importData("VPP Result File(*.xml);; All Files (*.*)");

		// Instantiate the sailset. Note that the variableFileParser has already
		// been updated while importing the settings
		pSails_.reset( SailSet::SailSetFactory( *pVariableFileParser_ ) );

		// Instantiate the items
		pVppItems_.reset( new VPPItemFactory(pVariableFileParser_.get(),pSails_) );

		// Instantiate a new solverFactory without vppItems_
		pSolverFactory_.reset( new Optim::NLOptSolverFactory(pVppItems_) );

		// And now import the results from the file
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


// Given the settings, instantiates (refreshes) all of the
// VPP items: sailItems, resistanceItems... All is required
// to run an analysis
void MainWindow::updateVppItems() {

	// Instantiate the sailset
	pSails_.reset( SailSet::SailSetFactory( *pVariableFileParser_ ) );

	// Instantiate the items
	pVppItems_.reset( new VPPItemFactory(pVariableFileParser_.get(),pSails_) );

	// SailSet also contains several variables. Append them to the bottom
	pSails_->populate( pVariablesWidget_->getTreeModel() );
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

// Run a VPPDefaultFileBrowser to select a sail coefficient file
// OR leave the default sail coeffs in place
void MainWindow::importSailCoeffs() {

	if(!hasBoatDescription())
		return;

	if(!pSailCoeffFileBrowser_)
		pSailCoeffFileBrowser_.reset( new VPPDefaultFileBrowser(
				"Sail coefficient file browser",
				tr("Sail Coeffs Input File(*.sailCoeff);; All Files (*.*)"),
				this)
		);

	// Show the dialog and block the rest of the application
	pSailCoeffFileBrowser_->exec();

	// Get the sailCoeffsItem that owns the VPP_CL/CD_IOs
	SailCoefficientItem* pSailCoeffItem= pVppItems_->getSailCoefficientItem();

	// The IO containers will parse the coeff file and override the current coeffs
	pSailCoeffItem->getClIO()->parse( pSailCoeffFileBrowser_->getSelectedFileName() );
	pSailCoeffItem->getCdIO()->parse( pSailCoeffFileBrowser_->getSelectedFileName() );

	// Remember to refresh the spline interpolators with the new coefficient arrays
	pSailCoeffItem->interpolateCoeffs();

	return;

}

// Plot the velocity polars
void MainWindow::plotPolars() {

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
		if(pPolarPlotWidget_)
			delete pPolarPlotWidget_;

		pPolarPlotWidget_= new MultiplePlotWidget(this, "Polars");

		// Hand the multiplePlotwidget to the solver factory that stores the results and knows how to plot them
		pSolverFactory_->get()->plotPolars( pPolarPlotWidget_ );

		// Add the polar plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pPolarPlotWidget_);

		// Tab the widget if other widgets have already been instantiated
		// In the same area. Todo dtrimarchi : this is way too fragile
		// It requires widgets instantiated on the topDockWidgetArea and
		// I need to add the deleted signal to the slot removeWidgetFromVector
		tabDockWidget(pPolarPlotWidget_);

		// outer try-catch block
	}	catch(...) {}
}

// Plot XY results
void MainWindow::plotXY() {

	try{

		// If the boat description has not been imported we do not have the
		// vppItems nor the coefficients to be plotted!
		// If the boat description has not been imported we do not have the
		// vppItems nor the coefficients to be plotted!
		if(!hasBoatDescription())
			return;

		if(!hasSolver())
			return;

		// For which TWV, TWA shall we plot the aero forces/moments?
		WindIndicesDialog wd(pVppItems_->getWind());
		if (wd.exec() == QDialog::Rejected)
			return;

		std::cout<<"Plotting XY Results..."<<std::endl;

		// Instantiate a graphic plotting window in the central widget
		if(pXYPlotWidget_)
			delete pXYPlotWidget_;

		pXYPlotWidget_= new MultiplePlotWidget(this, "XY plot");

		// Hand the multiplePlotwidget to the solver factory that stores the results and knows how to plot them
		pSolverFactory_->get()->plotXY( pXYPlotWidget_, wd );

		// Add the polar plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pXYPlotWidget_);

		// Tab the widget if other widgets have already been instantiated
		// In the same area. Todo dtrimarchi : this is way too fragile
		// It requires widgets instantiated on the topDockWidgetArea and
		// I need to add the deleted signal to the slot removeWidgetFromVector
		tabDockWidget(pXYPlotWidget_);

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
		if(pSailCoeffPlotWidget_)
			delete pSailCoeffPlotWidget_;
		pSailCoeffPlotWidget_= new MultiplePlotWidget(this,"Sail Coeffs");

		// Hand the multiple plot to the plot method of the sailCoeffs that knows how to plot
		pVppItems_->getSailCoefficientItem()->plotInterpolatedCoefficients( pSailCoeffPlotWidget_ );

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pSailCoeffPlotWidget_ );

		tabDockWidget(pSailCoeffPlotWidget_);

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
		if(p_d_SailCoeffPlotWidget_)
			delete p_d_SailCoeffPlotWidget_;

		p_d_SailCoeffPlotWidget_= new MultiplePlotWidget(this,"d(Sail Coeffs)");

		// Hand the multiple plot to the plot method of the sailCoeffs that knows how to plot
		pVppItems_->getSailCoefficientItem()->plot_D_InterpolatedCoefficients( p_d_SailCoeffPlotWidget_ );

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, p_d_SailCoeffPlotWidget_ );

		tabDockWidget(p_d_SailCoeffPlotWidget_);

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
		if(p_d2_SailCoeffPlotWidget_)
			delete p_d2_SailCoeffPlotWidget_;

		p_d2_SailCoeffPlotWidget_= new MultiplePlotWidget(this,"d2(Sail Coeffs)");

		// Hand the multiple plot to the plot method of the sailCoeffs that knows how to plot
		pVppItems_->getSailCoefficientItem()->plot_D2_InterpolatedCoefficients( p_d2_SailCoeffPlotWidget_ );

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, p_d2_SailCoeffPlotWidget_);

		tabDockWidget(p_d2_SailCoeffPlotWidget_);
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
		if(pForceMomentsPlotWidget_)
			delete pForceMomentsPlotWidget_;

		pForceMomentsPlotWidget_= new MultiplePlotWidget(this,"Sail Force/Moments");

		// Ask the forces/moments to plot themself
		pVppItems_->getAeroForcesItem()->plot(pForceMomentsPlotWidget_);

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pForceMomentsPlotWidget_ );

		// Tab the widget with the others
		tabDockWidget(pForceMomentsPlotWidget_);

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
		if(pTotResistancePlotWidget_)
			delete pTotResistancePlotWidget_;

		pTotResistancePlotWidget_= new MultiplePlotWidget(this,"Total Resistance");

		// Ask the forces/moments to plot themself
		pTotResistancePlotWidget_->addChart(
				pVppItems_->plotTotalResistance(&wd,&sd)[0],
				0,0);

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pTotResistancePlotWidget_);

		// Tab the widget with the others
		tabDockWidget(pTotResistancePlotWidget_);

		// outer try-catch block
	}	catch(...) {}

}

/// Plot the Viscous resistance of the hull, the keel and the rudder
void MainWindow::plotViscousResistance() {

	try{

		if(!hasBoatDescription())
			return;

		std::cout<<"Plotting Viscous Resistance..."<<std::endl;

		// Instantiate an empty multiple plot widget
		if(pViscousResistancePlotWidget_)
			delete pViscousResistancePlotWidget_;

		pViscousResistancePlotWidget_= new MultiplePlotWidget(this,"Viscous Resistance");

		// Add the plots generated by the items into the multiPlotWidget
		pViscousResistancePlotWidget_->addChart(
				pVppItems_->getViscousResistanceItem()->plot()[0],
				0,0);

		pViscousResistancePlotWidget_->addChart(
				pVppItems_->getViscousResistanceKeelItem()->plot()[0],
				0,1);

		pViscousResistancePlotWidget_->addChart(
				pVppItems_->getViscousResistanceRudderItem()->plot()[0],
				0,2);

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pViscousResistancePlotWidget_ );

		// Tab the widget with the others
		tabDockWidget(pViscousResistancePlotWidget_);

		// outer try-catch block
	}	catch(...) {}
}

// Plot the delta Viscous resistance due to the heel of the hull
void MainWindow::plotDelta_ViscousResistance_Heel() {

	try{
		if(!hasBoatDescription())
			return;

		std::cout<<"Plotting Delta Viscous Resistance due to Heel..."<<std::endl;

		// For which TWV, TWA shall we plot the residuary resistance?
		WindIndicesDialog dg(pVppItems_->getWind());
		if (dg.exec() == QDialog::Rejected)
			return;

		if(p_dFrictRes_HeelPlotWidget_)
			delete p_dFrictRes_HeelPlotWidget_;

		// Instantiate an empty multiple plot widget
		p_dFrictRes_HeelPlotWidget_= new MultiplePlotWidget(this,"Delta Viscous Resistance due to Heel");

		// Plot the change of wetted area due to heel - see DSYHS99 p 116
		p_dFrictRes_HeelPlotWidget_->addChart(
				pVppItems_->getDelta_ViscousResistance_HeelItem()->plot_deltaWettedArea_heel()[0],
				0,0);

		// Add the plot for the Delta Viscous Resistance due to Heel
		p_dFrictRes_HeelPlotWidget_->addChart(
				pVppItems_->getDelta_ViscousResistance_HeelItem()->plot(&dg)[0],
				1,0);

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, p_dFrictRes_HeelPlotWidget_ );

		// Tab the widget with the others
		tabDockWidget(p_dFrictRes_HeelPlotWidget_);

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
		if(pInducedResistancePlotWidget_)
			delete pInducedResistancePlotWidget_;

		pInducedResistancePlotWidget_= new MultiplePlotWidget(this,"Induced Resistance");

		std::vector<VppXYCustomPlotWidget*> resCharts= pVppItems_->getInducedResistanceItem()->plot(&wd,&vd);

		// Ask the induced resistance item to plot itself
		pInducedResistancePlotWidget_->addChart(resCharts[0],0,0);

		// Ask the induced resistance item to plot itself
		pInducedResistancePlotWidget_->addChart(resCharts[1],0,1);

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pInducedResistancePlotWidget_ );

		// Tab the widget with the others
		tabDockWidget(pInducedResistancePlotWidget_);

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

		if(pResiduaryResistancePlotWidget_)
			delete pResiduaryResistancePlotWidget_;

		// Instantiate an empty multiple plot widget
		pResiduaryResistancePlotWidget_= new MultiplePlotWidget(this,"Residuary Resistance");

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
		addDockWidget(Qt::TopDockWidgetArea, pResiduaryResistancePlotWidget_ );

		// Tab the widget with the others
		tabDockWidget(pResiduaryResistancePlotWidget_);

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
		if(pNegativeResistancePlotWidget_)
			delete pNegativeResistancePlotWidget_;

		pNegativeResistancePlotWidget_= new MultiplePlotWidget(this,"Negative Resistance");

		// Ask the negative resistance item to plot itself
		pNegativeResistancePlotWidget_->addChart(
				pVppItems_->getNegativeResistanceItem()->plot()[0],
				0,0);

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pNegativeResistancePlotWidget_ );

		// Tab the widget with the others
		tabDockWidget(pNegativeResistancePlotWidget_);

		// outer try-catch block
	}	catch(...) {}
}

// Temp method used to test QCustomPlot in the current env
// This method shall be removed at some point todo dtrimarchi
void MainWindow::testQCustomPlot() {

	try{
		// Init the widget that will be containing this plot
		pCustomPlotWidget_.reset( new VppCustomPlotWidget(this) );

		// Add the xy plot view to the left of the app window
		addDockWidget(Qt::TopDockWidgetArea, pCustomPlotWidget_.get() );

		// Tabify the dockwidget
		tabDockWidget(pCustomPlotWidget_.get());

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

		if(pGradientPlotWidget_)
			delete pGradientPlotWidget_;

		pGradientPlotWidget_= new MultiplePlotWidget(this,"VPP Gradient");

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
		addDockWidget(Qt::TopDockWidgetArea, pGradientPlotWidget_);

		// Tab the widget if other widgets have already been instantiated
		// In the same area.
		tabDockWidget(pGradientPlotWidget_);

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
		if(pJacobianPlotWidget_)
			delete pJacobianPlotWidget_;

		pJacobianPlotWidget_= new MultiplePlotWidget(this,"VPP Jacobian");

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
		addDockWidget(Qt::TopDockWidgetArea, pJacobianPlotWidget_);

		// Tab the widget if other widgets have already been instantiated
		// In the same area.
		tabDockWidget(pJacobianPlotWidget_);

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

// Get the toolbar with some shortcuts to actions
QToolBar* MainWindow::getToolBar() {
	return pToolBar_;
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
