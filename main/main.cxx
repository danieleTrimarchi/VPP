#include <stdio.h>
#include <iostream>
#include <fstream>
#include <getopt.h>

using namespace std;

// ------------------------
// Directives for EIGEN
#include <Eigen/Core>
using namespace Eigen;


#include "VariableFileParser.h"
#include "SailSet.h"
#include "VPPItem.h"
#include "Regression.h"
#include "Interpolator.h"
#include "VPPException.h"
#include "mathUtils.h"
#include "Version.h"
#include "VPPResultIO.h"
#include "VPPSolverFactoryBase.h"

#include <QApplication>
#include "MainWindow.h"

// MAIN
int main(int argc, char** argv) {

	// Instantiate the utilities to get the external resources such
	// as the icons
	Q_INIT_RESOURCE(VPP);

	try {

		// Instantiate a Qt application that will be executed at the end of main
		QApplication app(argc, argv);

		//
		QDir dir(QCoreApplication::applicationDirPath());
		dir.cdUp();
		dir.cd("PlugIns/platforms");
		QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));

		// Instantiate the MainWindow of the VPP application
		MainWindow mainWin;
		mainWin.resize(800, 600);
		mainWin.show();

		// Execute the application
		return app.exec();

	} catch(std::exception& e) {
		std::cout<<"\n-----------------------------------------"<<std::endl;
		std::cout<<" Exception caught in Main:  "<<std::endl;
		std::cout<<" --> "<<e.what()<<std::endl;
		std::cout<<" The program is terminated. "<<std::endl;
		std::cout<<"-----------------------------------------\n"<<std::endl;
	}	catch(...) {
		cout << "Unknown Exception occurred\n";
	}

}

