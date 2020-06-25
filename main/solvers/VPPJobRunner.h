#ifndef __JOB_RUNNER__
#define __JOB_RUNNER__

# include "VPPSolverFactoryBase.h"

using namespace Optim;

/// Forward declare
class MainWindow;

/// Class used to run jobs. The advantage of this class is -by now-
/// simply to centralize the exception handling
class VPPJobRunner {

	public:

		/// Ctor
		VPPJobRunner(VPPSolverFactoryBase* pSf, size_t nta, size_t ntw, MainWindow* ui=Q_NULLPTR);

		/// Dtor
		virtual ~VPPJobRunner();

	private:

		/// Ptr to the Solver
		VPPSolverFactoryBase* pSf_;

		/// Number of points to process
		size_t nta_, ntw_;

		/// For the version called by the UI, issue a QProgressDialog
		/// in charge for showing at what point of the analysis we are
		/// at
		std::shared_ptr<QProgressDialog> pProgress_;
};

#endif
