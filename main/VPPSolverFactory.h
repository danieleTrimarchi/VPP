#ifndef VPP_SOLVER_FACTORY
#define VPP_SOLVER_FACTORY

#include "VPPItemFactory.h"
#include "boost/shared_ptr.hpp"
#include "VPPSolverBase.h"

/// ipOpt integration
#include "IpIpoptApplication.hpp"
#include "VPP_nlp.h"
using namespace Ipopt;

/// Factory class in charge for the instantiation
/// and storage of a solver / optimizer for the
/// solution of the VPP problem.
/// The class was made necessary when building the ipOpt
/// integration, in order to create a unique container
/// storing the problem representation (VPP_nlp) and the
/// solving application (IpoptApplication). Other solvers/
/// optimizers are also instantiated from here.
class VPPSolverFactory {

	public:

		/// Ctor
		VPPSolverFactory(boost::shared_ptr<VPPItemFactory>);

		/// Virtual Dtor
		virtual ~VPPSolverFactory();

		/// Returns a reference to the underlying problem representation
		virtual VPPSolverBase* get() const =0;

		/// Pure virtual used to execute a VPP-like analysis
		virtual void run(int TWV, int TWA) =0;

	protected:

		/// Disallow default constructor
		VPPSolverFactory();

		/// Ptr to the item factory
		boost::shared_ptr<VPPItemFactory> pVppItems_;

};

/// Wraps up the ipOpt integration
class IppOptSolverFactory : public VPPSolverFactory {

	public:

		/// Ctor
		IppOptSolverFactory(boost::shared_ptr<VPPItemFactory>);

		/// Virtual Dtor
		virtual ~IppOptSolverFactory();

		/// Implement pure virtual declared in the mother class
		/// Returns a reference to the underlying problem representation
		VPP_NLP* get() const;

		/// Implement pure virtual used to execute a VPP-like analysis
		virtual void run(int TWV, int TWA);

	private:

		/// Ptr to the problem representation
		SmartPtr<VPP_NLP> pSolver_;

		/// Ptr to the ipOpt solver
		SmartPtr<IpoptApplication> pApp_;

};

#endif

