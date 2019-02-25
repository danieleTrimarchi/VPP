#include "VPPJobRunner.h"
#include "MainWindow.h"
#include <QObject>

// Ctor
VPPJobRunner::VPPJobRunner(VPPSolverFactoryBase* pSf,
		size_t nta, size_t ntw, MainWindow* ui/*=0*/):
		pSf_(pSf),
		nta_(nta),
		ntw_(ntw) {

	// If we have a UI, we instantiate and initialize a progress
	// dialog
	if(ui) {
		pProgress_.reset(new QProgressDialog(ui));
		pProgress_->setRange(0,nta_*ntw_);
		pProgress_->setCancelButtonText(QObject::tr("&Cancel"));
		pProgress_->setWindowTitle(QObject::tr("Running VPP analysis..."));
	}

	int statusProgress=0;

	// Loop on the wind ANGLES and VELOCITIES
	for(size_t aTW=0; aTW<nta_; aTW++){

		// exit the outer loop if the user pressed the 'cancel' button
		if (pProgress_ && pProgress_->wasCanceled())
			break;

		for(size_t vTW=0; vTW<ntw_; vTW++){

			try{

				std::cout<<"vTW="<<vTW<<"  "<<"aTW="<<aTW<<std::endl;

				// Run the optimizer for the current wind speed/angle
				pSf_->run(vTW,aTW);

				if(pProgress_) {

					// Refresh the UI -> update the progress bar and the log
					QCoreApplication::processEvents();

					pProgress_->setValue(statusProgress);
					pProgress_->setLabelText(QObject::tr("_ Solving case number %1 of %n...", 0, nta_*ntw_).arg(statusProgress));

				}
				statusProgress++;

			} catch(VPPException& e){
				// Print the message and keep going...
				std::cout<<"A VPPException was catched..."<<std::endl;
				std::cout<<e.what()<<std::endl;
				break;
			}
			catch(NonConvergedException& e) {
				std::cout<<"A NonConvergedException was catched..."<<std::endl;
				std::cout<<e.what()<<std::endl;
				// Do nothing and keep going
			} catch(...){
				std::cout<<"An unknown exception was catched..."<<std::endl;
				break;
			}
		}
	}
}

// Dtor
VPPJobRunner::~VPPJobRunner() {

}
